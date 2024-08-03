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

#ifndef __TASK_OUT_RS485_H
#define __TASK_OUT_RS485_H

#include "freertos_demo.h"
#include "./BSP/rs485.h"


/*************�������� **************/

extern TaskHandle_t    task_out_rs485_Handler;

/************* API **************/
void task_out_rs485(void *pvParameters);

#endif
