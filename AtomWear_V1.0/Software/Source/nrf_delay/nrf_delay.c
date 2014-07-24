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
 
#include <stdio.h> 
#include "compiler_abstraction.h"
#include "nrf.h"
#include "nrf_delay.h"

/*lint --e{438} "Variable not used" */
void nrf_delay_ms(uint32_t volatile number_of_ms)
{
    while(number_of_ms != 0)
    {
        number_of_ms--;
        nrf_delay_us(999);
    }
}



void Delay_s(uint32_t t)
{
    nrf_delay_ms(t*1000);    
}


void Delay_Ms(uint32_t i)
{
    nrf_delay_ms(i*10);
}



