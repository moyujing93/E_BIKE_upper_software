/**
 ****************************************************************************************************
 * @file        key.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-20
 * @brief       �������� ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20200420
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "./BSP/key.h"
#include "./SYSTEM/delay/delay.h"


static key_struct key_bike = {0};


/**
 * @brief       ������ʼ������
 * @param       ��
 * @retval      ��
 */
void key_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    __HAL_RCC_GPIOB_CLK_ENABLE();
    gpio_init_struct.Pin = KEY1_GPIO_PIN | KEY2_GPIO_PIN | KEY3_GPIO_PIN;     /* KEY1���� */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;                    /* ���� */
    gpio_init_struct.Pull = GPIO_PULLUP;                        /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;              /* ���� */
    HAL_GPIO_Init(GPIOB, &gpio_init_struct);           /* KEY1����ģʽ����,�������� */

}

/**
 * @brief       ����ɨ�躯��
 * @note        �ú�������Ӧ���ȼ�(ͬʱ���¶������): WK_UP > KEY1 > KEY1!!
 * @param       mode:0 / 1, ���庬������:
 *   @arg       0,  ��֧��������(���������²���ʱ, ֻ�е�һ�ε��û᷵�ؼ�ֵ,
 *                  �����ɿ��Ժ�, �ٴΰ��²Ż᷵��������ֵ)
 *   @arg       1,  ֧��������(���������²���ʱ, ÿ�ε��øú������᷵�ؼ�ֵ)
 * @retval      ��ֵ, ��������:
 *              KEY1_PRES, 2, KEY1����
 */
uint8_t key_scan(uint8_t mode)
{
    uint8_t keyval = 0;
    uint8_t keyrel = 0;
    
    if(KEY1 == 0 || KEY2 == 0 || KEY3 == 0)  /* �����ɿ���־Ϊ1, ��������һ������������ */
    {
        key_bike.key_sta = 1;
        delay_ms(5);
        
        if (KEY1 == 0)  key_bike.keyval_temp = 1;
        else if (KEY2 == 0)  key_bike.keyval_temp = 2;
        else if (KEY3 == 0)  key_bike.keyval_temp = 3;
        /* �жϳ��� */
        if( key_bike.keyval_temp == 2 && key_bike.key_longsta == 0 )
        {
            uint32_t i = HAL_GetTick() - key_bike.long_time;
            if(i > 500)           /* lvgl���Ǽ��30ms���һ�Σ�35ms*15=525ms*/
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
            if(keyrel) /* ����ͳ�����ͻ�����������ɿ�����Ч */
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
    return keyval;              /* ���ؼ�ֵ */
}

