#ifndef __BUZZER_H
#define __BUZZER_H
/*
BEEP	PC7
高电平	响
低电平	不响
*/
#include "stm32f10x_conf.h"

extern void buzzer_init(void);

extern void buzzer_on(void);

extern void buzzer_off(void);

#endif



