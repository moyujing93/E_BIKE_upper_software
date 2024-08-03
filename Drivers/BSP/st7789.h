/**
 ****************************************************************************************************
 * @file        spi.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       SPI ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 */

#ifndef __ST7735S_H
#define __ST7735S_H

#include "./SYSTEM/sys/sys.h"

#define WHITE           0xFFFF      /* ��ɫ */
#define BLACK           0x0000      /* ��ɫ */
#define RED             0xF800      /* ��ɫ */
#define GREEN           0x07E0      /* ��ɫ */
#define BLUE            0x001F      /* ��ɫ */ 
#define MAGENTA         0XF81F      /* Ʒ��ɫ/�Ϻ�ɫ = BLUE + RED */
#define YELLOW          0XFFE0      /* ��ɫ = GREEN + RED */
#define CYAN            0X07FF      /* ��ɫ = GREEN + BLUE */
#define BROWN           0XBC40      /* ��ɫ */
#define BRRED           0XFC07      /* �غ�ɫ */
#define GRAY            0X2164      /* ��ɫ */ 
#define DARKBLUE        0X01CF      /* ����ɫ */
#define LIGHTBLUE       0X7D7C      /* ǳ��ɫ */ 
#define GRAYBLUE        0X5458      /* ����ɫ */ 
#define LIGHTGREEN      0X841F      /* ǳ��ɫ */  
#define LGRAY           0XC618      /* ǳ��ɫ(PANNEL),���屳��ɫ */ 
#define LGRAYBLUE       0XA651      /* ǳ����ɫ(�м����ɫ) */ 
#define LBBLUE          0X2B12      /* ǳ����ɫ(ѡ����Ŀ�ķ�ɫ) */ 

/* LCD��Ҫ������ */
typedef struct
{
    uint16_t lcd_x;     /* LCD ��� */
    uint16_t lcd_y;    /* LCD �߶� */
    uint8_t  dir;        /* ���������������ƣ�0��������1�������� */
    uint16_t wramcmd;   /* ��ʼдgramָ�� */
    uint16_t setxcmd;   /* ����x����ָ�� */
    uint16_t setycmd;   /* ����y����ָ�� */
} _lcd_dev;


void LCD_Init(uint8_t i);
void LCD_Line_init(void);
uint16_t LCD_changle_rgb(uint8_t r,uint8_t g,uint8_t b);
void lcd_show_chars(uint16_t x, uint16_t y, char *chr,uint16_t length, uint8_t size, uint16_t color,uint16_t blakcolor);
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num,uint16_t length, uint8_t size, uint16_t color,uint16_t blakcolor);
void lcd_show_signednum(uint16_t x, uint16_t y, int32_t num,uint16_t length, uint8_t size, uint16_t color,uint16_t blakcolor);
void LCD_clear(uint16_t color);
void LCD_setrange(uint16_t x,uint16_t y,uint16_t xend,uint16_t yend);
void LCD_RAM_DATA_LV (uint16_t *data,uint32_t data_size);

#endif
