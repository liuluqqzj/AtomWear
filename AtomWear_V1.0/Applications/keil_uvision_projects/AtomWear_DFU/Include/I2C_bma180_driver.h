#ifndef I2C_BMA180_DRIVER_H
#define I2C_BMA180_DRIVER_H

#include <stdbool.h>
#include <stdint.h>

#define DEBUG_MODE		     1
#define TRANS_STOP		     1
#define TRANS_CONTINUE		 0
#define GPIO_INT_PIN       12

#define bma180_addr_write   0x80
#define bma180_addr_read    0x81
 
#include "twi_master.h"
#include "nrf_delay.h"
#include "I2C_bma180_driver.h"
#include "simple_uart.h"
#include "xprintf.h"


//IIC初始化
void IIC_Init(void );
 
//bma180工作模式配置
void bma180_basic_config(void );
 
//读取bma180的三轴加速度值
void bma180_read(uint8_t acce[6]);

/*											 typ
g-range:       +/-1.0 g  8192  
S1.5g g-range: +/-1.5 g  5460  
S2g g-range:   +/-2.0 g  4096  
*/
float byte_to_float(uint8_t LSB, uint8_t MSB, int typ);


/**
 *将一个32位有符号浮点数转换成8位无符号字符数组存储
 *返回字符数组的长度,arrary,至少要6位以上。
 */
unsigned char Float_to_Char(float value, unsigned char* array);


void bma180_demo(void );

void gpio_INT_test(void );

void system_gsensor_init(void);

//void nRF_gpio_INT_config(void);

#endif
