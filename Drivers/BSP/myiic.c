/**
 ****************************************************************************************************
 * @file        spi.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       SPI ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 */

#include "./BSP/myiic.h"
#include "./SYSTEM/delay/delay.h"
/******************************************************************************************/
/*  ���� ����  */
#define SDA_GPIO_PORT             GPIOB
#define SDA_GPIO_PIN              GPIO_PIN_8
#define SDA_Set(x)                 do{x?    \
                                            (SDA_GPIO_PORT->BSRR |= SDA_GPIO_PIN):     \
                                            (SDA_GPIO_PORT->BSRR |= (uint32_t)SDA_GPIO_PIN << 16U);}while(0)
#define SDA_Read()                HAL_GPIO_ReadPin(SDA_GPIO_PORT,SDA_GPIO_PIN)

#define CLK_GPIO_PORT             GPIOB
#define CLK_GPIO_PIN              GPIO_PIN_9
#define CLK_Set(x)                 do{x?    \
                                            (CLK_GPIO_PORT->BSRR |= CLK_GPIO_PIN):     \
                                            (CLK_GPIO_PORT->BSRR |= (uint32_t)CLK_GPIO_PIN << 16U);   }while(0)

/******************************************************************************************/

/**
 * @brief       iicʱ��
 * @param       
 * @param       
 * @retval      
 */
static void iic_delay(void)
{
    delay_us(2);        /*                ���Ʋ�����500kHZ.                  */
}

void iic_start(void)
{
    /*    _____
     *SDA      \_____________
     *    __________
     *SCL           \________
     */
    iic_delay();
    CLK_Set(0);
    SDA_Set(1);
    iic_delay();
    CLK_Set(1);
    iic_delay();
    SDA_Set(0);
}

void iic_stop(void)
{
    /*               _______
     *SDA __________/
     *          ____________
     *SCL _____/
     */
    iic_delay();
    CLK_Set(0);
    SDA_Set(0);
    iic_delay();
    CLK_Set(1);
    iic_delay();
    SDA_Set(1);
}

uint8_t iic_waitack(void)
{
    iic_delay();
    CLK_Set(0);
    SDA_Set(1);
    iic_delay();
    CLK_Set(1);
    iic_delay();
    if(SDA_Read())
    {
        iic_stop();
        return 0;
    }
    return 1;
}

void send_ack(void)
{
    /*           ____
     *SCL ______/    \______
     *    ____         _____
     *SDA     \_______/
     */
    iic_delay();
    CLK_Set(0);
    SDA_Set(0);
    iic_delay();
    CLK_Set(1);
    iic_delay();
    CLK_Set(0);
    SDA_Set(1);     /* �ͷ�SDA���� */
}

void send_noack(void)
{
    /*           ____
     *SCL ______/    \______
     *    __________________
     *SDA
     */
    iic_delay();
    CLK_Set(0);
    SDA_Set(1);
    iic_delay();
    CLK_Set(1);
}

void iic_send(uint8_t data)
{
    for(uint8_t i = 0;i < 8;i++)
    {
        iic_delay();
        CLK_Set(0);
        if(data & 0x80) /*�ȷ���MSB*/
        {
            SDA_Set(1);
        }else
        {
            SDA_Set(0);
        }
        iic_delay();
        CLK_Set(1);
        data <<= 1;
    }
    iic_delay();
    CLK_Set(0);
    SDA_Set(1);     /* �ͷ�SDA���� */
    
//    iic_delay();            /*���������Ӧ���źţ�������һ��ʱ������*/
//    CLK_Set(0);            /*�����Ҫ����Ӧ���źţ���ע�͵�*/
//    iic_delay();
//    CLK_Set(1);
}

uint8_t iic_read(uint8_t ack)
{
    uint8_t data;
    iic_delay();
    CLK_Set(0);
    SDA_Set(1);     /* �ͷ�SDA���� */
    for(uint8_t i = 0;i < 8;i++)
    {
        data <<= 1;
        CLK_Set(0);
        iic_delay();
        CLK_Set(1);
        iic_delay();
        if(SDA_Read())     /*��ƽΪ��*/
        {
            data++;
        }
    }
    if(ack)
    {
        send_ack();
    }else
    {
        send_noack();
    }
    return data;
}
/**
 * @brief       ��ʼ��gpio
 * @param       
 * @param       
 * @retval      
 */
void iic_gpioinit(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef   gpio_init_hander;
    gpio_init_hander.Mode  =  GPIO_MODE_OUTPUT_OD;
    gpio_init_hander.Pin   =  SDA_GPIO_PIN  |  CLK_GPIO_PIN;
    gpio_init_hander.Pull  =  GPIO_NOPULL;
    gpio_init_hander.Speed =  GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB,&gpio_init_hander);
    iic_stop();
}
