//****************************包含头文件***************************/
#include "twi_master.h"
#include "nrf_delay.h"
#include "IC.h"
#include "nrf_delay.h"


#define TRANS_STOP		     1
#define TRANS_CONTINUE		 0


/**********************************************
//
//IIC通信开始函数
//
**********************************************/

// void IC_IIC_Start()
// {
//    SDA = high;
//    SCL = high;
//    _nop_();
//    SDA = low;
//    _nop_();_nop_();
//    SCL = low;
// }

/**********************************************
//
//IIC通信停止函数
//
**********************************************/

// void IC_IIC_Stop()
// {
//    SDA = low;
//    _nop_();
//    SCL = high;
//    _nop_();_nop_();
//    SDA = high;
// }

/**********************************************
//
//向IIC写数据函数
//返回值为acknowledgement位信号
//
**********************************************/

// bit Write_IIC_Data(uchar Data)
// {
// 	unsigned char i;
// 	bit Ack_Bit;                    //应答信号
// 	for(i = 0; i < 8; i++)		
// 	{
// 		SDA = (bit)(Data & 0x80);
// 		_nop_();
// 		SCL = high;
// 		_nop_();_nop_();
// 		SCL = low;
// 		Data = _crol_(Data,1);
// 	}
// 	SDA = high;		                //释放IIC SDA总线为主器件接收从器件产生应答信号	
// 	_nop_();_nop_();
// 	SCL = high;                     //第9个时钟周期
// 	_nop_();_nop_();
// 	Ack_Bit = SDA;		            //读取应答信号
// 	SCL = low;
// 	return Ack_Bit;		
// }

/**********************************************
//
//写命令函数

**********************************************/
//
//

void Write_Command(uint8_t command)
{
    uint8_t send[2];
    // send[0] = Slave_Address;
    send[0] = OP_Command;
    send[1] = command;
    twi_master_transfer(Slave_Address, send, 2 ,TRANS_STOP );
}

/**********************************************
//
//写数据函数
//
**********************************************/
//
//

void Write_Data(uint8_t data)
{
    uint8_t send[2];
    // send[0] = Slave_Address;
    send[0] = OP_Data;
    send[1] = data;
    twi_master_transfer(Slave_Address, send, 2 ,TRANS_STOP );
}



//
//复位IC函数
//
//******************************************************/
//
//

// void Reset_IC()
// {
//    Delay_Ms(10);
//    LED_Work = low;
//    RES = low;
//    Delay_Ms(50);
//    RES = high;
//    Delay_Ms(100);
//    VCC_Change = high;
// }

/******************************************************
//
//初始化IC函数
//
******************************************************/
//
//

void Init_IC()
{

   Write_Command(0xAE);     //Set Display Off

   Write_Command(0xd5);     //display divide ratio/osc. freq. mode
   Write_Command(0xd1);     //105Hz	  内部晶振470 KHz
 
   Write_Command(0xA8);     //multiplex ration mode:31
   Write_Command(0x1F);		//32

   Write_Command(0xD3);     //Set Display Offset
   Write_Command(0x00);
   
   Write_Command(0x40);     //Set Display Start Line

   Write_Command(0x8D);     //Set Display Offset
   //Write_Command(0x10);		//disable charge pump
   Write_Command(0x14);	//for enabling charge pump

   Write_Command(0xA1);     //Segment Remap

   Write_Command(0xC8);     //Sst COM Output Scan Direction

   Write_Command(0xDA);     //common pads hardware: alternative
   Write_Command(0x12);

   Write_Command(0x81);     //contrast control 
   Write_Command(0xcf);		// 内部0xCF


   Write_Command(0xD9);	    //set pre-charge period
   Write_Command(0xF1);

   Write_Command(0xDB);     //VCOM deselect level mode
   Write_Command(0x40);	    //set Vvcomh=0.83*Vcc

   Write_Command(0xA4);     //Set Entire Display On/Off

   Write_Command(0xA6);     //Set Normal Display

   Write_Command(0xAF);     //Set Display On
}


