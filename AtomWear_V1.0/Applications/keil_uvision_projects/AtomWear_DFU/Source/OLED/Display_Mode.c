#include "IC.h"
#include "IC_Init.h"
#include "Photo_Show.h"
#include "twi_master.h"
#include "nrf_delay.h"
#include "Display_Mode.h"
#include "Photo_Show.h"
//
//

//extern uint8_t ROW[3][8];

uint8_t ROW[3][8]={
{0x49,0x92,0x24,0x49,0x92,0x24,0x49,0x92},
{0x92,0x24,0x49,0x92,0x24,0x49,0x92,0x24},
{0x24,0x49,0x92,0x24,0x49,0x92,0x24,0x49}
};

/******************************************************
//
//清屏显示模式
//
******************************************************/
//
//

void Clear_Screen()
{
   uint8_t page_number,column_number;
   for(page_number=OLED_MIN;page_number<PAGE_TOTAL;page_number++)
   {
     Write_Command(START_PAGE+page_number);
     Write_Command(START_HIGH_BIT);
     Write_Command(START_LOW_BIT);
     for(column_number=OLED_MIN;column_number<COLUMN_MAX;column_number++)
     {
        Write_Data(STATE_MIN);
     }
   }
}

/******************************************************
//
//全屏显示模式
//
******************************************************/
//
//

void All_Screen()
{
   uint8_t page_number,column_number;
   for(page_number=OLED_MIN;page_number<PAGE_TOTAL;page_number++)
   {
     Write_Command(START_PAGE+page_number);
     Write_Command(START_HIGH_BIT);
     Write_Command(START_LOW_BIT);
     for(column_number=OLED_MIN;column_number<COLUMN_MAX;column_number++)
     {
        Write_Data(STATE_MAX);
     }
   }
}


/******************************************************
// #define OLED_MIN 0
// #define PAGE_TOTAL 4
// #define COLUMN_MAX 64
// #define ROW_MAX 32
******************************************************/
void oled_display(const uint8_t * pic)
{
   uint8_t page_number,column_number;
   uint32_t piexl=OLED_MIN;
   for(page_number=OLED_MIN;page_number<PAGE_TOTAL;page_number++)
   {
      Write_Command(START_PAGE+page_number);
      Write_Command(START_HIGH_BIT);
      Write_Command(START_LOW_BIT);
      for(column_number=OLED_MIN;column_number<COLUMN_MAX;column_number++)
      {
		 Write_Data(pic[piexl]);
		 piexl++;
	  }
  }
}



void oled_test()
{
   //Reset_IC();                    //复位IC
   Init_IC();                     //初始化IC
   Clear_Screen();
   // All_Screen();                  //全屏显示效果
   while(1)
   {
      // All_Screen();  

      //Clear_Screen();
      oled_display(display_system_init);

	  Delay_s(2);
      oled_display(BBBBB);
      Delay_s(3);

   }
}


/***********************************************************************/
