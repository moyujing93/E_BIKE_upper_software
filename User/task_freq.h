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

#ifndef __TASK_FREQ_H
#define __TASK_FREQ_H
#include "freertos_demo.h"



/* 引脚 和 中断编号 & 中断服务函数 定义 */ 

#define speed_INT_GPIO_PORT              GPIOC
#define speed_INT_GPIO_PIN               GPIO_PIN_0
#define speed_INT_IRQn                   EXTI0_IRQn

#define KEY0_INT_GPIO_PORT              GPIOC
#define KEY0_INT_GPIO_PIN               GPIO_PIN_1
#define KEY0_INT_IRQn                   EXTI1_IRQn

#define KEY1_INT_GPIO_PORT              GPIOC
#define KEY1_INT_GPIO_PIN               GPIO_PIN_2
#define KEY1_INT_IRQn                   EXTI2_IRQn

/* 参数 常量 定义 */ 

#define SPEED_MAGNET                      1    /*   测车速的磁铁个数    */
#define RPM_MAGNET                        12    /*   测踏频的磁铁个数    */

/* 踏频传感器的结构体 */
typedef struct
{
    uint8_t sta;             /* 有中断发生 sta = 1 */
    uint8_t dir;             /* 旋转方向 0=正转 */
    uint32_t last_time;      /* 上一次进入中断的时间 */
    uint32_t get_time;       /* 两中断的间隔，主函数根据这个计算速度 */
} freq_typedef;

/*速度传感器的结构体 */
typedef struct
{
    uint8_t sta;             /* 有中断发生 sta = 1 */
    uint8_t get_num;        /* 计算里程用的，转100圈叠加一次里程 */
    uint32_t last_time;      /* 上一次进入中断的时间 */
    uint32_t get_time;       /* 两中断的间隔，主函数根据这个计算速度 */
} speed_typedef;

/* 返回速度和踏频的结构体 */
typedef struct
{
    uint16_t speed;
    uint16_t rpm;
} GET_speed_typedef;

/*************参数定义 **************/

extern QueueHandle_t   g_xQueue_freq;
extern TaskHandle_t    task_freq_Handler;

/************* API **************/
void T_EXTI_init(void);  /* 外部中断初始化 */
void task_freq(void *pvParameters);

#endif
