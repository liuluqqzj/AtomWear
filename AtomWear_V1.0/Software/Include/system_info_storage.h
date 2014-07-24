#ifndef _SYSTEM_INFO_STORAGE_H_
#define _SYSTEM_INFO_STORAGE_H_
#include "stdbool.h"

/*--------------------- system work mode -----------------------------*/
#define SYSTEM_DEFAULT_MODE                0x01
#define SYSTEM_PEDOMETER_MODE              0x01
#define SYSTEM_USER_SLEEP_MODE             0x02
#define SYSTEM_BLE_WORK_MODE               0x03
#define SYSTEM_MCU_SLEEP_MODE              0x04
#define SYSTEM_POWER_SHUTDOWN_MODE         0x05
            
/*-------------- system data collect and synchronous -----------------*/
#define SYSTEM_STANDARD_TIME               0x01
#define SYSTEM_SET_ALARM                   0x02
#define SYSTEM_BATTERY_SYNC                0x03
#define SYSTEM_PEDO_START                  0x04
#define SYSTEM_USER_SLEEP_SYNC             0x05
#define SYSTEM_SLICE_TIME_START            0x06
#define SYSTEM_SLICE_TIME_STOP             0x07
#define SYSTEM_RESPONSE                    0x08
#define SYSTEM_DATA_SYNC_END               0x09
#define SYSTEM_DATA_SYNC_AMOUNT            0x0A
#define SYSTEM_DATA_SYNC                   0x0B
#define SYSTEM_RELATIVE_TIME               0x0C
#define SYSTEM_APP_UPDATE                  0x0D
#define SYSTEM_APP_VERSION                 0x11

/*----------------- ��ַ/����/ϵͳ״̬���� ----------------------------*/
#define CODE_PAGE_SIZE                     1024
#define BOOTLOADER_ADDRESS                 0x00028000
#define BOOTLOADER_ADDRESS_DEFAULT         0xFFFFFFFF
#define SYSTEM_PARAMETERS_SETTINGS_ADDRESS 0x0003FC00
#define NRF_UICR_BOOT_START_ADDRESS        (NRF_UICR_BASE + 0x14)

#define SYSTEM_APP_VALID_OFFSET            0
#define SYSTEM_APP_VALID                   0x00000001
#define SYSTEM_APP_INVALID                 0xFFFFFFFF

#define SYSTEM_SLEEP_DEADLINE              300 

/*------------------------- ϵͳ������ʼ�� ----------------------------*/
void sys_parameters_init(void);

/*------------------------- ϵͳ��׼ʱ���ж� -------------------------*/
bool standard_time_judge(void);

/*-------------------------- ���طǱ�׼ʱ�� ---------------------------*/
uint32_t get_no_sync_time(void);

/*-------------------------- �洢�Ǳ�׼ʱ�� ---------------------------*/
void sto_no_sync_time(void);

/*-------------------------- ���ر�׼ʱ�� ---------------------------*/
uint32_t get_system_time(void);

/*-------------------------- ˢ�±�׼ʱ�� ---------------------------*/
void system_time_update(void);

/*-------------------------- ͬ��������׼ʱ�� ---------------------------*/
void set_system_time(uint32_t std_time);

/*-------------------------- ����ϵͳ���ߵ���ʱ ---------------------------*/
uint32_t get_sleep_countdown(void);

/*-------------------------- ˢ��ϵͳ���ߵ���ʱ ---------------------------*/
void system_countdown_update(void);

/*-------------------------- ϵͳ���ߵ���ʱ���� ---------------------------*/
void clear_sleep_countdown(void);

/*-------------------------- ����ϵͳ�汾�� ---------------------------*/
uint16_t get_system_app_version(void);

/*--------------------------- ����ϵͳ������Ϣ -------------------------*/
void set_battery_level(uint8_t level);

/*--------------------------- ����ϵͳ������Ϣ -------------------------*/
uint8_t get_battery_level(void);

/*--------------------------- ����ϵͳ���״̬ -------------------------*/
void set_charging_level(bool state);

/*--------------------------- ����ϵͳ���״̬ -------------------------*/
bool get_charging_level(void);
   
/*--------------------------- ϵͳ����״̬���� -------------------------*/
void work_mode_switch(void);

/*--------------------------- ����ϵͳ����״̬ -------------------------*/
uint8_t get_work_mode(void);

/*--------------------------- ��ʼ��ϵͳ����״̬ -----------------------*/
void work_mode_init(void);  

/*--------------------------- ����ϵͳ�л�״̬ -------------------------*/
bool get_work_switch_state(void);

/*--------------------------- ��λϵͳ�л�״̬ -----------------------*/
void work_switch_set(void);

/*--------------------------- ���ϵͳ�л�״̬ -----------------------*/
void work_switch_clear(void);
   
/*--------------------------- ϵͳ����ϵͳ��� -----------------------*/
void mcu_bootloader_check(void); 
   
   
   
   
   
   
   
   
   
/*--------------------------- �Ʋ����ݿ��Ƴ�ʼ�� ------------------------------*/
void pedo_data_ctrl_init(void);

/*--------------------------- ����һ���˶��洢Ƭ�ε�״̬ ------------------------*/
bool get_pedo_slice_handle_state(void);

/*--------------------------- �˶����ݵĳ��� ------------------------------------*/
uint32_t get_pedo_data_length(void);

/*--------------------------- �˶����ݲ��� --------------------------------------*/
void pedo_data_sto_ctrl_erase(void);

/*--------------------------- �˶�����Ƭ�δ洢���� -------------------------------*/
void pedo_data_sto_handle(uint16_t pedo_time, uint16_t pedo_peak, uint16_t pedo_valley);

/*--------------------------- �˶�����Ƭ�ν������� ----------------------------*/
void pedo_data_end_handle(void);

/*--------------------------- �˶�ʱ�����У׼ ---------------------------------------*/
void pedo_time_diff_sto_handle(void);

/*--------------------------- ��ȡ�˶����� --------------------------------------*/
/*----- dataΪ���ʹ洢�ռ䣬offsetΪ��Ҫ��ȡ�ĵ�ַ��lengthΪ��ȡ�����ĳ��� ------*/
void get_pedo_data_handle(uint8_t *data, uint32_t *offset, uint8_t *length);

#endif
