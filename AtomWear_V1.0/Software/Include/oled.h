#ifndef  _OLED_H_
#define  _OLED_H_

#define high 1
#define low  0 
#define W_Data 0x5C
//
//
//

#define STATE_MAX 0xFF
#define STATE_MIN 0x00
#define STATE_55 0x55
#define STATE_AA 0xAA
#define START_PAGE 0xB0
#define PAGE_TOTAL 4
#define START_HIGH_BIT 0x12
#define START_LOW_BIT 0x00
#define FRAME_HIGH_ROW 0x01
#define FRAME_LOW_ROW 0x80

#define OLED_MIN 0
#define COLUMN_MAX 64
#define ROW_MAX 32
//
//
#define	Slave_Address 0x78		// ������ַ
#define	OP_Command 0x00
#define	OP_Data 0x40
//
//
#define Manual 1
#define Auto 0
//
//
/**********************************************/
//
//
typedef unsigned int   uint;
typedef unsigned char  uchar;
typedef unsigned long  ulong;

void OLED_DrawPoint(uint8_t, uint8_t, uint8_t);
void OLED_ShowChar(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);
void OLED_ShowString(uint8_t x,uint8_t y,const uint8_t *p);
void OLED_Refresh_Gram(void);
void oled_test(void);

//
//

#endif
