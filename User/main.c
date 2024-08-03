/**
 ****************************************************************************************************
 * @file        spi.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       SPI ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
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
    
    HAL_Init();                                 /* ��ʼ��HAL�� */
    sys_stm32_clock_init(RCC_PLL_MUL9);         /* ����ʱ��,72M */
    delay_init(72);                             /* ��ʼ����ʱ���� */
    delay_ms(500);                              /* �����д�����һ��׼��ʱ�� */
    
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

