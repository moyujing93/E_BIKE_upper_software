/**
 ****************************************************************************************************
 * @file        freertos.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       FreeRTOS 实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 精英F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#ifndef __TASK_24G_H
#define __TASK_24G_H
#include "freertos_demo.h"



/* 引脚 和 中断编号 & 中断服务函数 定义 */ 





/*************参数定义 **************/

extern TaskHandle_t    task_24g_Handler;
extern QueueHandle_t   g_xQueue_24g;

/************* API **************/

void task_24g(void *pvParameters);

#endif
