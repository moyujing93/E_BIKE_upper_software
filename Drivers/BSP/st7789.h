/**
 ****************************************************************************************************
 * @file        spi.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       SPI 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 */

#ifndef __ST7735S_H
#define __ST7735S_H

#include "./SYSTEM/sys/sys.h"

#define WHITE           0xFFFF      /* 白色 */
#define BLACK           0x0000      /* 黑色 */
#define RED             0xF800      /* 红色 */
#define GREEN           0x07E0      /* 绿色 */
#define BLUE            0x001F      /* 蓝色 */ 
#define MAGENTA         0XF81F      /* 品红色/紫红色 = BLUE + RED */
#define YELLOW          0XFFE0      /* 黄色 = GREEN + RED */
#define CYAN            0X07FF      /* 青色 = GREEN + BLUE */
#define BROWN           0XBC40      /* 棕色 */
#define BRRED           0XFC07      /* 棕红色 */
#define GRAY            0X2164      /* 灰色 */ 
#define DARKBLUE        0X01CF      /* 深蓝色 */
#define LIGHTBLUE       0X7D7C      /* 浅蓝色 */ 
#define GRAYBLUE        0X5458      /* 灰蓝色 */ 
#define LIGHTGREEN      0X841F      /* 浅绿色 */  
#define LGRAY           0XC618      /* 浅灰色(PANNEL),窗体背景色 */ 
#define LGRAYBLUE       0XA651      /* 浅灰蓝色(中间层颜色) */ 
#define LBBLUE          0X2B12      /* 浅棕蓝色(选择条目的反色) */ 

/* LCD重要参数集 */
typedef struct
{
    uint16_t lcd_x;     /* LCD 宽度 */
    uint16_t lcd_y;    /* LCD 高度 */
    uint8_t  dir;        /* 横屏还是竖屏控制：0，竖屏；1，横屏。 */
    uint16_t wramcmd;   /* 开始写gram指令 */
    uint16_t setxcmd;   /* 设置x坐标指令 */
    uint16_t setycmd;   /* 设置y坐标指令 */
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
