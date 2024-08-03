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

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/st7789.h"
#include "./BSP/front.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

/******************************************************************************************/
/* LCD ���� ���� */
#define LCD_SDA_GPIO_PORT             GPIOA
#define LCD_SDA_GPIO_PIN              GPIO_PIN_7
#define LCD_SDA_GPIO_CLK_ENABLE()     do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)
#define LCD_SDA_Set(x)                 do{x?    \
                                            (GPIOA->BSRR |= LCD_SDA_GPIO_PIN):     \
                                            (GPIOA->BSRR |= (uint32_t)LCD_SDA_GPIO_PIN << 16U);     \
                                         }while(0)

#define LCD_CLK_GPIO_PORT              GPIOA
#define LCD_CLK_GPIO_PIN               GPIO_PIN_5
#define LCD_CLK_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)
#define LCD_CLK_Set(x)                 do{x ?    \
                                            (GPIOA->BSRR |= LCD_CLK_GPIO_PIN):     \
                                            (GPIOA->BSRR |= (uint32_t)LCD_CLK_GPIO_PIN << 16U);     \
                                         }while(0)

#define LCD_DC_GPIO_PORT             GPIOA
#define LCD_DC_GPIO_PIN              GPIO_PIN_9
#define LCD_DC_GPIO_CLK_ENABLE()     do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)
#define LCD_DC_Set(x)                 do{x?    \
                                            (GPIOA->BSRR |= LCD_DC_GPIO_PIN):     \
                                            (GPIOA->BSRR |= (uint32_t)LCD_DC_GPIO_PIN << 16U);     \
                                         }while(0)


#define LCD_CS_GPIO_PORT             GPIOA
#define LCD_CS_GPIO_PIN              GPIO_PIN_10
#define LCD_CS_GPIO_CLK_ENABLE()     do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)
#define LCD_CS_Set(x)                 do{x?    \
                                            (GPIOA->BSRR |= LCD_CS_GPIO_PIN):     \
                                            (GPIOA->BSRR |= (uint32_t)LCD_CS_GPIO_PIN << 16U);     \
                                         }while(0)


#define LCD_RES_GPIO_PORT             GPIOA
#define LCD_RES_GPIO_PIN              GPIO_PIN_8
#define LCD_RES_GPIO_CLK_ENABLE()     do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)
#define LCD_RES_Set(x)                 do{x?    (GPIOA->BSRR |= LCD_RES_GPIO_PIN):   \
                                                (GPIOA->BSRR |= (uint32_t)LCD_RES_GPIO_PIN << 16U);    \
                                           }while(0)


#define LCD_BL_GPIO_PORT             GPIOA
#define LCD_BL_GPIO_PIN              GPIO_PIN_2
#define LCD_BL_GPIO_CLK_ENABLE()     do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)
#define LCD_BL_Set(x)                 do{x?    (GPIOA->BSRR |= LCD_BL_GPIO_PIN):   \
                                                (GPIOA->BSRR |= (uint32_t)LCD_BL_GPIO_PIN << 16U);    \
                                           }while(0)
/******************************************************************************************/
/* ���� �ṹ�� ���� */
#define  bufer_x    240
#define  bufer_y    5
                                           
SPI_HandleTypeDef g_spi_lcd_hander = {0};
DMA_HandleTypeDef g_dma_spi_handle = {0};
_lcd_dev lcddev;

uint8_t lcd_gram_data[bufer_x * bufer_y * 2];  // G_RAM������
uint8_t lcd_gram_data2[bufer_x * bufer_y * 2];  // G_RAM������2
volatile uint8_t  dma_sta = 1;


/******************************************************************************************/

#define USE_HSPIDMA         1
// �Ƿ�ʹ��Ӳ��SPI+DMA
#if USE_HSPIDMA == 1

void dma_spi_init(void)
{
    __HAL_RCC_DMA1_CLK_ENABLE();
    /* ��ʼ��DMA */
    g_dma_spi_handle.Instance = DMA1_Channel3;                              /* ����DMAͨ�� */
    g_dma_spi_handle.Init.Direction = DMA_MEMORY_TO_PERIPH;                 /* �����赽�洢��ģʽ */
    g_dma_spi_handle.Init.PeriphInc = DMA_PINC_DISABLE;                     /* ���������ģʽ */
    g_dma_spi_handle.Init.MemInc = DMA_MINC_ENABLE;                         /* �洢������ģʽ */
    g_dma_spi_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;        /* �������ݳ���: */
    g_dma_spi_handle.Init.MemDataAlignment = DMA_PDATAALIGN_BYTE;           /* �洢�����ݳ���: */
    g_dma_spi_handle.Init.Mode = DMA_NORMAL;                              /* ѭ��ģʽѡ�� */
    g_dma_spi_handle.Init.Priority = DMA_PRIORITY_HIGH;                   /* �е����ȼ� */
    __HAL_DMA_DISABLE(&g_dma_spi_handle);                              /* ��ʼ��֮ǰ�ȹر�DMA */
    HAL_DMA_Init(&g_dma_spi_handle);

/* ��dma�ж��������־��������ݶ�ʧ�����spi�ٶ�Խ��Խ���� */
    HAL_NVIC_SetPriority(DMA1_Channel3_IRQn,2,0);
    HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
}

void LCD_Line_init(void)
{
    __HAL_RCC_SPI1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    GPIO_InitTypeDef gpiotye;
    gpiotye.Mode = GPIO_MODE_OUTPUT_PP;
    gpiotye.Pin = (LCD_RES_GPIO_PIN | LCD_CS_GPIO_PIN | LCD_DC_GPIO_PIN | LCD_BL_GPIO_PIN );
    gpiotye.Pull = GPIO_PULLUP;
    gpiotye.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA,&gpiotye);
    
    gpiotye.Pin = LCD_SDA_GPIO_PIN | LCD_CLK_GPIO_PIN;
    gpiotye.Mode = GPIO_MODE_AF_PP;
    HAL_GPIO_Init(GPIOA,&gpiotye);
    
    g_spi_lcd_hander.Instance = SPI1;
    g_spi_lcd_hander.Init.Mode = SPI_MODE_MASTER;
    g_spi_lcd_hander.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    g_spi_lcd_hander.Init.CLKPhase = SPI_PHASE_2EDGE;
    g_spi_lcd_hander.Init.CLKPolarity = SPI_POLARITY_HIGH;
    g_spi_lcd_hander.Init.DataSize = SPI_DATASIZE_8BIT;
    g_spi_lcd_hander.Init.Direction = SPI_DIRECTION_2LINES;
    g_spi_lcd_hander.Init.NSS = SPI_NSS_SOFT;
    g_spi_lcd_hander.Init.FirstBit = SPI_FIRSTBIT_MSB;
    g_spi_lcd_hander.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    g_spi_lcd_hander.Init.CRCPolynomial = 7;
    HAL_SPI_Init(&g_spi_lcd_hander);
    dma_spi_init();
    
    __HAL_LINKDMA(&g_spi_lcd_hander,hdmatx, g_dma_spi_handle);         /* ��DMA��adc��ϵ���� */
    __HAL_SPI_ENABLE(&g_spi_lcd_hander);                             // ����DMA��������ж�
    /* ��LCD���� */
    LCD_BL_Set(1);
}

/**
 * @brief       дһ���ֽ�����
 * @param       
 * @retval      ��
 */
void LCD_WR_REG (uint8_t data)
{
    LCD_CS_Set(0);
    LCD_DC_Set(0);
    HAL_SPI_Transmit(&g_spi_lcd_hander,&data,1,1000);
    LCD_CS_Set(1);
}
/**
 * @brief       дһ���ֽ�����
 * @param       
 * @retval      ��
 */
void LCD_WR_DATA (uint8_t data)
{
    LCD_CS_Set(0);
    LCD_DC_Set(1);
    HAL_SPI_Transmit(&g_spi_lcd_hander,&data,1,1000);
    LCD_CS_Set(1);
}


/**
 * @brief       ��DMA�ѻ�����������д��spi�Ĵ���(LVGL˫����ר��)
 * @param       
 * @retval      ��
 */
#define Double_buffer   0
void LCD_RAM_DATA (uint8_t *data,uint32_t data_size)
{

    #if  Double_buffer == 1
    /* DMA���� ˫������ */
    while(1)
    {
        if(dma_sta == 1)
        {
            dma_sta = 0;
            break; 
        }
    }
    #endif
    
    /* SPI��ʼ */
    LCD_CS_Set(0);
    LCD_DC_Set(1);
    
    /* �����͵����ݲ�Ϊ0 */
    while(data_size > 0)
    {
        
        /* �������DMA���˵�������ݣ��ȷ���ȥ */
        if(data_size >= 65000)
        {
            /* ���ͻ�����2�����ݣ���ʱ���õ�������Ҳ����д������1�������� */
            HAL_SPI_Transmit_DMA(&g_spi_lcd_hander,(uint8_t *)data,65000);
            data_size -= 65000;
        }
        else
        {
            /* ���ͻ�����2�����ݣ���ʱ���õ�������Ҳ����д������1�������� */
            HAL_SPI_Transmit_DMA(&g_spi_lcd_hander,(uint8_t *)data,data_size);
            data_size = 0;
        }
    }
    
    #if  Double_buffer == 0
    /* DMA���� �������� */
    while(1)
    {
        if(dma_sta == 1)
        {
            dma_sta = 0;
            break; 
        }
    }
    #endif
}


/**
 * @brief       ��DMA�ѻ�����������д��spi�Ĵ���(���ʺ�LVGL)
 * @param       
 * @retval      ��
 */
//void LCD_RAM_DATA (uint8_t *data,uint32_t data_size)
//{
//    uint32_t i;
//    
//    /* ����һ��DMA���� */
//    while(1)
//    {
//        if(dma_sta == 1)
//        {
//            dma_sta = 0;
//            break; 
//        }
//    }
//    
//    /* SPI��ʼ */
//    LCD_CS_Set(0);
//    LCD_DC_Set(1);
//    
//    /* ������1�����ݸ��Ƶ�������2 */
//    memcpy(lcd_gram_data2, data, sizeof(lcd_gram_data2));
//    /* �����͵����ݲ�Ϊ0 */
//    while(data_size > 0)
//    {
//        
//        /* �������DMA���˵�������ݣ��ȷ���ȥ */
//        if(data_size >= 65000)
//        {
//            /* ���ͻ�����2�����ݣ���ʱ���õ�������Ҳ����д������1�������� */
//            HAL_SPI_Transmit_DMA(&g_spi_lcd_hander,(uint8_t *)lcd_gram_data2,65000);
//            data_size -= 65000;
//        }
//        else
//        {
//            /* ���ͻ�����2�����ݣ���ʱ���õ�������Ҳ����д������1�������� */
//            HAL_SPI_Transmit_DMA(&g_spi_lcd_hander,(uint8_t *)lcd_gram_data2,data_size);
//            data_size = 0;
//        }
//    }
//}

#else
void LCD_Line_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef gpiotye;
    gpiotye.Mode = GPIO_MODE_OUTPUT_PP;
    gpiotye.Pin = (LCD_RES_GPIO_PIN | LCD_CS_GPIO_PIN | LCD_DC_GPIO_PIN | LCD_SDA_GPIO_PIN | LCD_CLK_GPIO_PIN);
    gpiotye.Pull = GPIO_PULLUP;
    gpiotye.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA,&gpiotye);
}

void LCD_WR_REG (uint8_t data)
{
    LCD_CS_Set(0);
    LCD_DC_Set(0);
    uint8_t i;
    for(i=0;i<8;i++)
    {
        LCD_CLK_Set(0);
        if (data & 0x80)
        {
            LCD_SDA_Set(1);
        }
        else
        {
            LCD_SDA_Set(0);
        }
        data <<= 1;
        LCD_CLK_Set(1);
    }
    
    LCD_CS_Set(1);
}
void LCD_WR_DATA (uint8_t data)
{
    LCD_CS_Set(0);
    LCD_DC_Set(1);
    uint8_t i;
    for(i=0;i<8;i++)
    {
        LCD_CLK_Set(0);
        if (data & 0x80)
        {
            LCD_SDA_Set(1);
        }
        else
        {
            LCD_SDA_Set(0);
        }
        data <<= 1;
        LCD_CLK_Set(1);
    }
    
    LCD_CS_Set(1);
}

/**
 * @brief       ��DMA�ѻ�����������д��spi�Ĵ���
 * @param       
 * @retval      ��
 */
void LCD_RAM_DATA (uint8_t *data,uint32_t data_size)
{
    uint32_t j;
    uint16_t i,DATA_T;
    for(j=0;j<data_size;j++)
    {
        DATA_T = data[j];
        for(i=0;i < 8 ;i++)
        {
            LCD_CLK_Set(0);
            if (DATA_T & 0x80)
            {
                LCD_SDA_Set(1);
            }
            else
            {
                LCD_SDA_Set(0);
            }
            DATA_T <<= 1;
            LCD_CLK_Set(1);
        }
    }
}

#endif

/**
 * @brief       ���÷�Χ,�������������ʼдrgb
 * @param
 * @retval      ��
 */
void LCD_setrange(uint16_t x,uint16_t y,uint16_t xend,uint16_t yend)
{
    #if   Double_buffer == 1
    /* DMA���� ˫������ */
    while(1)
    {
        if(dma_sta == 1)
        {
            break; 
        }
    }
    #endif
    if(xend > 0 && yend > 0)
    {
        y+=20;
        yend+=20;
        LCD_WR_REG(0x2a);
        LCD_WR_DATA(x>>8);
        LCD_WR_DATA(x);
        LCD_WR_DATA(xend>>8);
        LCD_WR_DATA(xend);
        LCD_WR_REG(0x2b);
        LCD_WR_DATA(y>>8);
        LCD_WR_DATA(y);
        LCD_WR_DATA(yend>>8);
        LCD_WR_DATA(yend);
        LCD_WR_REG(0x2c);
    }
}

/**
 * @brief       ST7789 �Ĵ�����ʼ������
 * @param       ��
 * @retval      ��
 */
void lcd_ex_st7789_reginit(uint8_t i)
{
    LCD_WR_REG(0x11);

    delay_ms(120); 

    LCD_WR_REG(0x36); //������ʾ����MX,MY,RGB mode 
    
    switch (i)//��ʾ�ķ���
    {
        case    0: LCD_WR_DATA(0x00);break;//����
        case    1: LCD_WR_DATA(0xC0);break;//����
        case    2: LCD_WR_DATA(0x70);break;//������ת180��
        default  : LCD_WR_DATA(0xA0);break;//������ת180��
    }

    LCD_WR_REG(0x3A);
//    LCD_WR_DATA(0X06); /* RGB666ģʽ */
    LCD_WR_DATA(0X05); /* RGB565ģʽ */
//    LCD_WR_DATA(0X03); /* RGB444ģʽ */

    LCD_WR_REG(0xB2);
    LCD_WR_DATA(0x0C);
    LCD_WR_DATA(0x0C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x33);
    LCD_WR_DATA(0x33);

    LCD_WR_REG(0xB7);
    LCD_WR_DATA(0x35);

    LCD_WR_REG(0xBB); /* vcom */
    LCD_WR_DATA(0x32);  /* 30 */

    LCD_WR_REG(0xC2);
    LCD_WR_DATA(0x01);

    LCD_WR_REG(0xC3); /* vrh */
    LCD_WR_DATA(0x15);

    LCD_WR_REG(0xC4); /* vdv */
    LCD_WR_DATA(0x20);  /* 20 */

    LCD_WR_REG(0xC6);
    LCD_WR_DATA(0x0f); //0x0F:60Hz

    LCD_WR_REG(0xD0);
    LCD_WR_DATA(0xA4); 
    LCD_WR_DATA(0xA1); 

    LCD_WR_REG(0xE0); /* Set Gamma  */
    LCD_WR_DATA(0xD0);
    LCD_WR_DATA(0x08);
    LCD_WR_DATA(0x0E);
    LCD_WR_DATA(0x09);
    LCD_WR_DATA(0x09);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x31);
    LCD_WR_DATA(0x33);
    LCD_WR_DATA(0x48);
    LCD_WR_DATA(0x17);
    LCD_WR_DATA(0x14);
    LCD_WR_DATA(0x15);
    LCD_WR_DATA(0x31);
    LCD_WR_DATA(0x34);


    LCD_WR_REG(0XE1);  /* Set Gamma */
    LCD_WR_DATA(0xD0);
    LCD_WR_DATA(0x08);
    LCD_WR_DATA(0x0E);
    LCD_WR_DATA(0x09);
    LCD_WR_DATA(0x09);
    LCD_WR_DATA(0x15);
    LCD_WR_DATA(0x31);
    LCD_WR_DATA(0x33);
    LCD_WR_DATA(0x48);
    LCD_WR_DATA(0x17);
    LCD_WR_DATA(0x14);
    LCD_WR_DATA(0x15);
    LCD_WR_DATA(0x31);
    LCD_WR_DATA(0x34);
    
    LCD_WR_REG(0x21);   /* ��ת��ʾ */
    LCD_WR_REG(0x29); /* display on */
    

}

/**
 * @brief       ����RGBֵ���Զ�����һ��HEX
 * @param       RGBȡֵ��Χ0-31
 * @retval      ��
 */
uint16_t LCD_changle_rgb(uint8_t r,uint8_t g,uint8_t b)
{
    if(r>31 || g>31 || b>>31)
    {
        while(1);
    }
    return r<<11|g<<6|b;
}

/**
 * @brief       ���㺯��
 * @param
 * @retval      ��
 */
//void LCD_showpoint(uint16_t x,uint16_t y ,uint16_t color)
//{
//    LCD_setrange(x,y,x,y);
//    LCD_WR_DATA( (uint8_t)(color >> 8) );  /*   ��λ   */
//    LCD_WR_DATA( (uint8_t)color );  /*   ��λ   */
//}
/**
 * @brief       ��������
 * @param
 * @retval      ��
 */
void LCD_clear(uint16_t color)
{
    uint32_t i,j,z;
    
    LCD_setrange(0,0,lcddev.lcd_x - 1,lcddev.lcd_y - 1);
    /*   �Ƿ�ʹ��Ӳ��spi+dma   */
    #if USE_HSPIDMA == 1
    
    for(i=0; i < lcddev.lcd_y / bufer_y; i++)
    {
        for(j=0;j < bufer_x * bufer_y * 2;j+=2)
        {
            lcd_gram_data[j] = color >> 8;
            lcd_gram_data[j+1] = color;
        }
        LCD_RAM_DATA (lcd_gram_data,bufer_x * bufer_y * 2);
    }
    z = lcddev.lcd_y % bufer_y;
    if( z )
    {
        for(j=0;j < bufer_x * z * 2;j+=2)
        {
            lcd_gram_data[j] = color >> 8;
            lcd_gram_data[j+1] = color;
        }
        LCD_RAM_DATA (lcd_gram_data,bufer_x * z * 2);
    }
    
    #else
    
    LCD_CS_Set(0);
    LCD_DC_Set(1);
    
    for(i=0; i<lcddev.lcd_y / bufer_y; i++)
    {
        LCD_RAM_DATA (lcd_gram_data,sizeof(lcd_gram_data));
    }
    
    LCD_CS_Set(1);
    #endif
}


/**
 * @brief       ��ָ��λ����ʾһ���ַ�
 * @param       x,y  : ����
 * @param       chr  : Ҫ��ʾ���ַ�
 * @param       size : �����С /24
 * @param       mode : ���ӷ�ʽ(1); �ǵ��ӷ�ʽ(0);
 * @retval      ��
 */
void lcd_show_chars(uint16_t x, uint16_t y, char *chr,uint16_t length, uint8_t size, uint16_t color,uint16_t blakcolor)
{
    uint16_t num;
    uint8_t i,char_y,char_x,charbit;
    uint8_t str_sta = 1;
    uint8_t *p;
    uint32_t bufer_num = 0;
    char temp;
    const uint8_t wide = size/2;
    const uint8_t csize = (size/8) + ((size % 8)? 1:0);
    const uint8_t ram = size % 8;
    
    while( x + length * wide > lcddev.lcd_x || y + size > lcddev.lcd_y);
    
    
    /* �Ƿ�ʹ�û��巽ʽ */
    #define   USE_BUFER      1
    /* ʹ�û��巽ʽ */
    #if  USE_BUFER  == 1
    
    /* ������������Ӳ��SPI+DMA���� */
    LCD_setrange( x, y,x+length*size/2-1,y+size-1);
    
    /*   ���ֿ���ˢ�µ�ǰ�ַ���RAM_bufer   */
    for( i = 0; i < csize ; i++ )       //y����ˢ���ٴ�
    {
        if( ram && i == (csize-1))     //��Ч���ݲ��������ֽڶ����������һ���ֽ�
        {
            charbit = ram;     //ˢ�겿��λ
        }
        else
        {
            charbit = 8;     //ˢ������λ
        }
        for(char_y=0;char_y<charbit;char_y++)     //ˢ��y����
        {
            /* ������λ��ʱ���Ѿ������˽�������
                ��������λ1*/
            str_sta = 1;
            
            for(num = 0; num < length; num++)
            {
                /* ��ȡ��N���ַ� ָ��ָ����*/
                temp  =  chr[num];
                if( size == 16 )
                {
                    p = (uint8_t*)&asc2_1608[temp - ' '][0];
                }
                else if( size == 24 )
                {
                    p = (uint8_t*)&asc2_2412[temp - ' '][0];
                }else
                {
                    while(1);
                }
                /* ����ַ����������� ����Ķ��ÿ���*/
                if(temp == 0 || str_sta == 0)
                {
                    p = (uint8_t*)&asc2_2412[0][0];
                    str_sta = 0;
                }
                
                for(char_x=0;char_x<wide;char_x++)     //ˢ��x����
                {
                    if(p[i * wide + char_x] & (0x80 >> char_y))  //��iҳ��Ҫָ���ƶ�wide*i����ַ
                    {
                        lcd_gram_data[bufer_num] = color >> 8;
                        lcd_gram_data[bufer_num+1] = color;
                    }
                    else
                    {
                        lcd_gram_data[bufer_num] = blakcolor >> 8;
                        lcd_gram_data[bufer_num+1] = blakcolor;
                    }
                    
                    /* ��������˻��������ȷ���ȥ */
                    if(bufer_num + 1 >= bufer_x * bufer_y * 2)
                    {
                        LCD_RAM_DATA (lcd_gram_data,bufer_x * bufer_y * 2);
                        bufer_num = 0;
                    }
                    else
                    {
                        bufer_num+=2;
                    }
                    
                }
            }
        }
    }
    
    LCD_RAM_DATA (lcd_gram_data,bufer_num);
    
    #else
    for(num = 0; num < length; num++)
    {
        /* �������� */
    ````LCD_setrange( x, y,x+size/2-1,y+size-1);
        x += size/2;
        /* ��ȡ��N���ַ� ָ��ָ����*/
        temp  =  chr[num];
        if( size == 16 )
        {
            p = (uint8_t*)&asc2_1608[temp - ' '][0];
        }
        else if( size == 24 )
        {
            p = (uint8_t*)&asc2_2412[temp - ' '][0];
        }else
        {
            while(1);
        }
        /* ����ַ����������� ����Ķ��ÿ���*/
        if(temp == 0 || str_sta == 0)
        {
            p = (uint8_t*)&asc2_2412[0][0];
            str_sta = 0;
        }
        
        /*   ���ֿ���ˢ�µ�ǰ�ַ���RAM_bufer   */
        for( i = 0; i < csize ; i++ )       //y����ˢ���ٴ�
        {
            if( ram && i == (csize-1))     //��Ч���ݲ��������ֽڶ����������һ���ֽ�
            {
                charbit = ram;     //ˢ�겿��λ
            }
            else
            {
                charbit = 8;     //ˢ������λ
            }
            for(char_y=0;char_y<charbit;char_y++)     //ˢ��y����
            {
                for(char_x=0;char_x<wide;char_x++)     //ˢ��x����
                {
                    if(p[char_x] & (0x80 >> char_y))
                    {
                        LCD_WR_DATA( (uint8_t)(color >> 8) );  /*   ��λ   */
                        LCD_WR_DATA( (uint8_t)color );  /*   ��λ   */
                    }
                    else
                    {
                        LCD_WR_DATA( (uint8_t)(blakcolor >> 8) );  /*   ��λ   */
                        LCD_WR_DATA( (uint8_t)blakcolor );  /*   ��λ   */
                    }
                }
            }
            p += wide;     //ָ���ƶ�����һ������
        }
    }
    #endif
    
}
/**
 * @brief       ��ָ��λ����ʾ�޷�������
 * @param       size      : �����С 12/16/24/32
 * @param       blakcolor : �ֵı�����ɫ
 * @retval      ���λ��
 */
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num,uint16_t length, uint8_t size, uint16_t color,uint16_t blakcolor)
{
    uint32_t a , b = {0};
    char temp,i = {0};
    a = num;
    b = 1;
    while( a >= 10 )
    {
        a /= 10;
        b *= 10;
    }
    for( a = num ; i < length; b /= 10)
    {
        i++;
        temp = a / b;
        a = a % b;
        temp = '0'+ temp;
        if(b)
        {
            lcd_show_chars( x,  y, &temp, 1, size, color, blakcolor);
        }else
        {
            lcd_show_chars( x, y, " " , 1 ,size,color, blakcolor);
        }
        x += size/2;
    }
    while(i < length)
    {
        i++;
        lcd_show_chars( x, y, "~" , 1 ,size,color, blakcolor);
        x += size/2;
    }
}
/**
 * @brief       ��ָ��λ����ʾ�з�������
 * @param       size      : �����С 12/16/24/32
 * @param       blakcolor : �ֵı�����ɫ
 * @retval      ���λ��
 */
void lcd_show_signednum(uint16_t x, uint16_t y, int32_t num,uint16_t length, uint8_t size, uint16_t color,uint16_t blakcolor)
{
    uint32_t a , b , c = {0};
    char temp,i = {0};
    
    if(num & 0x80000000)
    {
        lcd_show_chars( x, y, "-" , 1 ,size,color, blakcolor);
        a = ~num + 1;
    }else
    {
        lcd_show_chars( x, y, "+" , 1 ,size,color, blakcolor);
        a = num;
    }
    x += size/2;
    b = 1;
    c = a;
    while( c >= 10 )
    {
        c /= 10;
        b *= 10;
    }
    while(i < length)
    {
        temp = a / b;
        a = a % b;
        temp = '0'+ temp;
        if(b)
        {
            lcd_show_chars( x,  y, &temp, 1, size, color, blakcolor);
        }else
        {
            lcd_show_chars( x, y, "~" , 1 ,size,color, blakcolor);
        }
        
        x += size/2;
        b /= 10;
        i++;
    }
}

void LCD_Init(uint8_t i)
{
    LCD_Line_init();
    delay_ms(100);
    LCD_RES_Set(0);//��λ
    delay_ms(100);
    LCD_RES_Set(1);
    delay_ms(100);
    
    lcddev.lcd_x = 240;
    lcddev.lcd_y = 280;
    lcddev.dir = i;
    lcddev.setxcmd = 0x2a;
    lcddev.setycmd = 0x2b;
    lcddev.wramcmd = 0x2c;
    lcd_ex_st7789_reginit(i);
    
    LCD_clear(RED);
    delay_ms(200);
}

/* ��dma�ж��������־��������ݶ�ʧ�����spi�ٶ�Խ��Խ���� */
void DMA1_Channel3_IRQHandler(void)
{
    if(__HAL_DMA_GET_FLAG(&g_dma_spi_handle,DMA_FLAG_TC3)!=RESET )
    {
        //���ͨ��4������ɱ�־
        __HAL_DMA_CLEAR_FLAG(&g_dma_spi_handle,DMA_FLAG_GL3);    /* ��spi_TX�Ĵ���Ϊ�� */
        while(1)
        {
            if(__HAL_SPI_GET_FLAG(&g_spi_lcd_hander,SPI_FLAG_TXE))
            {
                HAL_SPI_DMAStop(&g_spi_lcd_hander);
                /* ����Ƭѡ */
                LCD_CS_Set(1);
                break; 
            }
        }
        dma_sta = 1;
    }
}
