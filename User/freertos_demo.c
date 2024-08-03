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
 *  3.22 每个信息采集任务给一个队列，队列长度是1，数据类型为结构体，都加入队列集
 *  3.22 增加输出任务 lgvl中的参数,RS485,GPIO控制mos
 *  3.22 在数据处理任务中读取队列集，处理好全局数据,任务通知指定任务.
 *
 ****************************************************************************************************
 */

/*FreeRTOS*********************************************************************************************/

#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "freertos_demo.h"
#include  "my_gui.h"
#include  "task_adc.h"
#include  "task_freq.h"
#include  "task_out_rs485.h"
#include  "task_out_light.h"
/******************************************************************************************************/
/*结构体与变量*/

EventGroupHandle_t g_EventGroup;       /* 事件标志组句柄 */
QueueSetHandle_t g_xQueueSet_Input;       /* 队列集句柄 */

SemaphoreHandle_t MutexSemaphore_rs485;           /* 互斥信号量  */
SemaphoreHandle_t MutexSemaphore_my_ebike;         /* 互斥信号量  */

volatile GET_ADC_typedef  my_adc;


volatile EBIKE_TYPEDEF  my_ebike = 
{
    .mode = e_mode_ebike,
    .voltage = 20000,
    .hot = 0,
    .rs485_error = 0,
    .rpm = 0,
    .size = 2100,
    .speed = 0,
    .mileage = 0,
    .set_motor_cun = 0,
    .set_motor_pwm = 0,
    .get_motor_rpm = 0,
    .get_motor_cun = 0,
    .e_maxcun   =  15000,
    .m_maxcun   =  15000,
};
/************************************************************************************************/
/*FreeRTOS配置*/

/* 开始任务 */
TaskHandle_t            StartTask_Handler;  /* 任务句柄 */
void start_task(void *pvParameters);        /* 任务函数 */
/* LCD */
TaskHandle_t            task_lcd_Handler;
void task_lcd(void *pvParameters);
/* 数据处理并发送 */
TaskHandle_t            task_data_disp_Handler;
void task_data_disp(void *pvParameters);
/* 打杂的任务 */
TaskHandle_t            Task3Task_Handler;
void task3(void *pvParameters);




/************************************************************************************************/

/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{
    xTaskCreate((TaskFunction_t )start_task,            /* 任务函数 */
                (const char*    )"start_task",          /* 任务名称 */
                (uint16_t       )128,        /* 任务堆栈大小 */
                (void*          )NULL,                  /* 传入给任务函数的参数 */
                (UBaseType_t    )1,       /* 任务优先级 */
                (TaskHandle_t*  )&StartTask_Handler);   /* 任务句柄 */
    vTaskStartScheduler();
}

/**
 * @brief       start_task
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           /* 进入临界区 */
    
    /* 创建事件标志组 */
    g_EventGroup = xEventGroupCreate();
    
    /* 创建队列集 */
    g_xQueueSet_Input = xQueueCreateSet(12);   // 长度远远大于实际
    
    /* 创建队列 */
    g_xQueue_adc = xQueueCreate(3,sizeof(GET_ADC_typedef));
    g_xQueue_freq = xQueueCreate(3,sizeof(GET_speed_typedef));
    
    /* 创建互斥信号量 */
    MutexSemaphore_rs485 = xSemaphoreCreateMutex();
    MutexSemaphore_my_ebike = xSemaphoreCreateMutex();
    
    /* 加入队列集 */
    xQueueAddToSet(g_xQueue_adc,g_xQueueSet_Input);
    xQueueAddToSet(g_xQueue_freq,g_xQueueSet_Input);
                
    xTaskCreate((TaskFunction_t )task3,
                (const char*    )"task3",
                (uint16_t       )256,
                (void*          )NULL,
                (UBaseType_t    )5,
                (TaskHandle_t*  )&Task3Task_Handler);
                
    xTaskCreate((TaskFunction_t )task_lcd,
                (const char*    )"task_lcd",
                (uint16_t       )1024,
                (void*          )NULL,
                (UBaseType_t    )6,
                (TaskHandle_t*  )&task_lcd_Handler);
                
    xTaskCreate((TaskFunction_t )task_data_disp,
                (const char*    )"task_data_disp",
                (uint16_t       )128,
                (void*          )NULL,
                (UBaseType_t    )9,
                (TaskHandle_t*  )&task_data_disp_Handler);
                
    xTaskCreate((TaskFunction_t )task_adc,
                (const char*    )"task_adc",
                (uint16_t       )128,
                (void*          )NULL,
                (UBaseType_t    )8,
                (TaskHandle_t*  )&task_adc_Handler);
                
    xTaskCreate((TaskFunction_t )task_freq,
                (const char*    )"task_freq",
                (uint16_t       )128,
                (void*          )NULL,
                (UBaseType_t    )7,
                (TaskHandle_t*  )&task_freq_Handler);
                
    xTaskCreate((TaskFunction_t )task_out_rs485,
                (const char*    )"task_out_rs485",
                (uint16_t       )128,
                (void*          )NULL,
                (UBaseType_t    )10,
                (TaskHandle_t*  )&task_out_rs485_Handler);
                
    xTaskCreate((TaskFunction_t )task_out_light,
                (const char*    )"task_out_light",
                (uint16_t       )64,
                (void*          )NULL,
                (UBaseType_t    )1,
                (TaskHandle_t*  )&task_out_light_Handler);
                
                /* 获取剩余空间 */
                uint32_t i = configTOTAL_HEAP_SIZE / 10;
                while(xPortGetFreeHeapSize() < i);   /* 卡在这里说明前面分配的堆栈快超出范围了 */
                
    vTaskDelete(StartTask_Handler); /* 删除开始任务 */
    taskEXIT_CRITICAL();            /* 退出临界区 */
}
/**
 * @brief       task_lcd
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task_lcd(void *pvParameters)
{
    while(1)
    {
        
        xSemaphoreTake(MutexSemaphore_my_ebike,portMAX_DELAY);
        ebike_lvgl.rpm = my_ebike.rpm;
        ebike_lvgl.speed = my_ebike.speed;
        ebike_lvgl.hot = my_ebike.hot;
        ebike_lvgl.voltage = my_ebike.voltage;
        ebike_lvgl.mileage = my_ebike.mileage;
        ebike_lvgl.get_motor_cun = my_ebike.get_motor_cun;
        ebike_lvgl.get_motor_rpm = my_ebike.get_motor_rpm;
        xSemaphoreGive(MutexSemaphore_my_ebike);
        
        lv_timer_handler(); /* LVGL计时器 */
        
        xSemaphoreTake(MutexSemaphore_my_ebike,portMAX_DELAY);
        my_ebike.mode = ebike_lvgl.mode;
        my_ebike.e_maxcun = ebike_lvgl.e_maxcun;
        my_ebike.m_maxcun = ebike_lvgl.m_maxcun;
        my_ebike.size = ebike_lvgl.size;
        xSemaphoreGive(MutexSemaphore_my_ebike);
        
        vTaskDelay(10);     /* 直接给数字可以调节LVGL的最大CPU使用率 */
    }
}


/**
 * @brief       原始数据进行处理在发送到指定位置
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task_data_disp(void *pvParameters)
{
    uint32_t lastWakeTime = xTaskGetTickCount();
    QueueSetMemberHandle_t    activate_member;
    lvgl_set_typedef light_control;
    while(1)
    {
        activate_member = xQueueSelectFromSet(g_xQueueSet_Input, portMAX_DELAY);/* 等待队列集中的队列接收到消息 */
        
        if(activate_member == g_xQueue_adc)
        {
            GET_ADC_typedef    data_temp;
            static uint32_t acc_get;
            if(xQueueReceive(g_xQueue_adc,&data_temp,0)  != pdFALSE )
            {
                /*  这个电门ADC范围是1050-3200  */
                if(data_temp.ACC_cun < 1100)
                {
                    acc_get = 0;
                }else if (data_temp.ACC_cun > 1150)
                {
                    acc_get  =  data_temp.ACC_cun - (1150 - 200);    // 让油门初段更有力
                    if(acc_get > 2200)  acc_get = 2200;
                }
                xSemaphoreTake(MutexSemaphore_my_ebike,portMAX_DELAY);
                my_ebike.set_motor_cun = (my_ebike.set_motor_cun + ((acc_get / 2200.00f) * EBIKE_MAX_CURRENT)) / 2; /* 简单处理成电流的原始数据 */
                my_ebike.set_motor_pwm = ( my_ebike.set_motor_pwm  + (acc_get * (EBIKE_MOTOR_MAXRPM / 2200.0f))) / 2;
                xSemaphoreGive(MutexSemaphore_my_ebike);
                
                xEventGroupSetBits(g_EventGroup,Ev_ADC);    // 设置事件标志组
                
                /* 自动灯光 */
                lvgl_set_typedef light_control_temp = lvgl_getcontrol();    /* 获取LVGL中设置的值 */
                if(light_control_temp.sta == 1)
                {
                    light_control = light_control_temp;
                }
                int16_t set_num = 1000 - ((10 - light_control.light_Threshold) * 100);
                if(data_temp.LSEN_cun > (set_num + 100) || light_control.auto_light_sta == 0)
                {
                    xEventGroupSetBits(g_EventGroup,Ev_LIG_OFF);
                }else if(data_temp.LSEN_cun < (set_num - 100))
                {
                    xEventGroupSetBits(g_EventGroup,Ev_LIG_ON);
                }
            }
        }else if(activate_member == g_xQueue_freq)
        {
            GET_speed_typedef   data_freq = {0};
            
            if(xQueueReceive(g_xQueue_freq,&data_freq,0)  != pdFALSE )
            {
                xSemaphoreTake(MutexSemaphore_my_ebike,portMAX_DELAY);
                my_ebike.speed = (my_ebike.speed * 0.8f) + (data_freq.speed * 0.2f);
                my_ebike.rpm = (my_ebike.rpm * 0.8f) + (data_freq.rpm * 0.2f);
                xSemaphoreGive(MutexSemaphore_my_ebike);
                xEventGroupSetBits(g_EventGroup,Ev_FREQ);
            }
        }
        vTaskDelayUntil(&lastWakeTime,(configTICK_RATE_HZ/50));
    }
}
/**
 * @brief       task3
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void task3(void *pvParameters)
{
    volatile EBIKE_TYPEDEF  ebike_task3 = { 0 };
    ebike_task3.voltage = EBIKE_VOLTAGE_S * 3800;
    ebike_task3.hot = 2500;
    uint8_t s_mode = 0;
    uint32_t t = 0;
    uint32_t count_error = 0;
    uint8_t get_num = 0;
    UBaseType_t  get_stack;
    lv_mem_monitor_t get_lvgl_mem;
    rs485_typedef  get_data = {NULL};
    lv_obj_t *massage_lab = NULL;
    
    #define show_stack   0
    
    #if   show_stack == 1
    /*   创建堆栈使用情况   */
    lv_obj_t *stack_lab = lv_label_create(lv_scr_act());
    lv_obj_align(stack_lab,LV_ALIGN_CENTER,-55,115);
    lv_obj_set_style_bg_color(stack_lab,lv_color_hex(0x000000),LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(stack_lab,125,LV_STATE_DEFAULT);
    #endif
    
    /*   创建警告标签   */
    if(massage_lab == NULL)
    {
        massage_lab = lv_label_create(lv_scr_act());
        lv_obj_align(massage_lab,LV_ALIGN_CENTER,0,0);
        lv_obj_set_style_bg_color(massage_lab,lv_color_hex(0xffffff),LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(massage_lab,255,LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(massage_lab,lv_color_hex(0xff0000),LV_STATE_DEFAULT);
        lv_obj_set_style_radius(massage_lab,15,LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(massage_lab,LV_TEXT_ALIGN_CENTER,LV_STATE_DEFAULT);
        lv_obj_add_flag(massage_lab,LV_OBJ_FLAG_HIDDEN);    /* 隐藏 */
    }
    
    while (1)
    {
        
        /*   放入全局变量中   */
        xSemaphoreTake(MutexSemaphore_my_ebike,portMAX_DELAY);
        ebike_task3.voltage = my_ebike.voltage;
        ebike_task3.hot = my_ebike.hot;
        ebike_task3.rs485_error = my_ebike.rs485_error;
        xSemaphoreGive(MutexSemaphore_my_ebike);
        
        /*   判断数据   */
        if( ebike_task3.rs485_error > 50 || ebike_task3.voltage < (EBIKE_VOLTAGE_S * 3300) || ebike_task3.hot > EBIKE_HOT)
        {
            if(ebike_task3.rs485_error > 50)
            {
                lv_label_set_text(massage_lab,"\n      RS485 ERROR!      \n");
            }else if(ebike_task3.voltage < (EBIKE_VOLTAGE_S * 3300))
            {
                lv_label_set_text(massage_lab,"\n      Voltage Low!      \n");
//                lv_setleg();
            }else if(ebike_task3.hot > EBIKE_HOT)
            {
                lv_label_set_text(massage_lab,"\n      Temperature!      \n");
            }
            lv_obj_clear_flag(massage_lab,LV_OBJ_FLAG_HIDDEN);
        }else if( ebike_task3.rs485_error < 10 && ebike_task3.voltage > ((EBIKE_VOLTAGE_S * 3300) + 500) && ebike_task3.hot < (EBIKE_HOT - 500))
        {
            lv_obj_add_flag(massage_lab,LV_OBJ_FLAG_HIDDEN);    /* 隐藏 */
        }
        
        
        #if   show_stack == 1
        /* 打印LVGL的空间使用量 */
        lv_mem_monitor(&get_lvgl_mem);
        
        /* 打印FREERTOS任务的剩余堆栈大小，用于分析堆栈设置够不够 */
        switch(s_mode)
        {
            case  0 :
                get_stack = uxTaskGetStackHighWaterMark(task_lcd_Handler);
                lv_label_set_text_fmt(stack_lab,"LCD: %dw\nLVGL: %db",(uint32_t)get_stack,get_lvgl_mem.free_size);
                break;
            case  1 :
                get_stack = uxTaskGetStackHighWaterMark(Task3Task_Handler);
                lv_label_set_text_fmt(stack_lab,"TASK3: %dw\nLVGL: %db",(uint32_t)get_stack,get_lvgl_mem.free_size);
                break;
            case  2 :
                get_stack = uxTaskGetStackHighWaterMark(task_out_rs485_Handler);
                lv_label_set_text_fmt(stack_lab,"RS485: %dw\nLVGL: %db",(uint32_t)get_stack,get_lvgl_mem.free_size);
                break;
            case  3 :
                get_stack = uxTaskGetStackHighWaterMark(task_adc_Handler);
                lv_label_set_text_fmt(stack_lab,"ADC: %dw\nLVGL: %db",(uint32_t)get_stack,get_lvgl_mem.free_size);
                break;
            case  4 :
                get_stack = uxTaskGetStackHighWaterMark(task_freq_Handler);
                lv_label_set_text_fmt(stack_lab,"FREQ: %dw\nLVGL: %db",(uint32_t)get_stack,get_lvgl_mem.free_size);
                break;
            case  5 :
                get_stack = uxTaskGetStackHighWaterMark(task_data_disp_Handler);
                lv_label_set_text_fmt(stack_lab,"DATADSP: %dw\nLVGL: %db",(uint32_t)get_stack,get_lvgl_mem.free_size);
                break;
            case  6 :
                get_stack = uxTaskGetStackHighWaterMark(task_out_light_Handler);
                lv_label_set_text_fmt(stack_lab,"OUT_LIGHT: %dw\nLVGL: %db",(uint32_t)get_stack,get_lvgl_mem.free_size);
                break;
        }
        
        
        if((xTaskGetTickCount() - t) > 2000)
        {
            
            if(s_mode >= 6)
            {
                s_mode = 0;
            }else
            {
                s_mode++;
            }
            t = xTaskGetTickCount();
        }
        #endif
        
        
        vTaskDelay(100);
        
    }
}
