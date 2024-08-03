/**
 ****************************************************************************************************
 * @file        rs485.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       RS485 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200424
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include <string.h>
#include "./BSP/rs485.h"
#include "freertos_demo.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"

/******************************************************************************************/

uint8_t  receive_data[USART_REC_LEN];
/******************************************************************************************/

/**
 * @brief       RS485初始化函数
 *   @note      该函数主要是初始化串口
 * @param       baudrate: 波特率, 根据自己需要设置波特率值
 * @retval      无
 */
void rs485_init(uint32_t baudrate)
{
    GPIO_InitTypeDef gpio_initure;
    
    /* IO 及 时钟配置 */
    RS485_RE_GPIO_CLK_ENABLE();
    gpio_initure.Pin = RS485_RE_GPIO_PIN;
    gpio_initure.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_initure.Pull = GPIO_PULLUP;
    gpio_initure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(RS485_RE_GPIO_PORT, &gpio_initure);
    /* 默认为接收模式 */
    RS485_RE(0);
    /* 调用usart初始化函数 */
    usart_init(baudrate);
}


/**
 * @brief       CRC检验
 * @param       发送区首地址
 * @param       发送的字节数
 * @retval      无
 */
unsigned short ModbusCRC16_Check(uint8_t *array, int BitNumber)
{
    unsigned short tmp = 0xffff;
    
    int n = 0, i = 0;
    for (n = 0; n < (BitNumber - 2); n++) {/*此处的6 -- 要校验的位数为6个*/
        tmp = array[n] ^ tmp;
        for (i = 0; i < 8; i++) {  /*此处的8 -- 指每一个char类型又8bit，每bit都要处理*/
            if (tmp & 0x01) {
                tmp = tmp >> 1;
                tmp = tmp ^ 0xa001;
            } else {
                tmp = tmp >> 1;
            }
        }
    }
    return tmp;
}

/**
 * @brief       RS485发送len个字节
 * @param       buf     : 发送区首地址
 * @param       len     : 发送的字节数(最大200字节)
 * @retval      无
 */
void rs485_send_data(uint8_t *buf, uint8_t len)
{
    
    taskENTER_CRITICAL();           /* 进入临界区 */
    RS485_RE(1);                                        /* 进入发送模式 */
    usart_send_data(buf,len);
    usart_send_data((uint8_t*)"\r\n",sizeof"\r\n");
    RS485_RE(0);                                        /* 进入接收模式 */
    taskEXIT_CRITICAL();            /* 退出临界区 */
}

/**
 * @brief       接收到的数据
 * @param       buf     : 接收缓冲区首地址
 * @retval      返回接受到的字节数,为0则是接受数据失败
 */
uint8_t rs485_receive_data(uint8_t *buf, uint16_t buf_size)
{
    uint8_t sta = 0;
    sta = usart_receive_data(buf,buf_size);
    return sta;

}
