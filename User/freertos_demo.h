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

#ifndef __FREERTOS_DEMO_H
#define __FREERTOS_DEMO_H

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "semphr.h"
#include "queue.h"



/* ���Բ�������*/
#define EBIKE_HOT                   9500              /* ���±�����ֵ25��=2500 */
//#define EBIKE_LOW_VOLTAGE           17000            /* �͵���������ֵmv */
//#define EBIKE_VOLTAGE               21000            /* �������mv */
#define EBIKE_VOLTAGE_S              5            /* �������mv */
#define EBIKE_STARTRPM              15               /* ��������ʱ��̤Ƶrpm/min */
#define EBIKE_MAX_CURRENT           28000            /* ���Ƶ������� ma  �������ˢ������������С30000*/
#define EBIKE_MOTOR_MAXRPM          3500             /* ��������ת�� rpm/min*/



/*E_BIKEģʽ*/
typedef enum
{
    e_mode_ebike = 1,
    e_mode_motor,
    e_mode_leg
}e_mode_typedef;

/*E_BIKE��λ*/
//typedef enum
//{
//    e_sport_d = 1,
//    e_sport_s,
//    e_sport_r
//}e_sport_typedef;
/*�¼���־λ����*/

#define Ev_ADC           (1 << 0)            /* ADC���ݴ������ */
#define Ev_FREQ          (1 << 1)
#define Ev_BEEP          (1 << 2)
#define Ev_LIG_ON        (1 << 3)
#define Ev_LIG_OFF       (1 << 4)


/*E_BIKE״̬�ṹ��*/
typedef struct{
    e_mode_typedef   mode;  /*E_BIKEģʽ*/
//    e_sport_typedef  sport; /*E_BIKE��λ*/
    uint16_t  voltage;    /* ��ѹ_���� */
    uint16_t  hot;        /* �¶�*100 */
    uint16_t  rs485_error;  /* 485������� */
    uint16_t  rpm;        /* ̤Ƶ */
    uint16_t  size;       /* ���ֵ��ܳ� mm*/
    uint16_t  speed;      /* ���� */
    uint32_t  mileage;              /* �����km */
    uint16_t  set_motor_cun;   /* ���õ�Ť�غ͵��� */
    uint16_t  set_motor_pwm;   /* ���õĵ��pwm */
    uint16_t  get_motor_rpm;   /* ��ȡ���ĵ��ת��,����̤Ƶ���ڿ��Ƶ��ת�� */
    uint16_t  get_motor_cun;   /* ��ȡ���ĵ������ */
    uint16_t  e_maxcun;         /* e_bikeģʽ�������� */
    uint16_t  m_maxcun;         /* motorģʽ�������� */
}EBIKE_TYPEDEF;


extern SemaphoreHandle_t MutexSemaphore_rs485;         /* �����ź���  */
extern SemaphoreHandle_t MutexSemaphore_my_ebike;         /* �����ź���  */
extern EventGroupHandle_t g_EventGroup;
extern volatile EBIKE_TYPEDEF  my_ebike;
void freertos_demo(void);
#endif
