/**
 ****************************************************************************************************
 * @file        rs485.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       RS485 ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20200424
 * ��һ�η���
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
 * @brief       RS485��ʼ������
 *   @note      �ú�����Ҫ�ǳ�ʼ������
 * @param       baudrate: ������, �����Լ���Ҫ���ò�����ֵ
 * @retval      ��
 */
void rs485_init(uint32_t baudrate)
{
    GPIO_InitTypeDef gpio_initure;
    
    /* IO �� ʱ������ */
    RS485_RE_GPIO_CLK_ENABLE();
    gpio_initure.Pin = RS485_RE_GPIO_PIN;
    gpio_initure.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_initure.Pull = GPIO_PULLUP;
    gpio_initure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(RS485_RE_GPIO_PORT, &gpio_initure);
    /* Ĭ��Ϊ����ģʽ */
    RS485_RE(0);
    /* ����usart��ʼ������ */
    usart_init(baudrate);
}


/**
 * @brief       CRC����
 * @param       �������׵�ַ
 * @param       ���͵��ֽ���
 * @retval      ��
 */
unsigned short ModbusCRC16_Check(uint8_t *array, int BitNumber)
{
    unsigned short tmp = 0xffff;
    
    int n = 0, i = 0;
    for (n = 0; n < (BitNumber - 2); n++) {/*�˴���6 -- ҪУ���λ��Ϊ6��*/
        tmp = array[n] ^ tmp;
        for (i = 0; i < 8; i++) {  /*�˴���8 -- ָÿһ��char������8bit��ÿbit��Ҫ����*/
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
 * @brief       RS485����len���ֽ�
 * @param       buf     : �������׵�ַ
 * @param       len     : ���͵��ֽ���(���200�ֽ�)
 * @retval      ��
 */
void rs485_send_data(uint8_t *buf, uint8_t len)
{
    
    taskENTER_CRITICAL();           /* �����ٽ��� */
    RS485_RE(1);                                        /* ���뷢��ģʽ */
    usart_send_data(buf,len);
    usart_send_data((uint8_t*)"\r\n",sizeof"\r\n");
    RS485_RE(0);                                        /* �������ģʽ */
    taskEXIT_CRITICAL();            /* �˳��ٽ��� */
}

/**
 * @brief       ���յ�������
 * @param       buf     : ���ջ������׵�ַ
 * @retval      ���ؽ��ܵ����ֽ���,Ϊ0���ǽ�������ʧ��
 */
uint8_t rs485_receive_data(uint8_t *buf, uint16_t buf_size)
{
    uint8_t sta = 0;
    sta = usart_receive_data(buf,buf_size);
    return sta;

}
