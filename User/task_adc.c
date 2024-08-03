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
#include  "task_adc.h"

/*************参数定义 **************/
TaskHandle_t            task_adc_Handler;   /* 任务ADC句柄 */
QueueHandle_t      g_xQueue_adc;            /* 队列adc采集 */


/************* 函数 **************/
void task_adc(void *pvParameters)
{
    uint32_t lastWakeTime = xTaskGetTickCount();
    GET_ADC_typedef ebike_getadc;
    while(1)
    {
        adc_dma_conver(&ebike_getadc);
//        xSemaphoreTake(MutexSemaphore_adc, portMAX_DELAY);  /* 获取互斥信号量 */
        xQueueSend(g_xQueue_adc,&ebike_getadc, portMAX_DELAY );
//        xSemaphoreGive(MutexSemaphore_adc);                 /* 释放互斥信号量 */
        
        vTaskDelayUntil(&lastWakeTime,(configTICK_RATE_HZ/50));
    }
}
