/**
 ****************************************************************************************************
 * @file        rs485.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       RS485 ��������
 * @license     Copyright (c) 2020-2032, �������������ӿƼ����޹�˾
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

#ifndef __RS485_H
#define __RS485_H

#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/

/* RS485 ���� �ο�modbus������ʽ�����ݳ��ȹ̶������ֽ�*/
typedef struct
{
    uint8_t      slave;        /* �ӻ���ַ */
    uint8_t      command;      /* ��д���� */
    uint16_t      w_rpm;       /* дת�� */
    uint16_t      w_current;   /* д���� */
    uint16_t      r_rpm;       /* ��ת�� */
    uint16_t      r_current;   /* ������ */
    uint16_t      r_vbus;      /* ����ѹ */
    uint16_t      r_temper;    /* ���¶� */
    uint16_t      crc;         /* crcУ��λ */
}rs485_typedef;


/* RS485 ���� �� ���� ���� */
#define RS485_RE_GPIO_PORT                  GPIOB
#define RS485_RE_GPIO_PIN                   GPIO_PIN_5
#define RS485_RE_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PD��ʱ��ʹ�� */

/* RS485 ���ӻ��ĵ�ַ  */
#define   slave_bldc                 0xf1
/* RS485 ���� ���� */
#define command_read                 0xf3
#define command_write                0xf6
         
/******************************************************************************************/

/* ����RS485_RE��, ����RS485����/����״̬
 * RS485_RE = 0, �������ģʽ
 * RS485_RE = 1, ���뷢��ģʽ
 */
#define RS485_RE(x)   do{ x ? \
                          HAL_GPIO_WritePin(RS485_RE_GPIO_PORT, RS485_RE_GPIO_PIN, GPIO_PIN_SET) : \
                          HAL_GPIO_WritePin(RS485_RE_GPIO_PORT, RS485_RE_GPIO_PIN, GPIO_PIN_RESET); \
                      }while(0)




void rs485_init( uint32_t baudrate);  /* RS485��ʼ�� */

unsigned short ModbusCRC16_Check(uint8_t *array, int BitNumber);

void rs485_send_data(uint8_t *buf, uint8_t len);

uint8_t rs485_receive_data(uint8_t *buf, uint16_t buf_size);

#endif















