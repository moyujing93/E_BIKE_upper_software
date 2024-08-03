/**
 ****************************************************************************************************
 * @file        spi.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       SPI 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 */

#include "./BSP/myiic.h"
#include "./SYSTEM/delay/delay.h"
/******************************************************************************************/
/*  引脚 定义  */
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
 * @brief       iic时序
 * @param       
 * @param       
 * @retval      
 */
static void iic_delay(void)
{
    delay_us(2);        /*                控制不超过500kHZ.                  */
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
    SDA_Set(1);     /* 释放SDA总线 */
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
        if(data & 0x80) /*先发送MSB*/
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
    SDA_Set(1);     /* 释放SDA总线 */
    
//    iic_delay();            /*如果不处理应答信号，就增加一个时钟周期*/
//    CLK_Set(0);            /*如果需要处理应答信号，就注释掉*/
//    iic_delay();
//    CLK_Set(1);
}

uint8_t iic_read(uint8_t ack)
{
    uint8_t data;
    iic_delay();
    CLK_Set(0);
    SDA_Set(1);     /* 释放SDA总线 */
    for(uint8_t i = 0;i < 8;i++)
    {
        data <<= 1;
        CLK_Set(0);
        iic_delay();
        CLK_Set(1);
        iic_delay();
        if(SDA_Read())     /*电平为高*/
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
 * @brief       初始化gpio
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
