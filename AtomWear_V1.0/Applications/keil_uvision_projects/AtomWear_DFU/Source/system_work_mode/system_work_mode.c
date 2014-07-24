#include "system_work_mode.h"
#include "stdbool.h"
#include "stdint.h"
#include "boards.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "gpio_INT.h"
#include "ble_startup.h"
#include "app_timer.h"
#include "pwr_ctrl.h"
#include "ble_nus.h"
/**
 * ����洢����
 */
// ϵͳ����
volatile uint8_t system_battery_level    = 0;
volatile bool    system_charge_state     = false;

// ϵͳ����ʱ��
volatile uint32_t SYSTEM_LOCAL_TIMING = 0;
volatile uint32_t SYSTEM_NO_SYNC_TIMING = 0;

// �������ݴ洢��ʽ��
uint8_t pedo_data[2048];

typedef struct
{
    uint32_t sto_pointer;           // ָ����һ��Ҫ�洢������
    bool slice_handle;              // ��ʶ�Ƿ���ʱ��Ƭ�ڲ��� 
    
    uint32_t diff_pointer[20];     // ���� pedo_data����Ҫ�洢���ʱ�������λ��
    uint8_t  diff_count;            // ��Ҫ�洢���ʱ����������
} pedo_struct;
pedo_struct pedo_ctrl;

/*------------------------------------------------------------*/
volatile uint8_t SYSTEM_WORK_MODE  = 0x00;

volatile uint8_t CURRENT_WORK_MODE = 0x00;

volatile bool system_wait_high_g_INT = false;

volatile uint8_t system_local_click   = 0;

volatile uint32_t SYSTEM_SLEEP_TIMING = 0;

/**
 * true:  relative time
 * false: absolute time
 * offset from year:2000/0/0/0:0
 */
bool system_standard_time_judge()
{
    if(SYSTEM_LOCAL_TIMING > 0xA39376E1598000)  // 2000/0/0/0
        return true;

    return false;
}

uint32_t get_no_sync_time()
{
    return SYSTEM_NO_SYNC_TIMING;
}

void sto_no_sync_time()
{
    SYSTEM_NO_SYNC_TIMING = SYSTEM_LOCAL_TIMING;
}

uint32_t get_system_time()
{
    return SYSTEM_LOCAL_TIMING;
}


void pedo_data_ctrl_init()
{
    pedo_ctrl.sto_pointer = 0;   // ָ��Ԥ�洢��λ��  = ʵ�ʵ��ֽ���
    pedo_ctrl.slice_handle = false;
    
    pedo_ctrl.diff_count = 0;
    memset(pedo_ctrl.diff_pointer, 0, 20);
    
    memset(pedo_data, 0, 2048);
}

bool get_pedo_slice_handle_state()
{
    return pedo_ctrl.slice_handle;
}

uint32_t get_pedo_data_length()
{
    return pedo_ctrl.sto_pointer;
}

void pedo_data_sto_handle(uint16_t pedo_time, uint16_t pedo_peak, uint16_t pedo_valley)
{   
    uint32_t the_value;
    uint8_t  i;
    if(pedo_ctrl.slice_handle == false)
    {   // �µ��˶�Ƭ�ο�ʼ

        pedo_ctrl.slice_handle = true;

        if(system_standard_time_judge() == true)
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

// dataΪ���ʹ洢�ռ䣬offsetΪ��Ҫ��ȡ�ĵ�ַ��lengthΪ��ȡ�����ĳ���
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

void pedo_data_sto_ctrl_erase()
{
    pedo_data_ctrl_init();
}


void system_parameter_init()
{
    CURRENT_WORK_MODE = SYSTEM_WORK_MODE = SYSTEM_IDLE_MODE;
    pedo_data_ctrl_init();
}


void system_run(void)
{
    TIMER1_OPEN();
    //RTC1_OPEN();
}


void system_user_sleep_mode_init(void)
{
    xprintf("\r\nenter the user sleep mode...\r\n");
}
void system_user_sleep_mode_operate(void)
{}



void system_ble_work_mode_init(void)
{
    ble_prepare();
    ble_start();
}
void system_ble_work_mode_operate(void)
{
    if(BL.E.TIME_OUT_STATE == true || BL.E.END_TRANS_STATE == true)  // ��ʱ�Ĵ���
    {
        ble_close();
    }
    else
    {
        ble_rec_handle();
    }

    // ble_data_operate;
}


void system_mcu_sleep_mode_init(void)
{
}

void system_mcu_sleep_mode_operate(void)
{
    TIMER1_INT_CLOSE();
    RTC1_OPEN();
    
    system_high_g_dect_open();
    system_wait_high_g_INT = true;
    
    system_on_sleep();
    
    xprintf("system wake up........\r\n");
    system_high_g_dect_close();
    nrf_delay_ms(10);
    system_wait_high_g_INT = false;
    
    RTC_COUNTER_STOP();
    TIMER1_INT_OPEN();
}

void system_power_shutdown_mode_init(void)
{
}
void system_power_shutdown_mode_operate(void)
{}


void system_state_detect()
{
    // 5min sleep 5*60 = 300 *1s
    if( (SYSTEM_SLEEP_TIMING >= 30) && (CURRENT_WORK_MODE == SYSTEM_DEFAULT_MODE))
    {
        SYSTEM_SLEEP_TIMING = 0;
        system_mcu_sleep_mode_operate();
    }
}

void system_default_operate(void)
{
    system_state_detect();
}



void system_led_flicker_init()
{
    // gpio and timer init
    // timer init
    nrf_gpio_cfg_output(RED_LED);
    nrf_gpio_cfg_output(GREEN_LED);
    nrf_gpio_pin_clear(RED_LED);
    nrf_gpio_pin_clear(GREEN_LED);

}



void system_HFCLK_init()
{
    // Start 16 MHz crystal oscillator.
    NRF_CLOCK->EVENTS_HFCLKSTARTED  = 0;    // HFCLK oscillator state = 0.
    NRF_CLOCK->TASKS_HFCLKSTART     = 1;    // Start HFCLK clock source

    // Wait for the external oscillator to start up.
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
}

void system_low_battery()
{
    uint8_t i = 10;
    // Configure LED-pins as outputs
    nrf_gpio_cfg_output(RED_LED);

    // LED 0 and LED 1 blink alternately.
    while(i--)
    {
        nrf_gpio_pin_set(RED_LED);
        nrf_delay_ms(300);
        nrf_gpio_pin_clear(RED_LED);
        nrf_delay_ms(300);
    }
    // shutdown...
    xprintf("Low battery...\r\n");
    NRF_POWER->SYSTEMOFF = 0x01;
}


void system_module_init()
{
    simple_uart_config(RTS_PIN_NUMBER, TX_PIN_NUMBER, CTS_PIN_NUMBER, RX_PIN_NUMBER, 0);

    system_HFCLK_init();
    xprintf("\n\n\n\n\r\nHKCLK init OK...\r\n");
    // get the battery value
    if(!system_battery_self_test())
    {
        system_low_battery();
    }

    system_led_flicker_init();

    nrf_gpio_pin_set(RED_LED);
    xprintf("RED_LED is %d....\r\n", nrf_gpio_pin_read(RED_LED));

    system_parameter_init();

    system_gsensor_init();

    nrf_delay_ms(2000);
    nrf_gpio_pin_clear(RED_LED);
    xprintf("RED_LED is %d...\r\n", nrf_gpio_pin_read(RED_LED));

}

