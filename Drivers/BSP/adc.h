/**
 ****************************************************************************************************
 * @file        spi.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       SPI ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 */

#ifndef _ADC_H
#define _ADC_H
#include "./SYSTEM/sys/sys.h"
/***************************************** �ṹ��ͱ��� *************************************************/

/* ��λ�� */
#define ACC_PIN             GPIO_PIN_0
#define ACC_PORT            GPIOA

/* ���������� */
#define LSEN_PIN             GPIO_PIN_1
#define LSEN_PORT            GPIOA

/*  �ṹ���������������DMA��������  */
#define DATA_NUM               2

/* ������ݵĽṹ�� */
typedef struct
{
    volatile uint32_t  ACC_cun;
    volatile uint32_t  LSEN_cun;
}GET_ADC_typedef;



/***************************************** ���� *************************************************/
void adc1_dma_init(void);
void adc_dma_conver(GET_ADC_typedef *get_adc);




















#endif
