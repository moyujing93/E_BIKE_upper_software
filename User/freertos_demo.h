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

#ifndef __FREERTOS_DEMO_H
#define __FREERTOS_DEMO_H

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "semphr.h"
#include "queue.h"



/* 绝对参数定义*/
#define EBIKE_HOT                   9500              /* 高温保护阈值25度=2500 */
//#define EBIKE_LOW_VOLTAGE           17000            /* 低电量保护阈值mv */
//#define EBIKE_VOLTAGE               21000            /* 满电电量mv */
#define EBIKE_VOLTAGE_S              5            /* 满电电量mv */
#define EBIKE_STARTRPM              15               /* 助力启动时的踏频rpm/min */
#define EBIKE_MAX_CURRENT           28000            /* 控制的最大电流 ma  必须比无刷驱动的最大电流小30000*/
#define EBIKE_MOTOR_MAXRPM          3500             /* 电机的最大转速 rpm/min*/



/*E_BIKE模式*/
typedef enum
{
    e_mode_ebike = 1,
    e_mode_motor,
    e_mode_leg
}e_mode_typedef;

/*E_BIKE挡位*/
//typedef enum
//{
//    e_sport_d = 1,
//    e_sport_s,
//    e_sport_r
//}e_sport_typedef;
/*事件标志位定义*/

#define Ev_ADC           (1 << 0)            /* ADC数据处理完毕 */
#define Ev_FREQ          (1 << 1)
#define Ev_BEEP          (1 << 2)
#define Ev_LIG_ON        (1 << 3)
#define Ev_LIG_OFF       (1 << 4)


/*E_BIKE状态结构体*/
typedef struct{
    e_mode_typedef   mode;  /*E_BIKE模式*/
//    e_sport_typedef  sport; /*E_BIKE挡位*/
    uint16_t  voltage;    /* 电压_毫伏 */
    uint16_t  hot;        /* 温度*100 */
    uint16_t  rs485_error;  /* 485错误计数 */
    uint16_t  rpm;        /* 踏频 */
    uint16_t  size;       /* 车轮的周长 mm*/
    uint16_t  speed;      /* 车速 */
    uint32_t  mileage;              /* 总里程km */
    uint16_t  set_motor_cun;   /* 设置的扭矩和电流 */
    uint16_t  set_motor_pwm;   /* 设置的电机pwm */
    uint16_t  get_motor_rpm;   /* 获取到的电机转速,根据踏频用于控制电机转速 */
    uint16_t  get_motor_cun;   /* 获取到的电机电流 */
    uint16_t  e_maxcun;         /* e_bike模式下最大电流 */
    uint16_t  m_maxcun;         /* motor模式下最大电流 */
}EBIKE_TYPEDEF;


extern SemaphoreHandle_t MutexSemaphore_rs485;         /* 互斥信号量  */
extern SemaphoreHandle_t MutexSemaphore_my_ebike;         /* 互斥信号量  */
extern EventGroupHandle_t g_EventGroup;
extern volatile EBIKE_TYPEDEF  my_ebike;
void freertos_demo(void);
#endif
