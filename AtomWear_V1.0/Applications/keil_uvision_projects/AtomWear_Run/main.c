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
 * the file.`12*\dsa
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "simple_uart.h"
#include "xprintf.h"
#include "system_work_handle.h"
#include "ble_startup.h"
#include "oled.h"

#include "bmp180.h"
#include "twi_master.h"
#include "mpu6050.h"
#include "hmc5883l.h"
#include "mma8452q.h"

void module_test(void);

/*------------------------ This is Version1.0.  ------------------------*/

int main()
{      
    system_module_init();  
        
    // default work mode
    system_default_work_mode_init();
    
        
    system_startup();

    for( ; ;)   // depend on sys click
    {             
       system_state_detect();
    }
}
//end
