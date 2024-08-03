/**
 ****************************************************************************************************
 * @file        key.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-20
 * @brief       按键输入 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200420
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./BSP/key.h"
#include "./SYSTEM/delay/delay.h"


static key_struct key_bike = {0};


/**
 * @brief       按键初始化函数
 * @param       无
 * @retval      无
 */
void key_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    __HAL_RCC_GPIOB_CLK_ENABLE();
    gpio_init_struct.Pin = KEY1_GPIO_PIN | KEY2_GPIO_PIN | KEY3_GPIO_PIN;     /* KEY1引脚 */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;                    /* 输入 */
    gpio_init_struct.Pull = GPIO_PULLUP;                        /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;              /* 高速 */
    HAL_GPIO_Init(GPIOB, &gpio_init_struct);           /* KEY1引脚模式设置,上拉输入 */

}

/**
 * @brief       按键扫描函数
 * @note        该函数有响应优先级(同时按下多个按键): WK_UP > KEY1 > KEY1!!
 * @param       mode:0 / 1, 具体含义如下:
 *   @arg       0,  不支持连续按(当按键按下不放时, 只有第一次调用会返回键值,
 *                  必须松开以后, 再次按下才会返回其他键值)
 *   @arg       1,  支持连续按(当按键按下不放时, 每次调用该函数都会返回键值)
 * @retval      键值, 定义如下:
 *              KEY1_PRES, 2, KEY1按下
 */
uint8_t key_scan(uint8_t mode)
{
    uint8_t keyval = 0;
    uint8_t keyrel = 0;
    
    if(KEY1 == 0 || KEY2 == 0 || KEY3 == 0)  /* 按键松开标志为1, 且有任意一个按键按下了 */
    {
        key_bike.key_sta = 1;
        delay_ms(5);
        
        if (KEY1 == 0)  key_bike.keyval_temp = 1;
        else if (KEY2 == 0)  key_bike.keyval_temp = 2;
        else if (KEY3 == 0)  key_bike.keyval_temp = 3;
        /* 判断长按 */
        if( key_bike.keyval_temp == 2 && key_bike.key_longsta == 0 )
        {
            uint32_t i = HAL_GetTick() - key_bike.long_time;
            if(i > 500)           /* lvgl中是间隔30ms检测一次，35ms*15=525ms*/
            {
                key_bike.keyval_temp = 4;
                key_bike.key_longsta = 1;
            }
        }
        
    }else
    {
        key_bike.long_time = HAL_GetTick();
        keyrel = 1;
    }
    
    if( key_bike.key_sta )
    {
        if(key_bike.keyval_temp == 2)
        {
            if(keyrel) /* 避免和长按冲突，按键必需松开才生效 */
            {
                if(key_bike.key_longsta == 0)
                {
                    keyval = 2;
                }else
                {
                    key_bike.key_longsta = 0;
                }
                key_bike.key_sta = 0;
            }
        }else
        {
            keyval = key_bike.keyval_temp;
            key_bike.key_sta = 0;
        }
        
    }
    return keyval;              /* 返回键值 */
}

