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

#ifndef __MY_GUI_H
#define __MY_GUI_H


#include "lvgl.h"
#include "freertos_demo.h"


/*LVGL���ƽṹ��*/
typedef struct
{
    uint16_t  sta;                 /* ���ݿ��ñ�־���ı���������1�������������ж�sta = 1�Ÿ�����Ϣ */
    uint16_t  eeprom_sta;          /* ��û��д�뵽EEPROM�ı�־�����ֵ=12345����д��� */
    uint16_t  pwm_sta;             /* �Ƿ�ʹ��ת�ٿ��� */
    uint16_t  auto_light_sta;      /* �Զ���ƿ��� */
    uint16_t  light_Threshold;     /* �Զ���������� */
    uint16_t  auto_cun;            /* mpu6050�������������Ȱٷֱ� */
    uint16_t  e_maxcun;            /* e_bikeģʽ���������ٷֱ� */
    uint16_t  m_maxcun;            /* motorģʽ���������ٷֱ� */
    uint16_t  sport_num;           /* ��ǰ��λ�ĵ����ٷֱ� */
    uint16_t  mode;                /* ģʽ */
    uint32_t  mileage;             /* �����km */
    uint16_t  size;                /* ���ֵ��ܳ� mm*/
}lvgl_set_typedef;


/*  FLASHģ��EEPROM */
#define EEP_LENTH sizeof(lvgl_set_typedef) /* ���鳤�� */

/*SIZE��ʾ���ֳ�(2�ֽ�), ��С������2��������, ������ǵĻ�, ǿ�ƶ��뵽2�������� */
#define EEP_SIZE      EEP_LENTH / 2 + ((EEP_LENTH % 2) ? 1 : 0)

#define FLASH_SAVE_ADDR 0X08078000 /* ����FLASH �����ַ(����Ϊż��������ֵҪ���ڱ�������ռ��FLASH�Ĵ�С + 0X08000000) */



extern lvgl_set_typedef  my_lvgl_cont;
extern volatile EBIKE_TYPEDEF  ebike_lvgl; 

void lv_setleg(void);
void lv_mainstart(void);
lvgl_set_typedef lvgl_getcontrol(void);
#endif // __MY_GUI_H
