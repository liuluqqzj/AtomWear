#include <stdio.h>
#include <math.h>
#include "nrf_delay.h"
#include "simple_uart.h"
#include "xprintf.h"
#include "nrf_gpio.h"
#include "nrf_error.h"
#include "step_count.h"
#include "I2C_bma180_driver.h"
#include "system_work_mode.h"


#define G_STAND               2065
#define THRESHOLD_UP          2478   // (uint32_t)(G_STAND*1.2)
#define THRESHOLD_DOWN        1858   // (uint32_t)(G_STAND*0.9)
#define THRESHOLD_WIDE        1445      // (0.7g)


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

// ˫��ת����16λ
int16_t byte_to_word(uint8_t LSB, uint8_t MSB)
{
    //����LSB,MSB�ֱ�ֻ��8λ�����ܶ���������ƣ��������ݷ�������
    int16_t acce;
    acce = MSB;
    acce = acce<<6;	//����λ�ȴ洢��Ȼ������6λ��ʣ�µ�6λ��LSB
    acce += ( LSB>>2 );	//��LSB������2λ��ȥ����Ч��2λ���ٴ���data�С�

    //00 10 0000 0000 0000b�����λ0��1��
    if( acce & 0x2000 )
    {
        acce = ~acce;
        acce &= ~0xc000;    //���2λû�õ���ȡ��+1����Դ��
        acce++;
        g_sum += acce*acce;      //�洢������
        acce = -acce;
        return acce;	  //������ٶ�
    }
    else
    {
        g_sum += acce*acce;
        return acce;
    }
}

//////////////////////////////////////////////////////////////////////////////////

// ��ӡ�������ݣ�  MSB���λΪ���ţ�LSB��2λ��Ч
void output_xyz_serial_send(uint8_t acce[6])
{
    uint8_t TxBuf[10],i;
    int16_t w_acce;
    unsigned short CRC;

    g_sum = 0;
    w_acce = byte_to_word(acce[0], acce[1]);//xprintf("%d  ",w_acce);
    TxBuf[0] = (uint8_t)(w_acce &0xff);
    TxBuf[1] = (uint8_t)(w_acce >> 8);

    w_acce = byte_to_word(acce[2], acce[3]);//xprintf("%d  ",w_acce);
    TxBuf[2] = (uint8_t)(w_acce &0xff);
    TxBuf[3] = (uint8_t)(w_acce >> 8);

    w_acce = byte_to_word(acce[4], acce[5]);//xprintf("%d  \n",w_acce);
    TxBuf[4] = (uint8_t)(w_acce &0xff);
    TxBuf[5] = (uint8_t)(w_acce >> 8);

    w_acce = (int16_t)sqrt(g_sum);//xprintf("%d  \n",w_acce);
    TxBuf[6] = (uint8_t)(w_acce &0xff);
    TxBuf[7] = (uint8_t)(w_acce >> 8);

    CRC = CRC_CHECK(TxBuf, 8);
    TxBuf[8] = (uint8_t)(CRC & 0xFF);
    TxBuf[9] = (uint8_t)(CRC >> 8);

    for(i=0; i<10; i++)
        simple_uart_put(TxBuf[i]);
}


// �����������ݲ���
void serial_digital_test()
{
    uint8_t acce[6];
    IIC_Init();
    bma180_basic_config();

    while(1)
    {
        bma180_read(acce);
        output_xyz_serial_send(acce);
        nrf_delay_ms(10);
    }
}

///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
// x,y,z������ģ
uint16_t mo_value()
{
    uint8_t rec[6],i;
    uint16_t acce;
    uint32_t m_acce = 0;

    bma180_read(rec);
    for(i=0; i<3; i++)
    {
        acce = rec[2*i+1];
        acce = acce<<6;	//��λ�ȴ棬Ȼ������6λ��ʣ�µ�6λ��LSB
        acce += ( rec[2*i]>>2 );

        //��Ϊ��
        if( acce & 0x2000 )
        {
            acce = ~acce;
            acce &= ~0xc000;    //���2λû�ã���0
            acce++;
        }
        m_acce += acce*acce;
    }
    return (uint16_t)sqrt(m_acce);
}

//  �������
void output_modular_test()
{
    uint8_t TxBuf[10] = {0},i;
    uint16_t m_value,CRC;

    IIC_Init();
    bma180_basic_config();

    while(1)
    {
        m_value = mo_value();
        xprintf("m_value = %d\n", m_value);

        if(0)
        {
            TxBuf[0] = (uint8_t)(m_value & 0xFF);
            TxBuf[1] = (uint8_t)(m_value >> 8);


            CRC = CRC_CHECK(TxBuf, 8);
            TxBuf[8] = (uint8_t)(CRC & 0xFF);
            TxBuf[9] = (uint8_t)(CRC >> 8);

            for(i=0; i<10; i++)
                simple_uart_put(TxBuf[i]);
        }

        nrf_delay_ms(20);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////


struct g_node cur, pre;
struct s_node peak[5], valley[5];
uint8_t spe_num;

// �����������
uint16_t que_value[5], Thr_Up, Thr_Down, Thr_Wide;
uint8_t  que_num, timing, deadline;
//�Ʋ�
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

    deadline = timing = 0;  // ��0.02sΪ��λ����
    step_count = 0;
    
//////////////////////////////////////////////////////
}


// /**
//  *ʹ�ö�ʱ��1�ж�
//  */
// void TIMER1_IRQHandler()
// {
//     if((NRF_TIMER1->EVENTS_COMPARE[0] == 1) && (NRF_TIMER1->INTENSET & TIMER_INTENSET_COMPARE0_Msk))
//     {
//         NRF_TIMER1->EVENTS_COMPARE[0] = 0;
//         // p_timer->INTENSET = TIMER_INTENSET_COMPARE0_Disabled << TIMER_INTENSET_COMPARE0_Pos; // �ض�ʱ����ʱ�ж�0
//         NRF_TIMER1->TASKS_CLEAR = 1;

//         if(deadline >= 100)
//         {
//             xprintf(" INT  2s.\r\n");

//             Thr_Up = THRESHOLD_UP;      // 1.2g high
//             Thr_Down = THRESHOLD_DOWN;    // 0.8g low
//             Thr_Wide = THRESHOLD_WIDE;     // 0.8g wide

//             peak[0].value = peak[1].value = peak[2].value = peak[3].value = peak[4].value = G_STAND;
//             valley[0].value = valley[1].value = valley[2].value = valley[3].value = valley[4].value =G_STAND;
//             peak[0].time = peak[1].time = peak[2].time = peak[3].time = peak[4].time = 0;
//             valley[0].time = valley[1].time = valley[2].time = valley[3].time = valley[4].time = 0;
//             spe_num = 0;

//             que_value[0] = que_value[1] = que_value[2] = que_value[3] = que_value[4]  = cur.value = pre.value = G_STAND;
//             cur.time = pre.time = 0;
//             que_num = 0;

//             deadline = timing = 0;  // ��0.02sΪ��λ����

//         }


//         else
//         {
//             timing ++;
//             deadline = cur.time =  timing;
//             if(++que_num ==5)   // que_num = (++que_num)%5
//                 que_num = 0;
//             que_value[que_num] = mo_value();
//             cur.value = (que_value[0]+que_value[1]+que_value[2]+que_value[3]+que_value[4])/5;

//             if(cur.value >= pre.value)
//             {
//                 // ������

//                 if(cur.value >= peak[spe_num].value)
//                 {
//                     // ����peak

//                     peak[spe_num].value =valley[spe_num].value = cur.value;
//                     peak[spe_num].time = valley[spe_num].time = cur.time;

//                     pre.value = cur.value;
//                     pre.time = cur.time;

//                 }
//                 else  if( (cur.value < Thr_Down) || ( (peak[spe_num].value - valley[spe_num].value) > Thr_Wide) )    // ���ڵ���ֵ�𣿻��߷�������
//                 {

//                     if(valley[spe_num].time < 10)    // 0.2s���ʱ���ж�
//                     {
//                         // peak[spe_num].value = valley[spe_num].value = G_STAND;
//                         //peak[spe_num].time = valley[spe_num].time = 0;

//                         pre.value = cur.value;
//                         pre.time = cur.time;
//                     }
//                     else
//                     {
//                         if( peak[spe_num].value > Thr_Up )
//                         {
//                             // һ�γɹ��Ʋ�
//                             step_count++;
//                             xprintf("steps = %d. peak = %d valley = %d ,time =%d \r\n", step_count, peak[spe_num].value, valley[spe_num].value, timing);
//                             timing = deadline = 0;
//                             que_num = 0;

//                             if(spe_num == 4)
//                             {
//                                 spe_num = 0;
//                                 Thr_Up = ( ( peak[0].value + peak[1].value + peak[2].value + peak[3].value + peak[4].value)/5 + THRESHOLD_UP)/2;
//                                 Thr_Down = ( ( valley[0].value + valley[1].value + valley[2].value + valley[3].value + valley[4].value)/5 + THRESHOLD_DOWN)/2;
//                                 Thr_Wide = ( ( (peak[0].value - valley[0].value) + (peak[1].value - valley[1].value) + (peak[2].value - valley[2].value) + (peak[3].value - valley[3].value) + (peak[4].value - valley[4].value) )/5 + THRESHOLD_WIDE)/2;


//                                 peak[0].value = peak[1].value = peak[2].value = peak[3].value = peak[4].value = G_STAND;
//                                 valley[0].value = valley[1].value = valley[2].value = valley[3].value = valley[4].value =G_STAND;
//                                 peak[0].time = peak[1].time = peak[2].time = peak[3].time = peak[4].time = 0;
//                                 valley[0].time = valley[1].time = valley[2].time = valley[3].time = valley[4].time = 0;

//                                 que_value[0] = que_value[1] = que_value[2] = que_value[3] = que_value[4]  = cur.value = pre.value = G_STAND;
//                                 cur.time = pre.time = 0;

//                             }
//                             else
//                             {
//                                 spe_num++;
//                                 peak[spe_num].value = valley[spe_num].value = THRESHOLD_UP;
//                                 peak[spe_num].time = valley[spe_num].value = THRESHOLD_DOWN;

//                                 que_value[0] = que_value[1] = que_value[2] = que_value[3] = que_value[4]  = cur.value = pre.value = G_STAND;
//                                 cur.time = pre.time = 0;

//                             }
//                         }
//                         else // ��ֵ������
//                         {
//                             pre.value = cur.value;
//                             pre.time = cur.time;
//                         }
//                     } //�Ʋ���ֵ�ж�

//                 }
//                 else    // ���ڵ���ֵ ������
//                 {
//                     pre.value = cur.value;
//                     pre.time = cur.time;
//                 }

//             } // ������


//             else   //�½���  ������
//             {
//                 if(cur.value >= valley[spe_num].value)
//                 {

//                     pre.value = cur.value;
//                     pre.time = cur.time;

//                 }
//                 else //С�ڹ�ֵ
//                 {
//                     valley[spe_num].value = cur.value;
//                     valley[spe_num].time = cur.time;

//                     pre.value = cur.value;
//                     pre.time = cur.time;
//                 }

//             } // �������ж�

//         } // 20ms�жϴ���
//     }

// }


// void timer1_config()
// {
//     // Start 16 MHz crystal oscillator.
//     NRF_CLOCK->EVENTS_HFCLKSTARTED  = 0;    // HFCLK oscillator state = 0.
//     NRF_CLOCK->TASKS_HFCLKSTART     = 1;    // Start HFCLK clock source

//     // Wait for the external oscillator to start up.
//     while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0)
//     {
//         // Do nothing.
//     }
//     NRF_TIMER1->MODE           = TIMER_MODE_MODE_Timer;        // Set the timer in Timer Mode. 1Mhz = 1000000 Hz
//     NRF_TIMER1->PRESCALER      = 4;                            // Prescaler 4 produces 1MHz timer frequency => 1 tick = 1 us.
//     NRF_TIMER1->BITMODE        = TIMER_BITMODE_BITMODE_32Bit;  // 32 bit mode.
//     /* Bit 16 : Enable interrupt on COMPARE[0] */
//     NVIC_EnableIRQ(TIMER1_IRQn);

//     NRF_TIMER1->TASKS_CLEAR    = 1;                            // clear the task first to be usable for later.

//     //
//     NRF_TIMER1->CC[0]          = 20000;    // 50Hz ���ã�ÿ1/50s��0.02s����20000us�ж�һ��
//     NRF_TIMER1->CC[1]          = 2000000;    // 50Hz ���ã�2s����2000000us��ʱ

//     NRF_TIMER1->INTENSET       = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos;   // ��ͨ��ʱ�ж�1
//     //NRF_TIMER1->INTENSET       = TIMER_INTENSET_COMPARE1_Enabled << TIMER_INTENSET_COMPARE1_Pos;   // ��ͨ��ʱ�ж�1
//     // p_timer->TASKS_START    = 1;                    // Start timer.
// }
// 

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



void system_default_work_mode_init()
{
    pedometer_init();
    CURRENT_WORK_MODE = SYSTEM_WORK_MODE = SYSTEM_PEDOMETER_MODE;    
}




void system_default_work_mode_operate()
{
    if(deadline >= 200)
    {
        xprintf(" INT  2s.\r\n");
        SYSTEM_SLEEP_TIMING += 2;
        if( (SYSTEM_SLEEP_TIMING > 120) && (get_pedo_slice_handle_state() == true ) )   // 2min = 2*60s = 120 *1s
        {
            pedo_data_end_handle();
        }        

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

        deadline = timing = 0;  // ��0.02sΪ��λ����

    }


    else
    {
        timing ++;
        deadline = cur.time =  timing;
        if(++que_num ==5)   // que_num = (++que_num)%5
            que_num = 0;
        que_value[que_num] = mo_value();
        cur.value = (que_value[0]+que_value[1]+que_value[2]+que_value[3]+que_value[4])/5;

        if(cur.value >= pre.value)
        {
            // ������

            if(cur.value >= peak[spe_num].value)
            {
                // ����peak

                peak[spe_num].value =valley[spe_num].value = cur.value;
                peak[spe_num].time = valley[spe_num].time = cur.time;

                pre.value = cur.value;
                pre.time = cur.time;

            }
            else  if( (cur.value < Thr_Down) || ( (peak[spe_num].value - valley[spe_num].value) > Thr_Wide) )    // ���ڵ���ֵ�𣿻��߷�������
            {

                if(valley[spe_num].time < 20)    // 0.2s���ʱ���ж�
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
                        // һ�γɹ��Ʋ�
                        pedo_data_sto_handle(valley[spe_num].time, peak[spe_num].value, valley[spe_num].value);
                        
                        step_count++;
                        SYSTEM_SLEEP_TIMING = 0;
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
                    else // ��ֵ������
                    {
                        pre.value = cur.value;
                        pre.time = cur.time;
                    }
                } //�Ʋ���ֵ�ж�

            }
            else    // ���ڵ���ֵ ������
            {
                pre.value = cur.value;
                pre.time = cur.time;
            }

        } // ������


        else   //�½���  ������
        {
            if(cur.value >= valley[spe_num].value)
            {

                pre.value = cur.value;
                pre.time = cur.time;

            }
            else //С�ڹ�ֵ
            {
                valley[spe_num].value = cur.value;
                valley[spe_num].time = cur.time;

                pre.value = cur.value;
                pre.time = cur.time;
            }

        } // �������ж�

    } // 20ms�жϴ���
}

