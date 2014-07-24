#ifndef _MPU6050_H_
#define _MPU6050_H_

#include "stdint.h"


#define MPU6050_ADDR0       0x68    // 1101000b AD0 = 0
#define MPU6050_ADDR1       0x69    // 1101001b AD0 = 1  

#define SMPRT_DIV           0x19    // 陀螺仪采样率
#define CONFIG              0x1A    // 低通滤波及外部帧同步配置
#define GYRO_CFG            0x1B    // 陀螺仪自检及量程配置
#define ACCEL_CFG           0x1C    // 加速度计自检及量程配置
#define FIFO_EN             0x23    // FIFO使能配置
#define INT_EN              0x38    // 中断使能配置

#define ACCEL_XOUT_H        0x3B    //  X轴加速度值高8位
#define ACCEL_XOUT_L        0x3C    //  ...........低...  
#define ACCEL_YOUT_H        0x3D
#define ACCEL_YOUT_L        0x3E
#define ACCEL_ZOUT_H        0x3F
#define ACCEL_ZOUT_L        0x40

#define	TEMP_OUT_H		    0x41
#define	TEMP_OUT_L		    0x42

#define	GYRO_XOUT_H		    0x43
#define	GYRO_XOUT_L		    0x44	
#define	GYRO_YOUT_H		    0x45
#define	GYRO_YOUT_L		    0x46
#define	GYRO_ZOUT_H		    0x47
#define	GYRO_ZOUT_L		    0x48

#define SIG_PATH_RST        0x68    // reset the analog and digital signal paths of \
                                       the gyroscope, accelerometer, and temperature sensors
#define USER_CTRL           0x6A    // 使能及复位FIFO，复位所有传感器
#define PWR_MGMT_1          0x6B    // 睡眠控制位在此
#define PWR_MGMT_2          0x6C   
#define FIFO_RW             0x74
#define WHO_AM_I            0x75

typedef float mpu6050_temp;

typedef struct 
{
    int16_t X;
    int16_t Y;
    int16_t Z;
}mpu6050_accel;

typedef struct 
{
    int16_t X;
    int16_t Y;
    int16_t Z;
}mpu6050_gyro;


void mpu6050_init(void);

mpu6050_temp mpu6050_get_temp(void);

mpu6050_accel mpu6050_get_accel(void);

mpu6050_gyro mpu_get_gyro(void);

void mpu6050_test(void);

#endif

