#include "twi_master.h"
#include "nrf_delay.h"
#include "bma180.h"
#include "xprintf.h"
#include "nrf_gpio.h"
#include "nrf_error.h"
#include "boards.h"

#define nRF_INT_pin 	12		//引脚12中断

uint8_t signal = 0;

volatile bool system_wait_high_g_INT = false;

//IIC初始化
void IIC_Init() 
{ 
  if(!twi_master_init()) 
	{	
		//simple_uart_putstring("I2C initate failed..\n");
		while(1);
	}
	//simple_uart_putstring("I2C initate successful!..\n");
} 

//向bma180寄存器写一个字节
bool bma_write(uint8_t reg_addr, uint8_t reg_data)
{
    uint8_t send[2];
    send[0] = reg_addr;
    send[1] = reg_data;
    return twi_master_transfer(bma180_addr_write, send, 2 ,TRANS_STOP );
}

//从bma180寄存器读取一个字节
uint8_t bma_byte_read(uint8_t reg_addr)
{
    uint8_t send_rec[1];
    send_rec[0] = reg_addr;
    twi_master_transfer(bma180_addr_write, send_rec, 1 ,TRANS_CONTINUE );
    twi_master_transfer(bma180_addr_read,   send_rec, 1 ,TRANS_STOP );
    return send_rec[0];
}

//从bma180读取多个字节
bool bma_multibyte_read(uint8_t reg_addr, uint8_t rec[], uint8_t rec_length)
{
    uint8_t send[1]; 
    send[0] = reg_addr;
    twi_master_transfer(bma180_addr_write, send, 1 , TRANS_CONTINUE );
    return twi_master_transfer(bma180_addr_read,  rec, rec_length , TRANS_STOP);
}

void bma_reset()
{
	//软件复位操作，reset寄存器地址：0x10,复位数据：0xB6
    bma_write(0x10, 0xB6);
    nrf_delay_ms(30);
}

void bma_write_enable()
{
    bma_write(0x0D, 0x10);
    nrf_delay_ms(10);
}

void bma_write_disable()
{
    bma_write(0x0D, 0x00);
    nrf_delay_ms(10);
}

//中断位清零
void bma_reset_int()
{
    uint8_t send_rec;
    send_rec = bma_byte_read(0x0D);
    send_rec = send_rec|0x40;
    bma_write(0x0D, send_rec);
}

//bma180基本工作模式配置
void bma180_basic_config() 
{ 
    uint8_t send_rec; 
    //软件复位
	bma_reset(); 
    
    //开启传感器写
    bma_write_enable();
       
	//工作模式配置 ，地址：0x30,低2位：00满功耗。
	send_rec = bma_byte_read(0x30);
	send_rec = send_rec&0xFC; 
	bma_write(0x30, send_rec);
    nrf_delay_ms(10);
	
	//不要唤醒模式
	bma_write(0x34, 0x80);
	nrf_delay_ms(10);
    
	//读频宽寄存器，地址：20h,数据高四位为bw,低四位为tcs.
    send_rec = bma_byte_read(0x20);
	send_rec= (send_rec&0x0F)|0x20;	//高4位0010,为40Hz低通模式
	bma_write(0x20, send_rec);
    nrf_delay_ms(10);
    
    //量程设置，地址：0x35h,1~3位为量程，0100为4g量程
	send_rec = bma_byte_read(0x35);
	send_rec =(send_rec&0xF1)|0x08;		//1~3位清零，然后等于(100)0，即4g.
	bma_write(0x35, send_rec);
    nrf_delay_ms(10);
} 
 

void bma_new_data_INT_config()
{
    bma_write(0x21, 0x03);
	nrf_delay_ms(10);
}
	
void shake_dect_open()
{
    //uint8_t send_rec;
    
    bma_write(0x2A, 0x5F);	// A0 high_th   (0111 0000) 高重力加速度临界值
	
    bma_write(0x23, 0x10);  // 60 high_hy 防止太过接近临界值  +-410/32=13 =1101 = (0110 0)000

	bma_write(0x25, 0x20);  //high_int_z,z轴产生高g  0011 0000 使用high filt

    bma_write(0x22, 0x30);  //mot_cd_r 递减方式	 0001 0000 count down by 2LSB 
	
    bma_write(0x27, 0x30);  //high_dur 计数值个数  (0010 000)0 
	
    bma_write(0x21,0x20);   //enable INT  0010 0001 enable high-g and no lab-int
    
    system_wait_high_g_INT = true;
    //中断使能
    //send_rec = bma_byte_read(0x21);
	//send_rec = send_rec|0x20; //开high g int
	//bma_write(0x21, send_rec);
}

void shake_dect_close()
{
    bma_write(0x21, 0x08);
}

bool get_shake_state()
{
    return system_wait_high_g_INT;
}

void clear_shake_state()
{
    system_wait_high_g_INT = false;
}


void bma180_tap_INT_config()
{
	uint8_t send_rec;
	
	//力度临界值
    bma_write(0x28, 0x80);     //0010 1001 100110b = 2662 = 
	
	//x、y、z定义触发
    send_rec = bma_byte_read(0x24);
	send_rec = send_rec|0x02;   //z开启 滤波
	bma_write(0x24, send_rec);
 
	//敲击时长，低噪下
    send_rec = bma_byte_read(0x31);
	send_rec = send_rec|0x04; //0000 0100b 250ms
	bma_write(0x31, send_rec);
	
    //中断使能
    send_rec = bma_byte_read(0x21);
	send_rec = send_rec|0x08; //开tapsense_int
	bma_write(0x21, send_rec);
	
    //tapsens_shock = (120*Tupdate) = 50 ms
	nrf_delay_ms(10);
}

 
/**
 * 读取bma180的三轴加速度值
*/
void bma180_read(uint8_t acce[]) 
{ 
    bma_multibyte_read(0x02, acce, 6); //x轴加速度LSB基地址
}


/**
 * 将读取到的MSB与LSB转换为一个浮点数
 *  g-range:       +/-1.0 g  8192  
 *  S1.5g g-range: +/-1.5 g  5460  
 *  S2g g-range:   +/-2.0 g  4096  
 *  S3g g-range:   +/-3.0 g  2730  
 *  S4g g-range:   +/-4.0 g  2048  
 *  S8g g-range:   +/-8.0 g  1024  
 *  S16g g-range:  +/-16 g   512  
*/
float byte_to_float(uint8_t LSB, uint8_t MSB, int typ)
{//由于LSB,MSB分别只有8位，不能对其进行左移，否则数据发生错误。
	int16_t acce;
	float f_acce;
	acce = MSB;
	acce = acce<<6;	//将高位先存储，然后左移6位，剩下的6位给LSB
	acce += ( LSB>>2 );	//将LSB先右移2位，去掉无效低2位，再存在data中。
	
	//00 10 0000 0000 0000b，最高位0正1负
	if( acce & 0x2000 )
	{ 
		acce = ~acce;
		acce &= ~0xc000;//最高2位没用到，取反+1，求源码
		acce++;
		acce = -acce;
		f_acce= (acce*1.00)/typ;
		//xprintf("\nthe acce16 is = %d\n",acce);
		return f_acce;	//反向加速度
	}
	  //xprintf("\nthe acce16 is = %d\n",acce);
	f_acce= (acce*1.00)/typ;
	return f_acce;		
}

/**
 * 将一个32位有符号浮点数转换成8位无符号字符数组存储。
 * 返回字符数组的长度,arrary至少要6位以上。
 */
unsigned char Float_to_Char(float value, unsigned char* array) 
{
	uint8_t IntegerPart;
	float DecimalPart;
	uint8_t i = 1;
	uint8_t j = 0;
	char temp;
	
	if(value<0)
	{
			value = -value;
			array[0] = '-';
  }else{
	array[0] = '+';
	}
	
	if (value >= 1) 
	{
		IntegerPart = (uint8_t) value;
		DecimalPart = value - IntegerPart;
  } else {
		IntegerPart = 0;
		DecimalPart = value - IntegerPart;
	}
	
	if (IntegerPart == 0) {
		array[1] = '0';
		array[2] = '.';
		i = 2;
	 } else {
		while (IntegerPart > 0) {
			array[i] = IntegerPart % 10 + '0';
			IntegerPart /= 10;
			i++;
		}
		i--;
		
    //fix the integer result
		for (j = 1; j < i; j++) {
			temp = array[j];
			array[j] = array[i - j];
			array[i - j] = temp;
		}
		i++;
		array[i] = '.';
	}
//convert the Decimalpart
	i++;
	array[i++] = (uint8_t) (DecimalPart * 10) % 10 + '0';
	array[i++] = (uint8_t) (DecimalPart * 100) % 10 + '0';
	array[i++] = (uint8_t) (DecimalPart * 1000) % 10 + '0';
	array[i++] = (uint8_t) (DecimalPart * 10000) % 10 + '0';
	array[i] = '\0';
	return i;
}

void bma_xyz_read_print()
{
        uint8_t  acce[6],acce_string[10],i,j;
        float    f_acce[3];
	
            bma180_read(acce);
			f_acce[0] = byte_to_float(acce[0], acce[1], 2048);
			f_acce[1] = byte_to_float(acce[2], acce[3], 2048);
			f_acce[2] = byte_to_float(acce[4], acce[5], 2048);
		
			i=Float_to_Char(f_acce[0], acce_string);
			for(j=0; j<i; j++)
			{
				xprintf("%c", acce_string[j]);
			}
			xprintf("  ");
		
			i=Float_to_Char(f_acce[1], acce_string);
			for(j=0; j<i; j++)
			{
				xprintf("%c", acce_string[j]);
			}
			xprintf("  ");
		
			i=Float_to_Char(f_acce[2], acce_string);
			for(j=0; j<i; j++)
			{
				xprintf("%c", acce_string[j]);
			}
			xprintf("\n");
}



void bma180_init()
{
    //IIC_Init();
    bma180_basic_config();
    bma180_tap_INT_config();  
    //nRF_tap_INT_config();	//开引脚中断
}
