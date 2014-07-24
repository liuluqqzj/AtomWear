#include "button.h"
#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_gpio.h"
#include "xprintf.h"

#define nRF_gpio_INT_pin         6     // 引脚(8)计时中断检测

static uint8_t button_tmp = 0;
// 0: nothing happened, 1:single-click, 2:double-click, 3:long-click 
static uint8_t BUTTON_STATE = 0;

//NRF_TIMER_Type * p_timer; //NRF_TIMER0/1/2
#define p_timer             NRF_TIMER0

// 中断事件处理函数
void GPIOTE_IRQHandler()
{
	 // Event causing the interrupt must be cleared.
    if((NRF_GPIOTE->EVENTS_IN[0] == 1) && (NRF_GPIOTE->INTENSET & GPIOTE_INTENSET_IN0_Msk) && (NRF_GPIOTE->CONFIG[0] &  (GPIOTE_CONFIG_POLARITY_HiToLo << GPIOTE_CONFIG_POLARITY_Pos) ) ) //上升沿
    {
        NRF_GPIOTE->INTENSET  = GPIOTE_INTENSET_IN0_Disabled << GPIOTE_INTENSET_IN0_Pos;     // 关GPIO引脚中断
        NRF_GPIOTE->EVENTS_IN[0] = 0;
        NRF_GPIOTE->CONFIG[0] =  (GPIOTE_CONFIG_POLARITY_LoToHi << GPIOTE_CONFIG_POLARITY_Pos)
                           | (nRF_gpio_INT_pin << GPIOTE_CONFIG_PSEL_Pos)  
                           | (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos);
               
        p_timer->TASKS_CLEAR = 1;
        p_timer->INTENSET       = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos;   // 开通延时中断1
        p_timer->TASKS_START = 1;
        //xprintf("High INT.\r\n");
    }
    // Event causing the interrupt must be cleared.
    if((NRF_GPIOTE->EVENTS_IN[0] == 1) && (NRF_GPIOTE->INTENSET & GPIOTE_INTENSET_IN0_Msk) && (NRF_GPIOTE->CONFIG[0] &  (GPIOTE_CONFIG_POLARITY_LoToHi << GPIOTE_CONFIG_POLARITY_Pos) ) )  // 下降沿
        {
            //xprintf("Low INT.\r\n");
           // NRF_GPIOTE->INTENSET  = GPIOTE_INTENSET_IN0_Disabled << GPIOTE_INTENSET_IN0_Pos;     // 关GPIO引脚中断
            NRF_GPIOTE->EVENTS_IN[0] = 0;
            NRF_GPIOTE->CONFIG[0] =  (GPIOTE_CONFIG_POLARITY_HiToLo << GPIOTE_CONFIG_POLARITY_Pos)
                           | (nRF_gpio_INT_pin << GPIOTE_CONFIG_PSEL_Pos)  
                           | (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos);
            
            p_timer->TASKS_CAPTURE[1] = 1;  // 获取当前的定时器计时
            if( (p_timer->CC[1] >= 28846) && (button_tmp == 1) )   //  获得一次按键并且是长按
            {
                BUTTON_STATE = 3;
                button_tmp  = 0;
                p_timer->TASKS_STOP = 1;
                p_timer->TASKS_CLEAR = 1;
                p_timer->INTENSET = TIMER_INTENSET_COMPARE0_Disabled << TIMER_INTENSET_COMPARE0_Pos; // 关定时器延时中断0
                p_timer->INTENSET = TIMER_INTENSET_COMPARE3_Disabled << TIMER_INTENSET_COMPARE3_Pos; // 关延时中断
                xprintf("Long click.\r\n");
            }
            else
            {
                if(button_tmp == 1)    //  获得一次按键，但不是长按，开启1s 双击计时
                {
                   // p_timer->TASKS_CLEAR = 1;
                    //xprintf("1s jishi.\r\n");
                    p_timer->INTENSET      = TIMER_INTENSET_COMPARE3_Enabled << TIMER_INTENSET_COMPARE3_Pos;   // 开延时中断2
                }
            }
          //NRF_GPIOTE->INTENSET  = GPIOTE_INTENSET_IN0_Set << GPIOTE_INTENSET_IN0_Pos;     // 开GPIO引脚中断
        }
}
void TIMER0_IRQHandler()
{
    if((p_timer->EVENTS_COMPARE[0] ==1) && (p_timer->INTENSET & TIMER_INTENSET_COMPARE0_Msk))     // 仅用来去抖延时
    {
        p_timer->EVENTS_COMPARE[0] = 0;
        p_timer->INTENSET = TIMER_INTENSET_COMPARE0_Disabled << TIMER_INTENSET_COMPARE0_Pos; // 关定时器延时中断0
        //p_timer->TASKS_CLEAR = 1;
        
        NRF_GPIOTE->INTENSET  = GPIOTE_INTENSET_IN0_Enabled << GPIOTE_INTENSET_IN0_Pos;     // 开GPIO引脚中断
        
        if((nrf_gpio_pin_read(8) & 1) && (button_tmp == 0))     // 一次有效高电平
        {
            button_tmp = 1;
           // xprintf("one ok.\r\n");
        }
        else 
        {
            if( (nrf_gpio_pin_read(8) & 1) && (button_tmp == 1))     // 检测到两次有效高电平 结束
            {
                p_timer->TASKS_STOP = 1;
                p_timer->TASKS_CLEAR = 1;

                p_timer->INTENSET = TIMER_INTENSET_COMPARE3_Enabled << TIMER_INTENSET_COMPARE3_Disabled; // 关延时中断2
                BUTTON_STATE = 2;
                button_tmp = 0;
                xprintf("Double click.\r\n");
            }
        }
        //xprintf("10ms.\r\n");
    }
    if( (p_timer->EVENTS_COMPARE[3] ==1) && (p_timer->INTENSET & TIMER_INTENSET_COMPARE3_Msk) && (nrf_gpio_pin_read(8) == 0 )&& (button_tmp == 1))  // 1s延时中断到达，只检测到一次
     {
         p_timer->EVENTS_COMPARE[3] = 0;
         p_timer->TASKS_STOP  = 1;
         p_timer->TASKS_CLEAR = 1;
         p_timer->INTENSET = TIMER_INTENSET_COMPARE0_Disabled << TIMER_INTENSET_COMPARE0_Pos; // 关定时器延时中断0
         p_timer->INTENSET = TIMER_INTENSET_COMPARE3_Disabled << TIMER_INTENSET_COMPARE3_Pos; // 关延时中断
         BUTTON_STATE = 1;
         button_tmp = 0;
         xprintf("Single click.\r\n");
     }
}

// gpio中断配置
void nRF_gpio_INT_config()
{
  nrf_gpio_cfg_input(nRF_gpio_INT_pin, NRF_GPIO_PIN_PULLUP);
	// Enable interrupt:
  NVIC_EnableIRQ(GPIOTE_IRQn);
   // 1,翻转 2,8引脚 3,事件模式
  NRF_GPIOTE->CONFIG[0] =  (GPIOTE_CONFIG_POLARITY_HiToLo << GPIOTE_CONFIG_POLARITY_Pos)
                           | (nRF_gpio_INT_pin << GPIOTE_CONFIG_PSEL_Pos)  
                           | (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos);
    //使能中断
  NRF_GPIOTE->INTENSET  = GPIOTE_INTENSET_IN0_Set << GPIOTE_INTENSET_IN0_Pos;
}


void button_start()
{
    // Start 16 MHz crystal oscillator.
    NRF_CLOCK->EVENTS_HFCLKSTARTED  = 0;    // HFCLK oscillator state = 0.
    NRF_CLOCK->TASKS_HFCLKSTART     = 1;    // Start HFCLK clock source

    // Wait for the external oscillator to start up.
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) 
    {
        // Do nothing.
    }
    p_timer->MODE           = TIMER_MODE_MODE_Timer;        // Set the timer in Timer Mode.
    p_timer->PRESCALER      = 9;                            // Prescaler 9 produces 31250 Hz timer frequency => 1 tick = 32 us.
    p_timer->BITMODE        = TIMER_BITMODE_BITMODE_32Bit;  // 16 bit mode.
    /* Bit 16 : Enable interrupt on COMPARE[0] */
    NVIC_EnableIRQ(TIMER0_IRQn);
    
    p_timer->TASKS_CLEAR    = 1;                            // clear the task first to be usable for later.
    
    //
    p_timer->CC[0]          = 288;    // 15ms延时
    p_timer->CC[1]          = 0;
    p_timer->CC[2]          = 0;
    p_timer->CC[3]          = 9615;    // 0.5s延时
    
    // p_timer->TASKS_START    = 1;                    // Start timer.

    
}


