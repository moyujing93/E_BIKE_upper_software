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

#ifndef __MPU6050_H
#define __MPU6050_H
#include "./SYSTEM/sys/sys.h"

/**************原始数据载体**************/
typedef struct
{
    int16_t accx;
    int16_t accy;
    int16_t accz;
    int16_t gyrox;
    int16_t gyroy;
    int16_t gyroz;
} mpu_datastruct;

void mpu6050_init(void);
uint8_t mpu6050_getid(void);
uint8_t mpu6050_readdata( mpu_datastruct *mpu_tempdata );

#endif
