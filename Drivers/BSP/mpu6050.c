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
#include "./BSP/MPU6050.h"
#include "./BSP/st7789.h"
#include "./SYSTEM/delay/delay.h"

/******************************************************************************************/
/*************参数定义 **************/
#define    MPU_ID                   0x68     // 设备地址
#define    ACC_LSB                  2048.0   // 2048 LSB/g
#define    GYRO_LSB                 16.40    // 16.4 LSB/°/s
/**************常用的寄存器**************/
#define    MPU6050_PWR_MGMT_1       0x6B     // 重置所有，配置传感器时钟，电源模式，on/off温度
#define    MPU6050_PWR_MGMT_2       0x6C     // 低功耗电源模式下才能启用
#define    MPU6050_WHO_AM_I         0x75     // IIC地址寄存器(默认数值0x68，只读)
#define    MPU6050_GYRO_CONFIG      0x1B     // 陀螺仪自检,测量范围
#define    MPU6050_ACCEL_CONFIG     0x1C     // 加速计自检,测量范围
#define    MPU6050_SMPLRT_DIV       0x19     // 陀螺仪采样率=陀螺仪输出速率/这个数值
#define    MPU6050_CONFIG           0x1A     // 低通滤波频率
#define    MPU6050_ACCEL_XOUT_H     0x3B
#define    MPU6050_ACCEL_XOUT_L     0x3C
#define    MPU6050_ACCEL_YOUT_H     0x3D
#define    MPU6050_ACCEL_YOUT_L     0x3E
#define    MPU6050_ACCEL_ZOUT_H     0x3F
#define    MPU6050_ACCEL_ZOUT_L     0x40
#define    MPU6050_GYRO_XOUT_H      0x43
#define    MPU6050_GYRO_XOUT_L      0x44
#define    MPU6050_GYRO_YOUT_H      0x45
#define    MPU6050_GYRO_YOUT_L      0x46
#define    MPU6050_GYRO_ZOUT_H      0x47
#define    MPU6050_GYRO_ZOUT_L      0x48
/******************************************************************************************/

/**
 * @brief       MPU6050发送多个数据
 * @param       devaddr      设备地址
 * @param       regaddr :    设备寄存器地址
 * @retval      
 */
uint8_t mpu6050_write_bytes (uint8_t devaddr, uint8_t regaddr, uint8_t writebuf)
{
    uint8_t sta = 1;
    iic_start();                              /*主机发送起始信号*/
    iic_send(devaddr << 1 | 0x00);            /*主机发送写操作地址*/
    sta = iic_waitack();                      /*主机等待应答*/
    iic_send(regaddr);                        /*主机发送寄存器地址*/
    iic_waitack();
    iic_send(writebuf);
    iic_waitack();
    iic_stop();
    return sta;
}


/**
 * @brief       从MPU6050接收多个数据
 * @param       devaddr      设备地址
 * @param       regaddr :    设备寄存器地址
 * @retval      
 */
uint8_t mpu6050_read_nbytes (uint8_t devaddr, uint8_t regaddr, uint8_t length, uint8_t *readbuf)
{
    uint8_t sta = 1;
    iic_start();                              /*主机发送起始信号*/
    iic_send(devaddr << 1 | 0x00);            /*主机发送写操作地址*/
    sta = iic_waitack();                      /*主机等待应答*/
    iic_send(regaddr);                        /*主机发送寄存器地址*/
    iic_waitack();                            /*主机等待应答*/
    iic_start();                              /* 主机发送起始信号*/
    iic_send(devaddr << 1 | 0x01);            /*主机发送读操作地址*/
    iic_waitack();                            /*主机等待应答*/
    for(uint8_t i = 0;i < length-1;i++)
    {
        readbuf[i] = iic_read(1);
    }
    readbuf[length-1] = iic_read(0);
    iic_stop();
    return sta;
}
/**
 * @brief       初始化MPU6050
 * @param       
 * @retval      
 */
void mpu6050_init(void)
{
    iic_gpioinit();
    mpu6050_write_bytes(MPU_ID,MPU6050_PWR_MGMT_1,0x80);    //复位所有寄存器
    delay_ms(100);
    mpu6050_write_bytes(MPU_ID,MPU6050_PWR_MGMT_1,0x09);    //电源模式和时钟选择
    mpu6050_write_bytes(MPU_ID,MPU6050_PWR_MGMT_2, 0x00);   //仅在低功耗模式下启用
    mpu6050_write_bytes(MPU_ID,MPU6050_SMPLRT_DIV, 20);     //gyro采样率= 1k/20=50hz
    mpu6050_write_bytes(MPU_ID,MPU6050_CONFIG,0x06);        // 低通滤波
    mpu6050_write_bytes(MPU_ID,MPU6050_GYRO_CONFIG,0x18);   // 陀螺仪量程：± 2000 °/s
    mpu6050_write_bytes(MPU_ID,MPU6050_ACCEL_CONFIG,0x18);  // 加速度量程：±16g
}
/**
 * @brief       读取MPU6050数据
 * @param       
 * @retval      
 */
uint8_t mpu6050_getid(void)
{
    uint8_t temp = 0;
    mpu6050_read_nbytes(MPU_ID,MPU6050_WHO_AM_I,1,&temp);
    return temp;
}
/**
 * @brief       获取MPU,ID检测有没有通讯成功
 * @param       
 * @retval      pAccX/pAccY/pAccZ        用来保存X/Y/Z轴的加速度
 *              pGyroX/pGyroY/pGyroZ     用来保存X/Y/Z轴的角速度
 */
uint8_t mpu6050_readdata( mpu_datastruct *mpu_tempdata )
{
    
    uint8_t sta = 1;  //1 = 成功,下面得全部获取成功
    uint8_t tempH,tempL;
    if(mpu_tempdata != NULL)
    {
        sta &= mpu6050_read_nbytes(MPU_ID,MPU6050_ACCEL_XOUT_H,1,&tempH);
        mpu6050_read_nbytes(MPU_ID,MPU6050_ACCEL_XOUT_L,1,&tempL);
        mpu_tempdata->accx = tempH << 8 | tempL;
        sta &= mpu6050_read_nbytes(MPU_ID,MPU6050_ACCEL_YOUT_H,1,&tempH);
        mpu6050_read_nbytes(MPU_ID,MPU6050_ACCEL_YOUT_L,1,&tempL);
        mpu_tempdata->accy = tempH << 8 | tempL;
        sta &= mpu6050_read_nbytes(MPU_ID,MPU6050_ACCEL_ZOUT_H,1,&tempH);
        mpu6050_read_nbytes(MPU_ID,MPU6050_ACCEL_ZOUT_L,1,&tempL);
        mpu_tempdata->accz = tempH << 8 | tempL;
        sta &= mpu6050_read_nbytes(MPU_ID,MPU6050_GYRO_XOUT_H,1,&tempH);
        mpu6050_read_nbytes(MPU_ID,MPU6050_GYRO_XOUT_L,1,&tempL);
        mpu_tempdata->gyrox = tempH << 8 | tempL;
        sta &= mpu6050_read_nbytes(MPU_ID,MPU6050_GYRO_YOUT_H,1,&tempH);
        mpu6050_read_nbytes(MPU_ID,MPU6050_GYRO_YOUT_L,1,&tempL);
        mpu_tempdata->gyroy = tempH << 8 | tempL;
        sta &= mpu6050_read_nbytes(MPU_ID,MPU6050_GYRO_ZOUT_H,1,&tempH);
        mpu6050_read_nbytes(MPU_ID,MPU6050_GYRO_ZOUT_L,1,&tempL);
        mpu_tempdata->gyroz = tempH << 8 | tempL;
        
    }
    return sta;
}

/**
 * @brief       获取MPU,ID检测有没有通讯成功
 * @param       
 * @retval      pAccX/pAccY/pAccZ        用来保存X/Y/Z轴的加速度
 *              pGyroX/pGyroY/pGyroZ     用来保存X/Y/Z轴的角速度
 */
int16_t mpu6050_readgyro( void )
{
    /*目前只是解析了gyro的x轴
    好的效果因该是初始化后，获取以下零偏移值
    后面获取时减去这个值，
    最终角度就是时间*角速度
    如下示意*/
    mpu_datastruct g_mpu_offset,g_mpu_data = {NULL};
    mpu6050_readdata(&g_mpu_offset);
    int32_t gyrox = 0;
    while(1)
    {
        delay_ms(20);
        if(mpu6050_readdata(&g_mpu_data))
        {
            g_mpu_data.gyrox -= g_mpu_offset.gyrox;
            if(abs(g_mpu_data.gyrox) > 50)
            {
                gyrox += (( g_mpu_data.gyrox / 16.4f) * 21.0f);
            }
        }
    }
    return 1;
}
