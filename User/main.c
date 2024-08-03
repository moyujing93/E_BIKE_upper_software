/**
 ****************************************************************************************************
 * @file        spi.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       SPI 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 *
 *
 *
 *
 */
 
 
 
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "freertos_demo.h"
#include "lvgl.h"
#include "my_gui.h"
#include  "task_freq.h"
#include "./BSP/st7789.h"
#include "./BSP/adc.h"
#include "./BSP/rs485.h"
#include "./BSP/pwr.h"



int main(void)
{
    
    HAL_Init();                                 /* 初始化HAL库 */
    sys_stm32_clock_init(RCC_PLL_MUL9);         /* 设置时钟,72M */
    delay_init(72);                             /* 初始化延时函数 */
    delay_ms(500);                              /* 给所有传感器一个准备时间 */
    
    rs485_init(115200);
    adc1_dma_init();
    T_EXTI_init();
//    my_pwrinit();
    
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();
    lv_mainstart();
    
    freertos_demo();
    
    
    
    while(1)
    {
        
    }
}

