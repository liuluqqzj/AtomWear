#ifndef _SYSTEM_INFO_STORAGE_H_
#define _SYSTEM_INFO_STORAGE_H_
#include "stdbool.h"

/*--------------------- system work mode -----------------------------*/
#define SYSTEM_DEFAULT_MODE                0x01
#define SYSTEM_PEDOMETER_MODE              0x01
#define SYSTEM_USER_SLEEP_MODE             0x02
#define SYSTEM_BLE_WORK_MODE               0x03
#define SYSTEM_MCU_SLEEP_MODE              0x04
#define SYSTEM_POWER_SHUTDOWN_MODE         0x05
            
/*-------------- system data collect and synchronous -----------------*/
#define SYSTEM_STANDARD_TIME               0x01
#define SYSTEM_SET_ALARM                   0x02
#define SYSTEM_BATTERY_SYNC                0x03
#define SYSTEM_PEDO_START                  0x04
#define SYSTEM_USER_SLEEP_SYNC             0x05
#define SYSTEM_SLICE_TIME_START            0x06
#define SYSTEM_SLICE_TIME_STOP             0x07
#define SYSTEM_RESPONSE                    0x08
#define SYSTEM_DATA_SYNC_END               0x09
#define SYSTEM_DATA_SYNC_AMOUNT            0x0A
#define SYSTEM_DATA_SYNC                   0x0B
#define SYSTEM_RELATIVE_TIME               0x0C
#define SYSTEM_APP_UPDATE                  0x0D
#define SYSTEM_APP_VERSION                 0x11

/*----------------- 地址/扇区/系统状态参数 ----------------------------*/
#define CODE_PAGE_SIZE                     1024
#define BOOTLOADER_ADDRESS                 0x00028000
#define BOOTLOADER_ADDRESS_DEFAULT         0xFFFFFFFF
#define SYSTEM_PARAMETERS_SETTINGS_ADDRESS 0x0003FC00
#define NRF_UICR_BOOT_START_ADDRESS        (NRF_UICR_BASE + 0x14)

#define SYSTEM_APP_VALID_OFFSET            0
#define SYSTEM_APP_VALID                   0x00000001
#define SYSTEM_APP_INVALID                 0xFFFFFFFF

#define SYSTEM_SLEEP_DEADLINE              300 

/*------------------------- 系统参数初始化 ----------------------------*/
void sys_parameters_init(void);

/*------------------------- 系统标准时间判断 -------------------------*/
bool standard_time_judge(void);

/*-------------------------- 返回非标准时间 ---------------------------*/
uint32_t get_no_sync_time(void);

/*-------------------------- 存储非标准时间 ---------------------------*/
void sto_no_sync_time(void);

/*-------------------------- 返回标准时间 ---------------------------*/
uint32_t get_system_time(void);

/*-------------------------- 刷新标准时间 ---------------------------*/
void system_time_update(void);

/*-------------------------- 同步更正标准时间 ---------------------------*/
void set_system_time(uint32_t std_time);

/*-------------------------- 返回系统休眠倒计时 ---------------------------*/
uint32_t get_sleep_countdown(void);

/*-------------------------- 刷新系统休眠倒计时 ---------------------------*/
void system_countdown_update(void);

/*-------------------------- 系统休眠倒计时清零 ---------------------------*/
void clear_sleep_countdown(void);

/*-------------------------- 返回系统版本号 ---------------------------*/
uint16_t get_system_app_version(void);

/*--------------------------- 设置系统电量信息 -------------------------*/
void set_battery_level(uint8_t level);

/*--------------------------- 返回系统电量信息 -------------------------*/
uint8_t get_battery_level(void);

/*--------------------------- 设置系统充电状态 -------------------------*/
void set_charging_level(bool state);

/*--------------------------- 返回系统充电状态 -------------------------*/
bool get_charging_level(void);
   
/*--------------------------- 系统工作状态更改 -------------------------*/
void work_mode_switch(void);

/*--------------------------- 返回系统工作状态 -------------------------*/
uint8_t get_work_mode(void);

/*--------------------------- 初始化系统工作状态 -----------------------*/
void work_mode_init(void);  

/*--------------------------- 返回系统切换状态 -------------------------*/
bool get_work_switch_state(void);

/*--------------------------- 置位系统切换状态 -----------------------*/
void work_switch_set(void);

/*--------------------------- 清除系统切换状态 -----------------------*/
void work_switch_clear(void);
   
/*--------------------------- 系统引导系统检测 -----------------------*/
void mcu_bootloader_check(void); 
   
   
   
   
   
   
   
   
   
/*--------------------------- 计步数据控制初始化 ------------------------------*/
void pedo_data_ctrl_init(void);

/*--------------------------- 返回一个运动存储片段的状态 ------------------------*/
bool get_pedo_slice_handle_state(void);

/*--------------------------- 运动数据的长度 ------------------------------------*/
uint32_t get_pedo_data_length(void);

/*--------------------------- 运动数据擦除 --------------------------------------*/
void pedo_data_sto_ctrl_erase(void);

/*--------------------------- 运动数据片段存储处理 -------------------------------*/
void pedo_data_sto_handle(uint16_t pedo_time, uint16_t pedo_peak, uint16_t pedo_valley);

/*--------------------------- 运动数据片段结束处理 ----------------------------*/
void pedo_data_end_handle(void);

/*--------------------------- 运动时间更新校准 ---------------------------------------*/
void pedo_time_diff_sto_handle(void);

/*--------------------------- 读取运动数据 --------------------------------------*/
/*----- data为发送存储空间，offset为将要读取的地址，length为读取出来的长度 ------*/
void get_pedo_data_handle(uint8_t *data, uint32_t *offset, uint8_t *length);

#endif
