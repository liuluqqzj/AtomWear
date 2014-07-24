#ifndef _STEP_COUNT_H_
#define _STEP_COUNT_H_


#include "stdint.h"


//pedo_state
#define PEDO_COLLECT_PREPARE        1
#define PEDO_COLLECT_START          2
#define PEDO_COLLECT_STOP           3

// 普通结点结构体
struct g_node
{
    uint16_t value;
    uint8_t  time;
};


// 关键结点结构体
struct s_node  
{
    uint16_t value;
    uint8_t  time;
};

uint8_t get_pedo_start_state(void);

void set_pedo_start_state(uint8_t state);

void set_pedo_stop(void);



void pedometer_init(void);

void pedometer_startup(void);



#endif
