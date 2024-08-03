/**
****************************************************************************************************
* @file        exti.c
* @author      ����ԭ���Ŷ�(ALIENTEK)
* @version     V1.0
* @date        2020-04-19
* @brief       ����ͨѶtask����
* @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
****************************************************************************************************
*/

#include  "task_24g.h"
#include "./BSP/NRF24L01.h"

/*************�������� **************/

TaskHandle_t    task_24g_Handler;
QueueHandle_t   g_xQueue_24g;

/************* ���� **************/
/**
 * @brief       task����
 * @param       
 * @retval      
 */
void task_24g(void *pvParameters)
{
    uint8_t  data[32];
    while(1)
    {
        if(NRF_get_byte(data) != 0)
        {
            xQueueSend(g_xQueue_24g,&data,portMAX_DELAY);
        }
        vTaskDelay(200);
    }
}
