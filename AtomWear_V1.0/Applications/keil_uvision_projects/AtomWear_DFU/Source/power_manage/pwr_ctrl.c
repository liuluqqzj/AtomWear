#include "pwr_ctrl.h"
#include "nrf_soc.h"
#include "xprintf.h"
#include "simple_uart.h"
#include "nrf_delay.h"
#include "nrf51.h"

#define DCDC_MODE   0
#define LOW_MODE    1

//系统供电选择
void power_supply(uint8_t pwr_slt)
{
    //uint32_t retval;
    switch(pwr_slt)
    {
        case DCDC_MODE :
            if (sd_power_dcdc_mode_set(NRF_POWER_DCDC_MODE_ON) != NRF_SUCCESS )
            {
                xprintf("DCDC mode open failed.\r\n");
            }
            // Enabling this will give a softdevice event (NRF_EVENT_POWER_FAILURE_WARNING) when the power failure warning occurs.
            // The event can be retrived with sd_event_get();
            if( sd_power_pof_enable(1) != NRF_SUCCESS)
            {
                xprintf("power failed enable failed.\r\n");
            }
            if( sd_power_pof_threshold_set(NRF_POWER_THRESHOLD_V25) != NRF_SUCCESS)
            {
                xprintf("set threshold failed.\r\n");
            }
            break;
        
        case LOW_MODE :
            
            if (sd_power_dcdc_mode_set(NRF_POWER_DCDC_MODE_OFF) != NRF_SUCCESS )
            {
                xprintf("DCDC mode close failed.\r\n");
            }
            break;
           
                      
    }

}
//系统关闭模式设置
void system_off_set()
{               
    NRF_POWER->INTENSET = 0x01; //中断  RAMON 管理   RESET 引脚复位 
    NRF_POWER->RAMON = NRF_POWER->RAMON|0x000f0000;
    nrf_delay_ms(30);
    
    NRF_POWER->SYSTEMOFF = 0x01;
}

//系统打开，低功耗模式设置
void system_on_sleep()
{
    xprintf("system is going to sleep..z..z..Z...\r\n");
    NRF_POWER->TASKS_LOWPWR = 0x01;
    __WFI();
}

void power_set_test()
{
    xprintf("the system is running ...\r\n");
    nrf_delay_ms(5000);
    
    system_on_sleep();
    xprintf("I wake up ...\r\n");
    nrf_delay_ms(200);
}
