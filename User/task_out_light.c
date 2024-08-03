/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       FreeRTOS �����ź�������ʵ��
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
 *
 ****************************************************************************************************
 */
#include  "task_out_light.h"

/*************�������� **************/

TaskHandle_t  task_out_light_Handler;

/* ���� ���� */

#define LIGHT_GPIO_PORT                  GPIOC
#define LIGHT_GPIO_PIN                   GPIO_PIN_3
#define LIGHT_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)


/************* ���� **************/

/**
 * @brief       ��ʼ������
 * @param       ��
 * @retval      ��
 */
static void light_key_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    /* ��ʱ�� */
    LIGHT_GPIO_CLK_ENABLE();
    /* ��ʼ��GPIO */
    gpio_init_struct.Pin = LIGHT_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LIGHT_GPIO_PORT, &gpio_init_struct);
}
/**
 * @brief       �ƹ�����
 * @param       ��
 * @retval      ��
 */
void task_out_light(void *pvParameters)
{
    light_key_init();
    while(1)
    {
        /* �ȴ����ݴ������ */
        xEventGroupWaitBits(g_EventGroup,( Ev_LIG_ON | Ev_LIG_OFF ),pdFALSE,pdFALSE,portMAX_DELAY);
        
        if( xEventGroupGetBits(g_EventGroup) & Ev_LIG_ON)
        {
            HAL_GPIO_WritePin(LIGHT_GPIO_PORT,LIGHT_GPIO_PIN,GPIO_PIN_SET);
            xEventGroupClearBits(g_EventGroup,Ev_LIG_ON);
        }else
        {
            HAL_GPIO_WritePin(LIGHT_GPIO_PORT,LIGHT_GPIO_PIN,GPIO_PIN_RESET);
            xEventGroupClearBits(g_EventGroup,Ev_LIG_OFF);
        }
    }
}
