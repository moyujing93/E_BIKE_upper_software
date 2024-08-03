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
#include "./BSP/MPU6050.h"
#include "./BSP/st7789.h"
#include "./SYSTEM/delay/delay.h"

/******************************************************************************************/
/*************�������� **************/
#define    MPU_ID                   0x68     // �豸��ַ
#define    ACC_LSB                  2048.0   // 2048 LSB/g
#define    GYRO_LSB                 16.40    // 16.4 LSB/��/s
/**************���õļĴ���**************/
#define    MPU6050_PWR_MGMT_1       0x6B     // �������У����ô�����ʱ�ӣ���Դģʽ��on/off�¶�
#define    MPU6050_PWR_MGMT_2       0x6C     // �͹��ĵ�Դģʽ�²�������
#define    MPU6050_WHO_AM_I         0x75     // IIC��ַ�Ĵ���(Ĭ����ֵ0x68��ֻ��)
#define    MPU6050_GYRO_CONFIG      0x1B     // �������Լ�,������Χ
#define    MPU6050_ACCEL_CONFIG     0x1C     // ���ټ��Լ�,������Χ
#define    MPU6050_SMPLRT_DIV       0x19     // �����ǲ�����=�������������/�����ֵ
#define    MPU6050_CONFIG           0x1A     // ��ͨ�˲�Ƶ��
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
 * @brief       MPU6050���Ͷ������
 * @param       devaddr      �豸��ַ
 * @param       regaddr :    �豸�Ĵ�����ַ
 * @retval      
 */
uint8_t mpu6050_write_bytes (uint8_t devaddr, uint8_t regaddr, uint8_t writebuf)
{
    uint8_t sta = 1;
    iic_start();                              /*����������ʼ�ź�*/
    iic_send(devaddr << 1 | 0x00);            /*��������д������ַ*/
    sta = iic_waitack();                      /*�����ȴ�Ӧ��*/
    iic_send(regaddr);                        /*�������ͼĴ�����ַ*/
    iic_waitack();
    iic_send(writebuf);
    iic_waitack();
    iic_stop();
    return sta;
}


/**
 * @brief       ��MPU6050���ն������
 * @param       devaddr      �豸��ַ
 * @param       regaddr :    �豸�Ĵ�����ַ
 * @retval      
 */
uint8_t mpu6050_read_nbytes (uint8_t devaddr, uint8_t regaddr, uint8_t length, uint8_t *readbuf)
{
    uint8_t sta = 1;
    iic_start();                              /*����������ʼ�ź�*/
    iic_send(devaddr << 1 | 0x00);            /*��������д������ַ*/
    sta = iic_waitack();                      /*�����ȴ�Ӧ��*/
    iic_send(regaddr);                        /*�������ͼĴ�����ַ*/
    iic_waitack();                            /*�����ȴ�Ӧ��*/
    iic_start();                              /* ����������ʼ�ź�*/
    iic_send(devaddr << 1 | 0x01);            /*�������Ͷ�������ַ*/
    iic_waitack();                            /*�����ȴ�Ӧ��*/
    for(uint8_t i = 0;i < length-1;i++)
    {
        readbuf[i] = iic_read(1);
    }
    readbuf[length-1] = iic_read(0);
    iic_stop();
    return sta;
}
/**
 * @brief       ��ʼ��MPU6050
 * @param       
 * @retval      
 */
void mpu6050_init(void)
{
    iic_gpioinit();
    mpu6050_write_bytes(MPU_ID,MPU6050_PWR_MGMT_1,0x80);    //��λ���мĴ���
    delay_ms(100);
    mpu6050_write_bytes(MPU_ID,MPU6050_PWR_MGMT_1,0x09);    //��Դģʽ��ʱ��ѡ��
    mpu6050_write_bytes(MPU_ID,MPU6050_PWR_MGMT_2, 0x00);   //���ڵ͹���ģʽ������
    mpu6050_write_bytes(MPU_ID,MPU6050_SMPLRT_DIV, 20);     //gyro������= 1k/20=50hz
    mpu6050_write_bytes(MPU_ID,MPU6050_CONFIG,0x06);        // ��ͨ�˲�
    mpu6050_write_bytes(MPU_ID,MPU6050_GYRO_CONFIG,0x18);   // ���������̣��� 2000 ��/s
    mpu6050_write_bytes(MPU_ID,MPU6050_ACCEL_CONFIG,0x18);  // ���ٶ����̣���16g
}
/**
 * @brief       ��ȡMPU6050����
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
 * @brief       ��ȡMPU,ID�����û��ͨѶ�ɹ�
 * @param       
 * @retval      pAccX/pAccY/pAccZ        ��������X/Y/Z��ļ��ٶ�
 *              pGyroX/pGyroY/pGyroZ     ��������X/Y/Z��Ľ��ٶ�
 */
uint8_t mpu6050_readdata( mpu_datastruct *mpu_tempdata )
{
    
    uint8_t sta = 1;  //1 = �ɹ�,�����ȫ����ȡ�ɹ�
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
 * @brief       ��ȡMPU,ID�����û��ͨѶ�ɹ�
 * @param       
 * @retval      pAccX/pAccY/pAccZ        ��������X/Y/Z��ļ��ٶ�
 *              pGyroX/pGyroY/pGyroZ     ��������X/Y/Z��Ľ��ٶ�
 */
int16_t mpu6050_readgyro( void )
{
    /*Ŀǰֻ�ǽ�����gyro��x��
    �õ�Ч������ǳ�ʼ���󣬻�ȡ������ƫ��ֵ
    �����ȡʱ��ȥ���ֵ��
    ���սǶȾ���ʱ��*���ٶ�
    ����ʾ��*/
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
