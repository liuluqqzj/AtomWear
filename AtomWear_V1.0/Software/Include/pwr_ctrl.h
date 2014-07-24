#ifndef _PWR_CTRL_H_
#define _PWR_CTRL_H_

#include "stdbool.h"

bool battery_self_detect(void);

void system_on_sleep(void);

void battery_detect_start(void);

bool battery_is_available(void);

void battery_detect_stop(void);

uint8_t get_battery_value(void);

#endif 
