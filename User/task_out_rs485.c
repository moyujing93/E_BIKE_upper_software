/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       FreeRTOS �����ź�������ʵ��
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� ��ӢF103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 *
 ****************************************************************************************************
 */
#include "./SYSTEM/delay/delay.h"
#include  "task_out_rs485.h"
#include "my_gui.h"

/*************�������� **************/

TaskHandle_t            task_out_rs485_Handler;   /* ����ADC��� */

EBIKE_TYPEDEF  ebike_rs485 = { 0 };
lvgl_set_typedef rs485_control;
uint16_t motor_rpm = 0;
uint16_t motor_cun = 0;
rs485_typedef  read_data = {0};
rs485_typedef  write_data = {0};






/**************************************************************************
Function: The data sent by the serial port is assigned
Input   : none
Output  : none
�������ܣ����ݽ��и�ֵ
��ڲ�������
����  ֵ����
**************************************************************************/
static void data_transition(void)
{
    /*          ��������          */
    lvgl_set_typedef rs485_control_temp = lvgl_getcontrol();    /* ��ȡLVGL�����õĲ��� */
    if(rs485_control_temp.sta == 1) rs485_control = rs485_control_temp;

    if(ebike_rs485.mode == e_mode_ebike)
    {
        if(ebike_rs485.rpm > EBIKE_STARTRPM)
        {
            motor_rpm = (ebike_rs485.rpm * 27.72f) + 500;         //  ���� 27.72���ٱ�   ��Ϊ10���ݶ�����ת��
            if(motor_rpm > EBIKE_MOTOR_MAXRPM) motor_rpm = EBIKE_MOTOR_MAXRPM;
           /* m_maxcun��sport_num �����ز˵���LVGL�����õ�ֵ */
            motor_cun = EBIKE_MAX_CURRENT * ((rs485_control.e_maxcun * rs485_control.sport_num) / 10000.00f);
        }else
        {
            motor_rpm = 0;
            motor_cun = 0;
        }
    }else if(ebike_rs485.mode == e_mode_motor)
    {
        if(rs485_control.pwm_sta == SET) // �����ת�ٿ���
        {
           /* m_maxcun��sport_num �����ز˵���LVGL�����õ�ֵ */
            motor_cun = EBIKE_MAX_CURRENT * ((rs485_control.m_maxcun * rs485_control.sport_num) / 10000.00f);
            motor_rpm =  ebike_rs485.set_motor_pwm;
        }else
        {
            motor_rpm = EBIKE_MOTOR_MAXRPM;
            /* m_maxcun��sport_num �����ز˵���LVGL�����õ�ֵ */
            motor_cun = ebike_rs485.set_motor_cun * ((rs485_control.m_maxcun * rs485_control.sport_num) / 10000.00f);
        }
    }else
    {
        motor_rpm = 0;
        motor_cun = 0;
    }
}
void task_out_rs485(void *pvParameters)
{
    uint16_t CCR_temp;
    uint16_t  time = 0;
    uint32_t lastWakeTime = xTaskGetTickCount();
    while(1)
    {
        
        /*   ����ȫ�ֱ�����   */
        xSemaphoreTake(MutexSemaphore_my_ebike,portMAX_DELAY);
        ebike_rs485 = my_ebike;
        xSemaphoreGive(MutexSemaphore_my_ebike);
        /* �ȴ�ADC���ݴ������,���ȴ�FREQ,��Ȼ�л�ģʽ��ʱ������MOTOR����ҪFREQȴȥ�ȴ�FREQ */
        xEventGroupWaitBits(g_EventGroup,Ev_ADC,pdTRUE,pdTRUE,portMAX_DELAY);
        
        //��Ҫ���з��͵����ݽ��и�ֵ
        data_transition();
        
        
        /*      RS485��ʼͨѶ      */
//        xSemaphoreTake(MutexSemaphore_rs485,portMAX_DELAY);
        
        /* ���������� */
        memset(&read_data, 0x00, sizeof(rs485_typedef));
        read_data.slave = slave_bldc;
        read_data.command = command_read;
        read_data.crc = 0;
        read_data.crc = ModbusCRC16_Check((uint8_t*)&read_data,sizeof(rs485_typedef));
        /* RS485���� */
        rs485_send_data((uint8_t*)&read_data,sizeof(rs485_typedef));
        /* �ȴ��ظ���Ϣ */
        while(1)
        {
            if(rs485_receive_data((uint8_t*)&read_data,sizeof(rs485_typedef)) != 0)    /* �дӻ���Ӧ */
            {
                CCR_temp  =  read_data.crc;
                read_data.crc = 0;
                read_data.crc = ModbusCRC16_Check((uint8_t*)&read_data,sizeof(read_data));
                if(CCR_temp == read_data.crc  && read_data.slave == slave_bldc)        /* �ӻ���ȷ�ظ� */
                {
                    ebike_rs485.get_motor_rpm  =  read_data.r_rpm;
                    ebike_rs485.get_motor_cun  =  read_data.r_current;
                    ebike_rs485.voltage = ebike_rs485.voltage * 0.95 + read_data.r_vbus * 0.05;
                    ebike_rs485.hot = read_data.r_temper;
                    //����������
                    ebike_rs485.rs485_error = 0;
                }
                
                break;
            }else
            {
                vTaskDelay(5);
                time++;
                if(time >= 6)
                {
                    //�ۼӴ������
                    ebike_rs485.rs485_error ++;
                    if(ebike_rs485.rs485_error > 1000) ebike_rs485.rs485_error = 1000;
                    break;
                }
            }
        }
        
        vTaskDelay(10);
        /* ����д���� */
        memset(&write_data, 0x00, sizeof(rs485_typedef));
        write_data.slave = slave_bldc;
        write_data.command = command_write;
        write_data.w_rpm  =  motor_rpm;
        write_data.w_current  = motor_cun;
        write_data.crc = 0;
        write_data.crc = ModbusCRC16_Check((uint8_t*)&write_data,sizeof(rs485_typedef));
        /* RS485���� */
        rs485_send_data((uint8_t*)&write_data,sizeof(rs485_typedef));
        
//        xSemaphoreGive(MutexSemaphore_rs485);
        
        
        /*   ����ȫ�ֱ�����   */
        xSemaphoreTake(MutexSemaphore_my_ebike,portMAX_DELAY);
        my_ebike.get_motor_rpm = ebike_rs485.get_motor_rpm;
        my_ebike.get_motor_cun = ebike_rs485.get_motor_cun;
        my_ebike.voltage = ebike_rs485.voltage;
        my_ebike.hot = ebike_rs485.hot;
        my_ebike.rs485_error = ebike_rs485.rs485_error;
        xSemaphoreGive(MutexSemaphore_my_ebike);
        
        vTaskDelay(10);
//        vTaskDelayUntil(&lastWakeTime,(configTICK_RATE_HZ/50));
    }
}
