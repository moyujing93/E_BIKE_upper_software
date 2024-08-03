/**
 ****************************************************************************************************
 * @file        spi.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       SPI 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 */

#ifndef _ADC_H
#define _ADC_H
#include "./SYSTEM/sys/sys.h"
/***************************************** 结构体和变量 *************************************************/

/* 电位器 */
#define ACC_PIN             GPIO_PIN_0
#define ACC_PORT            GPIOA

/* 光敏二极管 */
#define LSEN_PIN             GPIO_PIN_1
#define LSEN_PORT            GPIOA

/*  结构体的数量用于设置DMA传输数量  */
#define DATA_NUM               2

/* 存放数据的结构体 */
typedef struct
{
    volatile uint32_t  ACC_cun;
    volatile uint32_t  LSEN_cun;
}GET_ADC_typedef;



/***************************************** 函数 *************************************************/
void adc1_dma_init(void);
void adc_dma_conver(GET_ADC_typedef *get_adc);




















#endif
