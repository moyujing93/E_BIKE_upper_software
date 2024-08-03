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

#ifndef __MY_GUI_H
#define __MY_GUI_H


#include "lvgl.h"
#include "freertos_demo.h"


/*LVGL控制结构体*/
typedef struct
{
    uint16_t  sta;                 /* 数据可用标志，改变完数据置1，其他任务中判断sta = 1才复制信息 */
    uint16_t  eeprom_sta;          /* 有没有写入到EEPROM的标志，如果值=12345就是写入过 */
    uint16_t  pwm_sta;             /* 是否使用转速控制 */
    uint16_t  auto_light_sta;      /* 自动大灯开关 */
    uint16_t  light_Threshold;     /* 自动大灯灵敏度 */
    uint16_t  auto_cun;            /* mpu6050动力介入灵敏度百分比 */
    uint16_t  e_maxcun;            /* e_bike模式下最大电流百分比 */
    uint16_t  m_maxcun;            /* motor模式下最大电流百分比 */
    uint16_t  sport_num;           /* 当前档位的电流百分比 */
    uint16_t  mode;                /* 模式 */
    uint32_t  mileage;             /* 总里程km */
    uint16_t  size;                /* 车轮的周长 mm*/
}lvgl_set_typedef;


/*  FLASH模拟EEPROM */
#define EEP_LENTH sizeof(lvgl_set_typedef) /* 数组长度 */

/*SIZE表示半字长(2字节), 大小必须是2的整数倍, 如果不是的话, 强制对齐到2的整数倍 */
#define EEP_SIZE      EEP_LENTH / 2 + ((EEP_LENTH % 2) ? 1 : 0)

#define FLASH_SAVE_ADDR 0X08078000 /* 设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小 + 0X08000000) */



extern lvgl_set_typedef  my_lvgl_cont;
extern volatile EBIKE_TYPEDEF  ebike_lvgl; 

void lv_setleg(void);
void lv_mainstart(void);
lvgl_set_typedef lvgl_getcontrol(void);
#endif // __MY_GUI_H
