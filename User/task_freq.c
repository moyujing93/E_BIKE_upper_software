/**
****************************************************************************************************
* @file        exti.c
* @author      正点原子团队(ALIENTEK)
* @version     V1.0
* @date        2020-04-19
* @brief       外部中断 驱动代码
* @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
****************************************************************************************************
* 现在没法解决exti10-15公用一个中断IRQ，如何在EXTI15_10_IRQHandler获取到对应的中断源？？？
*/

#include  "task_freq.h"
#include "./SYSTEM/delay/delay.h"

/*************参数定义 **************/

volatile freq_typedef   freq1 = {0};
volatile speed_typedef   speed1 = {0};
TaskHandle_t    task_freq_Handler;
QueueHandle_t   g_xQueue_freq;
//static QueueHandle_t  Semaphore_freq;


volatile EBIKE_TYPEDEF  ebike_fq = {0};

/************* 函数 **************/
/**
 * @brief       EXTI初始化
 * @param       
 * @retval      
 */
void T_EXTI_init(void)
{
    /* 创建用于中断任务同步的信号量 */
//    Semaphore_freq = xSemaphoreCreateBinary();
    /* 打开gpio */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef exti_hander;
    exti_hander.Mode = GPIO_MODE_IT_FALLING;
    exti_hander.Pin = KEY0_INT_GPIO_PIN;
    exti_hander.Pull = GPIO_PULLUP;
    exti_hander.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(KEY0_INT_GPIO_PORT,&exti_hander);
    exti_hander.Mode = GPIO_MODE_INPUT;
    exti_hander.Pin = KEY1_INT_GPIO_PIN;
    HAL_GPIO_Init(KEY1_INT_GPIO_PORT,&exti_hander);
    
    /* 速度传感器的gpio */
    exti_hander.Mode = GPIO_MODE_IT_FALLING;
    exti_hander.Pin = speed_INT_GPIO_PIN;
    exti_hander.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(speed_INT_GPIO_PORT,&exti_hander);
    
    /* 设置踏频传感器的NVIC */
    HAL_NVIC_SetPriority(KEY0_INT_IRQn,4,0);   //让EXTI被FreeRTOS管理，好在中断函数中写队列
    HAL_NVIC_EnableIRQ(KEY0_INT_IRQn);
    /* 设置速度传感器的NVIC */
    HAL_NVIC_SetPriority(speed_INT_IRQn,4,0);   //让EXTI被FreeRTOS管理，好在中断函数中写队列
    HAL_NVIC_EnableIRQ(speed_INT_IRQn);
}
/**
 * @brief       中断回调函数
 * @param       
 * @retval      
 */
void EXTI1_IRQHandler(void)
{
    /* EXTI line interrupt detected */
    if (__HAL_GPIO_EXTI_GET_IT(KEY0_INT_GPIO_PIN) != 0x00u)
    {
        uint32_t f_time = HAL_GetTick() - freq1.last_time;
        if(f_time > 40)   /* 按键有很多杂波，这里时过滤杂波频繁进中断 */
        {
            freq1.sta = 0;
            if(HAL_GPIO_ReadPin(KEY1_INT_GPIO_PORT,KEY1_INT_GPIO_PIN) != GPIO_PIN_SET)
            {
                freq1.dir = 0;
            }else
            {
                freq1.dir = 1;
            }
            
            freq1.get_time = f_time;
            freq1.last_time += f_time;
            freq1.sta = 1;
//            xSemaphoreGiveFromISR(Semaphore_freq,NULL);
        }
        __HAL_GPIO_EXTI_CLEAR_IT(KEY0_INT_GPIO_PIN);
    }
}
/**
 * @brief       速度传感器中断回调函数
 * @param       
 * @retval      
 */
void EXTI0_IRQHandler(void)
{
    /* EXTI line interrupt detected */
    if (__HAL_GPIO_EXTI_GET_IT(speed_INT_GPIO_PIN) != 0x00u)
    {
        uint32_t s_time = HAL_GetTick() - speed1.last_time;
        if(s_time > 140)   /* 按键有很多杂波，这里时过滤杂波频繁进中断 */
        {
            speed1.sta = 0;
           /*   让里程累加    */
            speed1.get_num++;
           if(speed1.get_num >= (100 * SPEED_MAGNET))
           {
               ebike_fq.mileage += (uint32_t)(ebike_fq.size * 0.1f);
               speed1.get_num = 0;
           }
           /*   计算速度用的    */
            speed1.get_time = s_time;
            speed1.last_time += s_time;
            speed1.sta = 1;
        }
        __HAL_GPIO_EXTI_CLEAR_IT(speed_INT_GPIO_PIN);
    }
}
/**
 * @brief       task函数
 * @param       
 * @retval      
 */
void task_freq(void *pvParameters)
{
    
    uint32_t lastWakeTime = xTaskGetTickCount();
    static uint32_t freq_get_time = 0; 
    static uint32_t speed_get_time = 0; 
    GET_speed_typedef get_sp_freq = {0};
            /*   放入全局变量中   */
    xSemaphoreTake(MutexSemaphore_my_ebike,portMAX_DELAY);   // 这里只需要获取一次
    ebike_fq.size = my_ebike.size;
    ebike_fq.mileage = my_ebike.mileage;
    xSemaphoreGive(MutexSemaphore_my_ebike);
    
    while(1)
    {
        
        /* 等待中断释放二值信号量 */
//        xSemaphoreTake(Semaphore_freq, 250);
        /* 返回速度值，上面等待踏频设置等待250ms */
        
        
        /* 计算踏频 */
        
        freq_typedef  freq_tep = freq1;    // 易变变量可能在意想不到的时候变化，所以复制此刻状态
        if(freq_tep.sta != 0)
        {
            if(freq_tep.dir == 0)
            {
                get_sp_freq.rpm = 0;
            }else
            {
                get_sp_freq.rpm = (60000 / RPM_MAGNET) / freq_tep.get_time;           /* rpm/min = 60000 / (N * time) */
            }
            /* 易变变量状态置0 */
            freq1.sta = 0;
            freq_get_time = freq_tep.last_time;
        }else if( (HAL_GetTick() - freq_get_time) > 250)
        {
            get_sp_freq.rpm = 0;
        }
        
        
        /* 计算速度 */
        
        speed_typedef speed_temp = speed1; /* 复制变量，不直接使用易变变量 */
        if(speed_temp.sta != 0)
        {
            get_sp_freq.speed = ((float)(ebike_fq.size / SPEED_MAGNET) / speed_temp.get_time) * 3.6f ;
            speed1.sta = 0;
            speed_get_time = speed1.last_time;
        }else if((HAL_GetTick() - speed_get_time) > 1500)
        {
            get_sp_freq.speed = 0;
        }
        
        xQueueSend(g_xQueue_freq,&get_sp_freq, portMAX_DELAY );
        
        
                /*   放入全局变量中   */
        xSemaphoreTake(MutexSemaphore_my_ebike,portMAX_DELAY);
        my_ebike.mileage = ebike_fq.mileage;
        xSemaphoreGive(MutexSemaphore_my_ebike);
        vTaskDelayUntil(&lastWakeTime,(configTICK_RATE_HZ/50));
    }
}
