/**
 ****************************************************************************************************
 * @file        freertos.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       FreeRTOS ʵ��
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� ��ӢF103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#ifndef __TASK_FREQ_H
#define __TASK_FREQ_H
#include "freertos_demo.h"



/* ���� �� �жϱ�� & �жϷ����� ���� */ 

#define speed_INT_GPIO_PORT              GPIOC
#define speed_INT_GPIO_PIN               GPIO_PIN_0
#define speed_INT_IRQn                   EXTI0_IRQn

#define KEY0_INT_GPIO_PORT              GPIOC
#define KEY0_INT_GPIO_PIN               GPIO_PIN_1
#define KEY0_INT_IRQn                   EXTI1_IRQn

#define KEY1_INT_GPIO_PORT              GPIOC
#define KEY1_INT_GPIO_PIN               GPIO_PIN_2
#define KEY1_INT_IRQn                   EXTI2_IRQn

/* ���� ���� ���� */ 

#define SPEED_MAGNET                      1    /*   �⳵�ٵĴ�������    */
#define RPM_MAGNET                        12    /*   ��̤Ƶ�Ĵ�������    */

/* ̤Ƶ�������Ľṹ�� */
typedef struct
{
    uint8_t sta;             /* ���жϷ��� sta = 1 */
    uint8_t dir;             /* ��ת���� 0=��ת */
    uint32_t last_time;      /* ��һ�ν����жϵ�ʱ�� */
    uint32_t get_time;       /* ���жϵļ����������������������ٶ� */
} freq_typedef;

/*�ٶȴ������Ľṹ�� */
typedef struct
{
    uint8_t sta;             /* ���жϷ��� sta = 1 */
    uint8_t get_num;        /* ��������õģ�ת100Ȧ����һ����� */
    uint32_t last_time;      /* ��һ�ν����жϵ�ʱ�� */
    uint32_t get_time;       /* ���жϵļ����������������������ٶ� */
} speed_typedef;

/* �����ٶȺ�̤Ƶ�Ľṹ�� */
typedef struct
{
    uint16_t speed;
    uint16_t rpm;
} GET_speed_typedef;

/*************�������� **************/

extern QueueHandle_t   g_xQueue_freq;
extern TaskHandle_t    task_freq_Handler;

/************* API **************/
void T_EXTI_init(void);  /* �ⲿ�жϳ�ʼ�� */
void task_freq(void *pvParameters);

#endif
