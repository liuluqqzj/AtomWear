/* Copyright (c) 2012 Giayee Technology Co. Ltd. All Rights Reserved.
 *
 * Author: LiuLu
 * Email:  liuluqqzj@foxmail.com
 *
 * The information contained herein is property of Giayee Technology Co. Ltd.
 * Terms and conditions of usage are described in detail in GIAYEE STANDARD 
 * SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
 
#include "stdint.h"
#include "stdbool.h"
#include <string.h>
#include "system_info_storage.h"
#include "flash_ctrl.h"
#include "nrf51.h"
#include "nrf51_bitfields.h"
#include "xprintf.h"

/*---------------------- �洢���� -------------------------------*/

/*---------------------- ϵͳ���� -------------------------------*/
volatile uint8_t system_battery_level    = 0;
volatile bool    system_charging_state     = false;

/*--------------------- ϵͳ����ʱ�� ----------------------------*/
volatile uint32_t SYSTEM_LOCAL_TIMING = 0;
volatile uint32_t SYSTEM_NO_SYNC_TIMING = 0;

/*------------------ �������ݴ洢��ʽ -------------------------*/
uint8_t pedo_data[2048];

/*---------------------- ϵͳ�汾�� -----------------------------*/
static uint16_t system_app_version = 0x0100;      // version 1.0

/*-------------------- �Ʋ��洢�����ṹ�� -----------------------*/
struct pedo_struct
{
    uint32_t sto_pointer;           // ָ����һ��Ҫ�洢������
    bool slice_handle;              // ��ʶ�Ƿ���ʱ��Ƭ�ڲ��� 
    
    uint32_t diff_pointer[20];     // ���� pedo_data����Ҫ�洢���ʱ�������λ��
    uint8_t  diff_count;            // ��Ҫ�洢���ʱ����������
} pedo_struct;
struct pedo_struct pedo_ctrl;

/*--------------------app��������bootloader������ַ-------------------------*/
uint32_t system_parameters_settings[256] __attribute__((at(SYSTEM_PARAMETERS_SETTINGS_ADDRESS))) __attribute__((used));      
uint32_t bootloader_start_address __attribute__((at(NRF_UICR_BOOT_START_ADDRESS)));



/*----------------------ϵͳ����ģʽ-----------------------------------*/
volatile uint8_t SYSTEM_WORK_MODE  = 0x00;
volatile bool SYSTEM_WORK_MODE_SWITCH = false; 

/*------------------------ϵͳ���ߵ���ʱ---------------------*/
volatile uint32_t SYSTEM_SLEEP_COUNTDOWN = 0;

/*--------------------------- �洢���� --------------------------------*/


/*------------------------- ϵͳ������ʼ�� ----------------------------*/
void pedo_data_ctrl_init(void);

void sys_parameters_init()
{
    system_battery_level  = 0;
    system_charging_state   = false;

    SYSTEM_LOCAL_TIMING   = 0;
    SYSTEM_NO_SYNC_TIMING = 0;

    memset(pedo_data, 0, 2048);

    system_app_version    = 0x0100;      // version 1.0

    pedo_data_ctrl_init();

    SYSTEM_WORK_MODE      = SYSTEM_DEFAULT_MODE;

    SYSTEM_SLEEP_COUNTDOWN   = 0;
}



/*------------------------- ϵͳ��׼ʱ���ж� -------------------------*/
/* true:  relative time
 * false: absolute time
 * offset from year:2000/0/0/0:0
 */
bool standard_time_judge()
{
    if(SYSTEM_LOCAL_TIMING > 0xA39376E1598000)  // 2000/0/0/0
        return true;

    return false;
}

/*-------------------------- ���طǱ�׼ʱ�� ---------------------------*/
uint32_t get_no_sync_time()
{
    return SYSTEM_NO_SYNC_TIMING;
}

/*-------------------------- �洢�Ǳ�׼ʱ�� ---------------------------*/
void sto_no_sync_time()
{
    SYSTEM_NO_SYNC_TIMING = SYSTEM_LOCAL_TIMING;
}

/*-------------------------- ���ر�׼ʱ�� ---------------------------*/
uint32_t get_system_time()
{
    return SYSTEM_LOCAL_TIMING;
}

/*-------------------------- ˢ�±�׼ʱ�� ---------------------------*/
void system_time_update()
{
    SYSTEM_LOCAL_TIMING++;
}

/*-------------------------- ͬ��������׼ʱ�� ---------------------------*/
void set_system_time(uint32_t std_time)
{
    SYSTEM_LOCAL_TIMING = std_time;
}

/*-------------------------- ����ϵͳ���ߵ���ʱ ---------------------------*/
uint32_t get_sleep_countdown()
{
    return SYSTEM_SLEEP_COUNTDOWN;
}

/*-------------------------- ˢ��ϵͳ���ߵ���ʱ ---------------------------*/
void system_countdown_update()
{
    SYSTEM_SLEEP_COUNTDOWN++;
}

/*-------------------------- ϵͳ���ߵ���ʱ���� ---------------------------*/
void clear_sleep_countdown()
{
    SYSTEM_SLEEP_COUNTDOWN = 0;
}

/*-------------------------- ����ϵͳ�汾�� ---------------------------*/
uint16_t get_system_app_version()
{
    return system_app_version;
}

/*--------------------------- ����ϵͳ������Ϣ -------------------------*/
void set_battery_level(uint8_t level)
{
    system_battery_level = level;
}

/*--------------------------- ����ϵͳ������Ϣ -------------------------*/
uint8_t get_battery_level()
{
    return system_battery_level;
}

/*--------------------------- ����ϵͳ���״̬ -------------------------*/
void set_charging_level(bool state)
{
    system_charging_state = state;
}

/*--------------------------- ����ϵͳ���״̬ -------------------------*/
bool get_charging_level()
{
    return system_charging_state;
}
   
/*--------------------------- ϵͳ����״̬���� -------------------------*/
void work_mode_switch()
{
    switch(SYSTEM_WORK_MODE)
    {
        case SYSTEM_PEDOMETER_MODE :
            SYSTEM_WORK_MODE = SYSTEM_USER_SLEEP_MODE;
        break;
                
        case SYSTEM_USER_SLEEP_MODE:
             SYSTEM_WORK_MODE = SYSTEM_BLE_WORK_MODE;
        break;

        case SYSTEM_BLE_WORK_MODE:
             SYSTEM_WORK_MODE = SYSTEM_PEDOMETER_MODE;
        break;
     }
}

/*--------------------------- ����ϵͳ����״̬ -------------------------*/
uint8_t get_work_mode()
{
    return SYSTEM_WORK_MODE;
}

/*--------------------------- ��ʼ��ϵͳ����״̬ -----------------------*/
void work_mode_init()
{
    SYSTEM_WORK_MODE = SYSTEM_DEFAULT_MODE;
}

/*--------------------------- ����ϵͳ�л�״̬ -------------------------*/
bool get_work_switch_state()
{
    return SYSTEM_WORK_MODE_SWITCH;
}

/*--------------------------- ��λϵͳ�л�״̬ -----------------------*/
void work_switch_set()
{
    SYSTEM_WORK_MODE_SWITCH = true;
}

/*--------------------------- ���ϵͳ�л�״̬ -----------------------*/
void work_switch_clear()
{
    SYSTEM_WORK_MODE_SWITCH = false;
}

/*--------------------------- �ض�������ֹ -----------------------------*/
void read_back_disable()
{
     NRF_UICR->RBPCONF = UICR_RBPCONF_PALL_Disabled << UICR_RBPCONF_PALL_Pos;
}

/*--------------------------- ϵͳ����ϵͳ��� ------------------------*/
void mcu_bootloader_check()
{    
    if(bootloader_start_address == BOOTLOADER_ADDRESS_DEFAULT)
    {                                                
        read_back_disable();
        flash_word_write((uint32_t*)NRF_UICR_BOOT_START_ADDRESS, BOOTLOADER_ADDRESS);                               
        flash_specword_write((uint32_t*)SYSTEM_PARAMETERS_SETTINGS_ADDRESS, SYSTEM_APP_VALID_OFFSET, SYSTEM_APP_VALID);
        
        xprintf("switch bootloader to 0x28000, begin to reset.\r\n");
        NVIC_SystemReset();
    }
    
    xprintf("system app start work...\r\n");
}


/************************************************************************************/
/*--------------------------- �Ʋ����ݵĴ��� ---------------------------------------*/
/************************************************************************************/

/*--------------------------- �Ʋ����ݿ��Ƴ�ʼ�� ------------------------------*/
void pedo_data_ctrl_init()
{
    pedo_ctrl.sto_pointer = 0;   // ָ��Ԥ�洢��λ��  = ʵ�ʵ��ֽ���
    pedo_ctrl.slice_handle = false;
    
    pedo_ctrl.diff_count = 0;
    memset(pedo_ctrl.diff_pointer, 0, 20);
    
    memset(pedo_data, 0, 2048);
}

/*--------------------------- ����һ���˶��洢Ƭ�ε�״̬ ------------------------*/
bool get_pedo_slice_handle_state()
{
    return pedo_ctrl.slice_handle;
}

/*--------------------------- �˶����ݵĳ��� ------------------------------------*/
uint32_t get_pedo_data_length()
{
    return pedo_ctrl.sto_pointer;
}


/*--------------------------- �˶����ݲ��� --------------------------------------*/
void pedo_data_sto_ctrl_erase()
{
    pedo_data_ctrl_init();
}

/*--------------------------- �˶�����Ƭ�δ洢���� -------------------------------*/
void pedo_data_sto_handle(uint16_t pedo_time, uint16_t pedo_peak, uint16_t pedo_valley)
{   
    uint32_t the_value;
    uint8_t  i;
    if(pedo_ctrl.slice_handle == false)
    {   // �µ��˶�Ƭ�ο�ʼ

        pedo_ctrl.slice_handle = true;

        if(standard_time_judge() == true)
        {
            //Ϊ��׼ʱ��
            pedo_data[pedo_ctrl.sto_pointer++] = SYSTEM_STANDARD_TIME;
        }
        else
        {   
            pedo_data[pedo_ctrl.sto_pointer++] = SYSTEM_RELATIVE_TIME;
            
            // Ԥ�����ʱ���Ĵ洢�ռ�
            pedo_ctrl.diff_pointer[pedo_ctrl.diff_count++] = pedo_ctrl.sto_pointer;
            pedo_ctrl.sto_pointer += 4;
        }
        
        // �˶���ʼʱ��
        pedo_data[pedo_ctrl.sto_pointer++] = SYSTEM_SLICE_TIME_START;
        the_value = get_system_time();
        for(i = 0; i < 4; i++)
        {
            pedo_data[pedo_ctrl.sto_pointer++] = (the_value >> i*8) & 0xff;
        }
        
    }
    
    // �˶����洢
    pedo_data[pedo_ctrl.sto_pointer++] = SYSTEM_PEDO_START;
    the_value = pedo_time;
    for(i = 0; i < 2; i++)
    {
        pedo_data[pedo_ctrl.sto_pointer++] = (the_value >> i*8) & 0xff;
    }
    the_value = pedo_peak;
    for(i = 0; i < 2; i++)
    {
        pedo_data[pedo_ctrl.sto_pointer++] = (the_value >> i*8) & 0xff;
    }
    the_value = pedo_valley;
    for(i = 0; i < 2; i++)
    {
        pedo_data[pedo_ctrl.sto_pointer++] = (the_value >> i*8) & 0xff;
    }
   
}

/*--------------------------- �˶�����Ƭ�ν������� ----------------------------*/
void pedo_data_end_handle()
{
    uint32_t the_value;
    uint8_t  i;
    
    pedo_data[pedo_ctrl.sto_pointer++] = SYSTEM_SLICE_TIME_STOP;
    the_value = get_system_time();
    for(i = 0; i < 4; i++)
    {
        pedo_data[pedo_ctrl.sto_pointer++] = (the_value >> i*8) & 0xff;
    }    
    
    pedo_ctrl.slice_handle = false;     // ʱ��Ƭ�������
}

/*--------------------------- �˶�ʱ�����У׼ ---------------------------------------*/
void pedo_time_diff_sto_handle()
{
    uint32_t time_diff;
    uint8_t i;
    // ����ʱ�������ʱ����˶�ʱ��Ƭ��
    if(pedo_ctrl.diff_count > 0)
    {
        time_diff = SYSTEM_LOCAL_TIMING - SYSTEM_NO_SYNC_TIMING;
        
        for(i = 0; i < pedo_ctrl.diff_count; i++)
        {
            pedo_data[pedo_ctrl.diff_pointer[i] + 0] = (time_diff >> 0 ) & 0xff;
            pedo_data[pedo_ctrl.diff_pointer[i] + 1] = (time_diff >> 8 ) & 0xff;
            pedo_data[pedo_ctrl.diff_pointer[i] + 2] = (time_diff >> 16) & 0xff;
            pedo_data[pedo_ctrl.diff_pointer[i] + 3] = (time_diff >> 24) & 0xff;
        }
    }
}

/*--------------------------- ��ȡ�˶����� --------------------------------------*/
/*----- dataΪ���ʹ洢�ռ䣬offsetΪ��Ҫ��ȡ�ĵ�ַ��lengthΪ��ȡ�����ĳ��� ------*/
void get_pedo_data_handle(uint8_t *data, uint32_t *offset, uint8_t *length)
{
    uint8_t i;
 
    if( (get_pedo_data_length() - (*offset)) >= 19)
    {
        for(i = 0; i < 20; i++)
        {
            data[i] = pedo_data[*offset];
            *offset += 1;
        }

        *length = 20;                
    }
    else
    {
        *length =  get_pedo_data_length() - *offset;
        
        for(i = 0; *offset < get_pedo_data_length(); *offset += 1, i++ )
        {
            data[i] = pedo_data[*offset];            
        }
    } 

   
}
