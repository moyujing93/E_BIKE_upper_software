/**
****************************************************************************************************
* @file        exti.c
* @author      正点原子团队(ALIENTEK)
* @version     V1.0
* @date        2020-04-19
* @brief       无线通讯task任务
* @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
****************************************************************************************************
*/

#include  "task_24g.h"
#include "./BSP/NRF24L01.h"

/*************参数定义 **************/

TaskHandle_t    task_24g_Handler;
QueueHandle_t   g_xQueue_24g;

/************* 函数 **************/
/**
 * @brief       task函数
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
