#ifndef _SYSTEM_WORK_MODE_H_
#define _SYSTEM_WORK_MODE_H_

#include "stdbool.h"
#include "stdint.h"
#include "system_info_storage.h"
#include "twi_master.h"
#include "step_count.h"
#include "pwr_ctrl.h"

/*----------------------------------- 外部函数调用 ----------------------------*/
#define system_battery_self_test()    \
        battery_self_detect()

#define system_parameter_init()    \
        sys_parameters_init()
        

#define system_bootloader_check()    \
        mcu_bootloader_check()
        
#define system_I2C_init()                   \
        do                                  \
        {                                   \
            if(!twi_master_init())          \
                {	                        \
                    while(1);               \
                }                           \
        }                                   \
        while(0)
        
#define system_gpio_LoToHi_INT_init(pin)    \
        GPIO_LoToHi_INT_config(pin)      
                            
/*--------------------------------------------------------------------------*/










/*----------------------------- 本地函数申明 --------------------------------*/
void system_module_init(void);

void system_startup(void);

/*-------------------------- 系统工作模式初始化 ------------------------------*/ 
void system_default_work_mode_init(void); 

void system_user_sleep_mode_init(void);
            
void system_ble_work_mode_init(void);
           
void system_mcu_sleep_mode_init(void);
           
void system_power_shutdown_mode_init(void);



/*----------------------- 系统工作模式处理函数 -------------------------------*/
void system_default_work_mode_operate(void);  

void system_user_sleep_mode_operate(void);
            
void system_ble_work_mode_operate(void);
           
void system_mcu_sleep_mode_operate(void);
           
void system_power_shutdown_mode_operate(void);        

void system_idle_operate(void);

void system_led_flicker_init(void);

void system_state_detect(void);


#endif
