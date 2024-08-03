/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       FreeRTOS 互斥信号量操作实验
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
 *
 ****************************************************************************************************
 */
#include  "task_out_light.h"

/*************参数定义 **************/

TaskHandle_t  task_out_light_Handler;

/* 引脚 定义 */

#define LIGHT_GPIO_PORT                  GPIOC
#define LIGHT_GPIO_PIN                   GPIO_PIN_3
#define LIGHT_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)


/************* 函数 **************/

/**
 * @brief       初始化函数
 * @param       无
 * @retval      无
 */
static void light_key_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    /* 打开时钟 */
    LIGHT_GPIO_CLK_ENABLE();
    /* 初始化GPIO */
    gpio_init_struct.Pin = LIGHT_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LIGHT_GPIO_PORT, &gpio_init_struct);
}
/**
 * @brief       灯光任务
 * @param       无
 * @retval      无
 */
void task_out_light(void *pvParameters)
{
    light_key_init();
    while(1)
    {
        /* 等待数据处理完成 */
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
