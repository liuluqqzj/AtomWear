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
#include <math.h>
#include "nrf_delay.h"
#include "xprintf.h"
#include "nrf_gpio.h"
#include "nrf_error.h"
#include "step_count.h"
#include "bma180.h"
#include "mma8452q.h"
#include "system_info_storage.h"

#define gensor_read() xxx

#define G_STAND               1010
#define THRESHOLD_UP          1212   // (uint32_t)(G_STAND*1.2)
#define THRESHOLD_DOWN        909    // (uint32_t)(G_STAND*0.9)
#define THRESHOLD_WIDE        707    // (0.7g)


static int g_sum;

/**-------------------------------------------------------------------------------------------
 * The data from MPU to computer should be like this:
 * Ch1Data_L,Ch1Data_H,
 * Ch2Data_L,Ch2Data_H,
 * Ch3Data_L,Ch3Data_H,
 * Ch4Data_L,Ch4Data_H,
 * CRC16_L,CRC16_H
 *Data send adapt CRC16 verification,The following is the function of CRC16,please refer*/
unsigned short CRC_CHECK(unsigned char *Buf, unsigned char CRC_CNT)
{
    unsigned short CRC_Temp;
    unsigned char i,j;
    CRC_Temp = 0xffff;

    for (i=0; i<CRC_CNT; i++)
    {
        CRC_Temp ^= Buf[i];
        for (j=0; j<8; j++)
        {
            if (CRC_Temp & 0x01)
                CRC_Temp = (CRC_Temp >>1 ) ^ 0xa001;
            else
                CRC_Temp = CRC_Temp >> 1;
        }
    }
    return(CRC_Temp);
}

 
int16_t byte_to_word(uint8_t LSB, uint8_t MSB)
{
     
    int16_t acce;
    acce = MSB;
    acce = acce<<6;	 
    acce += ( LSB>>2 );	 

    //00 10 0000 0000 0000b� 
    if( acce & 0x2000 )
    {
        acce = ~acce;
        acce &= ~0xc000;     
        acce++;
        g_sum += acce*acce;       
        acce = -acce;
        return acce;	   
    }
    else
    {
        g_sum += acce*acce;
        return acce;
    }
}




///////////////////////////////////////////////////////////////////////////////////////
// x,y,z 
uint16_t bma180_get_mo_value()
{
    uint8_t rec[6],i;
    uint16_t acce;
    uint32_t m_acce = 0;

    //gensor_read(rec);
    for(i=0; i<3; i++)
    {
        acce = rec[2*i+1];
        acce = acce<<6;	 
        acce += ( rec[2*i]>>2 );

        
        if( acce & 0x2000 )
        {
            acce = ~acce;
            acce &= ~0xc000;     
            acce++;
        }
        m_acce += acce*acce;
    }
    return (uint16_t)sqrt(m_acce);
}



/////////////////////////////////////////////////////////////////////////////////////////////////


struct g_node cur, pre;
struct s_node peak[5], valley[5];
uint8_t spe_num;

 
uint16_t que_value[5], Thr_Up, Thr_Down, Thr_Wide;
uint8_t  que_num, timing, deadline;
 
static uint32_t step_count;



void pedometer_init()
{
    peak[0].value = peak[1].value = peak[2].value = peak[3].value = peak[4].value = G_STAND;
    valley[0].value = valley[1].value = valley[2].value = valley[3].value = valley[4].value =G_STAND;
    peak[0].time = peak[1].time = peak[2].time = peak[3].time = peak[4].time = 0;
    valley[0].time = valley[1].time = valley[2].time = valley[3].time = valley[4].time = 0;

    que_value[0] = que_value[1] = que_value[2] = que_value[3] = que_value[4] = cur.value = pre.value = G_STAND;
    cur.time = pre.time = 0;

    spe_num = que_num = 0;

    Thr_Up = THRESHOLD_UP;       // 1.3g high
    Thr_Down = THRESHOLD_DOWN;     // 0.8g low
    Thr_Wide = THRESHOLD_WIDE;      // 0.8g wide

    deadline = timing = 0;  // 0.02s 
    step_count = 0;
    
//////////////////////////////////////////////////////
}




void timer1_startup()
{
    NRF_TIMER1->TASKS_START    = 1;
}

void timer1_stop()
{
    NRF_TIMER1->TASKS_STOP    = 1;
    NRF_TIMER1->INTENSET       = TIMER_INTENSET_COMPARE0_Disabled << TIMER_INTENSET_COMPARE0_Pos;   // ��ͨ��ʱ�ж�1
    //NRF_TIMER1->INTENSET       = TIMER_INTENSET_COMPARE1_Disabled << TIMER_INTENSET_COMPARE0_Pos;   // ��ͨ��ʱ�ж�1
}



//void system_default_work_mode_init()
//{
 //   pedometer_init();
//    CURRENT_WORK_MODE = SYSTEM_WORK_MODE = SYSTEM_PEDOMETER_MODE;    
//}




void pedometer_startup()
{
    if(deadline >= 200)
    {
        xprintf(" INT  2s.\r\n");
       /*--------------------------------------------------------------*/
        system_countdown_update();
        if( (get_sleep_countdown() > 120) && (get_pedo_slice_handle_state() == true ) )   // 2min = 2*60s = 120 *1s
        {
            pedo_data_end_handle();
        }        
        /*--------------------------------------------------------------*/
        
        Thr_Up = THRESHOLD_UP;          // 1.2g high
        Thr_Down = THRESHOLD_DOWN;      // 0.8g low
        Thr_Wide = THRESHOLD_WIDE;      // 0.8g wide

        peak[0].value = peak[1].value = peak[2].value = peak[3].value = peak[4].value = G_STAND;
        valley[0].value = valley[1].value = valley[2].value = valley[3].value = valley[4].value =G_STAND;
        peak[0].time = peak[1].time = peak[2].time = peak[3].time = peak[4].time = 0;
        valley[0].time = valley[1].time = valley[2].time = valley[3].time = valley[4].time = 0;
        spe_num = 0;

        que_value[0] = que_value[1] = que_value[2] = que_value[3] = que_value[4]  = cur.value = pre.value = G_STAND;
        cur.time = pre.time = 0;
        que_num = 0;

        deadline = timing = 0;  //  0.02s 

    }


    else
    {
        timing ++;
        deadline = cur.time =  timing;
        if(++que_num ==5)   // que_num = (++que_num)%5
            que_num = 0;
        que_value[que_num] = mma8452q_get_mo_value();
        cur.value = (que_value[0]+que_value[1]+que_value[2]+que_value[3]+que_value[4])/5;

        if(cur.value >= pre.value)
        {
            

            if(cur.value >= peak[spe_num].value)
            {
                // > peak

                peak[spe_num].value =valley[spe_num].value = cur.value;
                peak[spe_num].time = valley[spe_num].time = cur.time;

                pre.value = cur.value;
                pre.time = cur.time;

            }
            else  if( (cur.value < Thr_Down) || ( (peak[spe_num].value - valley[spe_num].value) > Thr_Wide) )    // ���ڵ���ֵ�𣿻��߷�������
            {

                if(valley[spe_num].time < 20)    // 0.2s 
                {
                    // peak[spe_num].value = valley[spe_num].value = G_STAND;
                    //peak[spe_num].time = valley[spe_num].time = 0;

                    pre.value = cur.value;
                    pre.time = cur.time;
                }
                else
                {
                    if( peak[spe_num].value > Thr_Up )
                    {
                        // step count sucessful
                        /*--------------------------------------------------------------*/
                        pedo_data_sto_handle(valley[spe_num].time, peak[spe_num].value, valley[spe_num].value);
                        clear_sleep_countdown();                       
                        /*--------------------------------------------------------------*/
                        
                        step_count++;                        
                        xprintf("steps = %d. time =%d  peak = %d valley = %d \r\n", step_count, valley[spe_num].time, peak[spe_num].value, valley[spe_num].value );
                        
                        timing = deadline = 0;
                        que_num = 0;
                        
                        if(spe_num == 4)
                        {
                            spe_num = 0;
                            Thr_Up = ( ( peak[0].value + peak[1].value + peak[2].value + peak[3].value + peak[4].value)/5 + THRESHOLD_UP)/2;
                            Thr_Down = ( ( valley[0].value + valley[1].value + valley[2].value + valley[3].value + valley[4].value)/5 + THRESHOLD_DOWN)/2;
                            Thr_Wide = ( ( (peak[0].value - valley[0].value) + (peak[1].value - valley[1].value) + (peak[2].value - valley[2].value) + (peak[3].value - valley[3].value) + (peak[4].value - valley[4].value) )/5 + THRESHOLD_WIDE)/2;


                            peak[0].value = peak[1].value = peak[2].value = peak[3].value = peak[4].value = G_STAND;
                            valley[0].value = valley[1].value = valley[2].value = valley[3].value = valley[4].value =G_STAND;
                            peak[0].time = peak[1].time = peak[2].time = peak[3].time = peak[4].time = 0;
                            valley[0].time = valley[1].time = valley[2].time = valley[3].time = valley[4].time = 0;

                            que_value[0] = que_value[1] = que_value[2] = que_value[3] = que_value[4]  = cur.value = pre.value = G_STAND;
                            cur.time = pre.time = 0;

                        }
                        else
                        {
                            spe_num++;
                            peak[spe_num].value = valley[spe_num].value = THRESHOLD_UP;
                            peak[spe_num].time = valley[spe_num].value = THRESHOLD_DOWN;

                            que_value[0] = que_value[1] = que_value[2] = que_value[3] = que_value[4]  = cur.value = pre.value = G_STAND;
                            cur.time = pre.time = 0;

                        }
                    }
                    else //  
                    {
                        pre.value = cur.value;
                        pre.time = cur.time;
                    }
                } // 

            }
            else    //  
            {
                pre.value = cur.value;
                pre.time = cur.time;
            }

        } //  


        else   // 
        {
            if(cur.value >= valley[spe_num].value)
            {

                pre.value = cur.value;
                pre.time = cur.time;

            }
            else // 
            {
                valley[spe_num].value = cur.value;
                valley[spe_num].time = cur.time;

                pre.value = cur.value;
                pre.time = cur.time;
            }

        } //  

    } //  
}

