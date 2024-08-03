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

#ifndef __TASK_24G_H
#define __TASK_24G_H
#include "freertos_demo.h"



/* ���� �� �жϱ�� & �жϷ����� ���� */ 





/*************�������� **************/

extern TaskHandle_t    task_24g_Handler;
extern QueueHandle_t   g_xQueue_24g;

/************* API **************/

void task_24g(void *pvParameters);

#endif
