/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       FreeRTOS 互斥信号量操作实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 精英F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 *
 ****************************************************************************************************
 */
#include "./SYSTEM/delay/delay.h"
#include  "task_out_rs485.h"
#include "my_gui.h"

/*************参数定义 **************/

TaskHandle_t            task_out_rs485_Handler;   /* 任务ADC句柄 */

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
函数功能：数据进行赋值
入口参数：无
返回  值：无
**************************************************************************/
static void data_transition(void)
{
    /*          处理数据          */
    lvgl_set_typedef rs485_control_temp = lvgl_getcontrol();    /* 获取LVGL中设置的参数 */
    if(rs485_control_temp.sta == 1) rs485_control = rs485_control_temp;

    if(ebike_rs485.mode == e_mode_ebike)
    {
        if(ebike_rs485.rpm > EBIKE_STARTRPM)
        {
            motor_rpm = (ebike_rs485.rpm * 27.72f) + 500;         //  等于 27.72减速比   分为10个梯度增加转速
            if(motor_rpm > EBIKE_MOTOR_MAXRPM) motor_rpm = EBIKE_MOTOR_MAXRPM;
           /* m_maxcun和sport_num 是主控菜单里LVGL中设置的值 */
            motor_cun = EBIKE_MAX_CURRENT * ((rs485_control.e_maxcun * rs485_control.sport_num) / 10000.00f);
        }else
        {
            motor_rpm = 0;
            motor_cun = 0;
        }
    }else if(ebike_rs485.mode == e_mode_motor)
    {
        if(rs485_control.pwm_sta == SET) // 如果是转速控制
        {
           /* m_maxcun和sport_num 是主控菜单里LVGL中设置的值 */
            motor_cun = EBIKE_MAX_CURRENT * ((rs485_control.m_maxcun * rs485_control.sport_num) / 10000.00f);
            motor_rpm =  ebike_rs485.set_motor_pwm;
        }else
        {
            motor_rpm = EBIKE_MOTOR_MAXRPM;
            /* m_maxcun和sport_num 是主控菜单里LVGL中设置的值 */
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
        
        /*   放入全局变量中   */
        xSemaphoreTake(MutexSemaphore_my_ebike,portMAX_DELAY);
        ebike_rs485 = my_ebike;
        xSemaphoreGive(MutexSemaphore_my_ebike);
        /* 等待ADC数据处理完成,不等待FREQ,不然切换模式的时候明明MOTOR不需要FREQ却去等待FREQ */
        xEventGroupWaitBits(g_EventGroup,Ev_ADC,pdTRUE,pdTRUE,portMAX_DELAY);
        
        //对要进行发送的数据进行赋值
        data_transition();
        
        
        /*      RS485开始通讯      */
//        xSemaphoreTake(MutexSemaphore_rs485,portMAX_DELAY);
        
        /* 发出读请求 */
        memset(&read_data, 0x00, sizeof(rs485_typedef));
        read_data.slave = slave_bldc;
        read_data.command = command_read;
        read_data.crc = 0;
        read_data.crc = ModbusCRC16_Check((uint8_t*)&read_data,sizeof(rs485_typedef));
        /* RS485发射 */
        rs485_send_data((uint8_t*)&read_data,sizeof(rs485_typedef));
        /* 等待回复消息 */
        while(1)
        {
            if(rs485_receive_data((uint8_t*)&read_data,sizeof(rs485_typedef)) != 0)    /* 有从机响应 */
            {
                CCR_temp  =  read_data.crc;
                read_data.crc = 0;
                read_data.crc = ModbusCRC16_Check((uint8_t*)&read_data,sizeof(read_data));
                if(CCR_temp == read_data.crc  && read_data.slave == slave_bldc)        /* 从机正确回复 */
                {
                    ebike_rs485.get_motor_rpm  =  read_data.r_rpm;
                    ebike_rs485.get_motor_cun  =  read_data.r_current;
                    ebike_rs485.voltage = ebike_rs485.voltage * 0.95 + read_data.r_vbus * 0.05;
                    ebike_rs485.hot = read_data.r_temper;
                    //清除错误计数
                    ebike_rs485.rs485_error = 0;
                }
                
                break;
            }else
            {
                vTaskDelay(5);
                time++;
                if(time >= 6)
                {
                    //累加错误计数
                    ebike_rs485.rs485_error ++;
                    if(ebike_rs485.rs485_error > 1000) ebike_rs485.rs485_error = 1000;
                    break;
                }
            }
        }
        
        vTaskDelay(10);
        /* 发出写请求 */
        memset(&write_data, 0x00, sizeof(rs485_typedef));
        write_data.slave = slave_bldc;
        write_data.command = command_write;
        write_data.w_rpm  =  motor_rpm;
        write_data.w_current  = motor_cun;
        write_data.crc = 0;
        write_data.crc = ModbusCRC16_Check((uint8_t*)&write_data,sizeof(rs485_typedef));
        /* RS485发射 */
        rs485_send_data((uint8_t*)&write_data,sizeof(rs485_typedef));
        
//        xSemaphoreGive(MutexSemaphore_rs485);
        
        
        /*   放入全局变量中   */
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
