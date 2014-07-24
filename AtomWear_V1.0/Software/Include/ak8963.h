#ifndef _AK8963_H_
#define _AK8963_H_

#include "stdint.h"

#define AK8963_ADDR		0x0C

#define AK8963_WIA		0x00
#define AK8963_INFO		0x01
#define AK8963_ST1		0x02
#define AK8963_HXL		0x03
#define AK8963_HXH		0x04
#define AK8963_HYL		0x05
#define AK8963_HYH		0x06
#define AK8963_HZL		0x07
#define AK8963_HZH		0x08
#define AK8963_ST2		0x09

#define AK8963_CNTL1	0x0A
#define AK8963_CNTL2	0x0B
#define AK8963_ASTC		0x0C
#define AK8963_TS1		0x0D
#define AK8963_TS2		0x0E
#define AK8963_I2CDIS	0x0F

#define AK8963_ASAX		0x10
#define AK8963_ASAY		0x11
#define AK8963_ASAZ		0x12
#define AK8963_RSV		0x13


void ak8963_init(void);
void self_test(void);
void ak8963_test(void);

#endif

