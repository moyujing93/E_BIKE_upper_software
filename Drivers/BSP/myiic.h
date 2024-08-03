#ifndef __MYIIC_H
#define __MYIIC_H
#include "./SYSTEM/sys/sys.h"

void iic_gpioinit(void);
void iic_start(void);
void iic_stop(void);
uint8_t iic_waitack(void);
void iic_send(uint8_t data);
uint8_t iic_read(uint8_t ack);







#endif
