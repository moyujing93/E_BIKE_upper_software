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
#include  "task_adc.h"

/*************�������� **************/
TaskHandle_t            task_adc_Handler;   /* ����ADC��� */
QueueHandle_t      g_xQueue_adc;            /* ����adc�ɼ� */


/************* ���� **************/
void task_adc(void *pvParameters)
{
    uint32_t lastWakeTime = xTaskGetTickCount();
    GET_ADC_typedef ebike_getadc;
    while(1)
    {
        adc_dma_conver(&ebike_getadc);
//        xSemaphoreTake(MutexSemaphore_adc, portMAX_DELAY);  /* ��ȡ�����ź��� */
        xQueueSend(g_xQueue_adc,&ebike_getadc, portMAX_DELAY );
//        xSemaphoreGive(MutexSemaphore_adc);                 /* �ͷŻ����ź��� */
        
        vTaskDelayUntil(&lastWakeTime,(configTICK_RATE_HZ/50));
    }
}
