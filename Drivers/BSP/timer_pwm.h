#ifndef __TIMER_PWM_H
#define __TIMER_PWM_H
#include "stm32f1xx.h"



void timer_pwm_init(uint16_t arr);
void pwm_setcompare(uint16_t duty);





#endif
