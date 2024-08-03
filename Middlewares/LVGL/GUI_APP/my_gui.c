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
 *
 *
 *
 *
 ****************************************************************************************************
 */
 
#include "my_gui.h"
#include "./BSP/adc.h"
#include "./BSP/rs485.h"
#include "./BSP/stmflash.h"

#define pi  0.01745329  //角度*这个参数得到弧度
#define lcd_x  240      //LCD的分辨率
#define lcd_y  280


/* 导入其他文件变量 */
extern lv_indev_t * indev_keypad;  /*   lv输入设备    */

/* 对外开放变量区 */
lv_group_t* key_group;        //按键组
volatile EBIKE_TYPEDEF  ebike_lvgl = { 0 };    


/* 内部变量区 */

uint16_t eeprom_buf[EEP_SIZE];     /* 由于2字节对齐，所以定义个数组，防止指针越界 */
lvgl_set_typedef  my_lvgl_cont = {0};


static lv_obj_t *Battery_bar;
static lv_obj_t *Battery_lab;
static lv_obj_t *pi_img1;     /* 角度300 - 420 代表0 - 50 km/h*/
static lv_obj_t *pi_img2;     /* 角度175 - 110 代表0 - 100 rpm/min*/
static lv_obj_t* MY_UI1;      //UI基础对象
static lv_obj_t* MY_UI2;
static lv_obj_t* set_arc ;
static lv_obj_t *arc_text;
static lv_obj_t *tabview;
static lv_obj_t *speed_lab;      //速度标签
static lv_obj_t *rpm_lab;        //踏频标签
static lv_obj_t *mode_lab;      //电助力模式标签
static lv_obj_t *sport_lab;     //运动模式标签
static lv_obj_t *bg_btn;
lv_timer_t *lv_timer_ui1;
lv_timer_t *lv_timer_lab;

/* tabview的四个选项卡 */
static lv_obj_t* tab1;
static lv_obj_t* tab2;
static lv_obj_t* tab3;
static lv_obj_t* tab4;
static lv_obj_t *content;

/* 信息界面 */
static lv_obj_t *tab_meg;
/* 动力界面 */
static lv_obj_t* slider1;
static lv_obj_t* slider2;
static lv_obj_t* slider3;
/* 自动界面 */
static lv_obj_t* switch_pwm;
static lv_obj_t* switch_light_auto;
/* 资源申明 */
LV_IMG_DECLARE(bl_0);
LV_IMG_DECLARE(bl_1);
LV_IMG_DECLARE(bl_4);
LV_FONT_DECLARE(my_num_35);
LV_FONT_DECLARE(my_num_24);



/**
 * @brief  设置渐变颜色
 * @param  
 * @return 
 */
static void u_set_bg_grad(lv_obj_t *obj_temp,lv_color_t value_s,
    lv_color_t value_e,lv_grad_dir_t dir, lv_opa_t opa)
{
    lv_obj_set_style_bg_color(obj_temp,value_s,LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(obj_temp,value_e,LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(obj_temp,dir,LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj_temp,opa,LV_STATE_DEFAULT);
}

/**
 * @brief  设置事件回调
 * @param  
 * @return 
 */
static void set_cb(lv_event_t *e)
{
    int16_t  value;
    
    my_lvgl_cont.sta = 0;
    
    lv_obj_t *get_obj = lv_event_get_target(e);
    if(get_obj == set_arc)
    {
        value = lv_arc_get_value(get_obj);  /* 获取的值为0-20 */
        lv_label_set_text_fmt(arc_text,"%d",value);
        my_lvgl_cont.light_Threshold = value;
//        my_lvgl_cont.light_Threshold = 2000 - ((value - 10) * 100);
    }else if(get_obj == slider1)
    {
        my_lvgl_cont.e_maxcun = lv_slider_get_value(slider1);
    }else if(get_obj == slider2)
    {
        my_lvgl_cont.auto_cun = lv_slider_get_value(slider2);
    }else if(get_obj == slider3)
    {
        my_lvgl_cont.m_maxcun = lv_slider_get_value(slider3);
    }else if(get_obj == switch_light_auto)
    {
        if(lv_obj_has_state(get_obj,LV_STATE_CHECKED) != 0)
        {
            my_lvgl_cont.auto_light_sta = 1;
        }else
        {
            my_lvgl_cont.auto_light_sta = 0;
        }
    }else if(get_obj == switch_pwm)
    {
        if(lv_obj_has_state(get_obj,LV_STATE_CHECKED) != 0)
        {
            my_lvgl_cont.pwm_sta = 1;
        }else
        {
            my_lvgl_cont.pwm_sta = 0;
        }
    }
    
    my_lvgl_cont.sta = 1;
}

/**
 * @brief  LVGL控制结构体返回
 * @param  
 * @return 
 */
lvgl_set_typedef lvgl_getcontrol(void)
{
    return my_lvgl_cont;
}
void lv_timer_lab_cb(lv_timer_t *e)
{
    lv_table_set_cell_value_fmt(tab_meg,0,1,"%d.%d V",ebike_lvgl.voltage / 1000,(ebike_lvgl.voltage % 1000)/100);
    lv_table_set_cell_value_fmt(tab_meg,1,1,"%d.%d C",ebike_lvgl.hot / 100,(ebike_lvgl.hot % 100)/10);
    lv_table_set_cell_value_fmt(tab_meg,2,1,"%d MA",ebike_lvgl.get_motor_cun);
    lv_table_set_cell_value_fmt(tab_meg,3,1,"%d KM",ebike_lvgl.mileage / 1000);
}

/**
 * @brief  设置菜单跳转
 * @param  
 * @return 
 */
static void back_bg_btn(lv_event_t *e)
{
    uint16_t num;
    static lv_obj_t *label_def;
    lv_obj_t  *get_obj  = lv_event_get_target(e);
    lv_obj_t  *user_obj  = lv_event_get_user_data(e);
    lv_event_code_t get_code  =  lv_event_get_code(e);
    
    if(get_code == LV_EVENT_CLICKED)
    {
        if(get_obj == sport_lab)
        {
            lv_obj_add_flag(MY_UI1,LV_OBJ_FLAG_HIDDEN);    /* 隐藏 */
            lv_obj_clear_flag(MY_UI2,LV_OBJ_FLAG_HIDDEN);
            lv_btnmatrix_set_selected_btn(bg_btn,3);    /* 设置成返回键 */
            lv_group_remove_all_objs(key_group);         /* 配置组 */
            lv_group_add_obj(key_group, bg_btn);
        }else if(get_obj == bg_btn)
        {
            /* 如果创建了提示文本 */
            if(label_def != NULL)
            {
                lv_obj_del(label_def);
                label_def = NULL;
            }
            /* 恢复菜单透明度 */
            lv_obj_set_style_opa(content,255,LV_STATE_DEFAULT);
            
            /* 根据按钮执行相应功能 */
            num = lv_btnmatrix_get_selected_btn(get_obj);
            switch(num)
            {
                case 0 :
                /* 加入到组中key才可以控制 */
                lv_group_remove_all_objs(key_group);
                lv_group_add_obj(key_group, bg_btn);
                lv_group_add_obj(key_group, slider1);
                lv_group_add_obj(key_group, slider2);
                lv_group_add_obj(key_group, slider3);
                break;
                
                case 1 :
                lv_group_remove_all_objs(key_group);
                lv_group_add_obj(key_group, bg_btn);
                if(lv_timer_lab == NULL)
                {
                    lv_timer_lab = lv_timer_create(&lv_timer_lab_cb,500,NULL);
                }
                
                break;
                
                case 2 :
                /* 加入到组中key才可以控制 */
                lv_group_remove_all_objs(key_group);
                lv_group_add_obj(key_group, bg_btn);
                lv_group_add_obj(key_group, switch_pwm);
                lv_group_add_obj(key_group, switch_light_auto);
                lv_group_add_obj(key_group, set_arc);
                break;
                
                case 3 :
                lv_obj_add_flag(MY_UI2,LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(MY_UI1,LV_OBJ_FLAG_HIDDEN);
                lv_group_remove_all_objs(key_group);
                lv_group_add_obj(key_group, sport_lab);
                lv_group_add_obj(key_group,mode_lab);
                
                /* 把数据存入flash模拟的EEPROM中 */
                my_lvgl_cont.eeprom_sta = 12345;      /* 标志下已经写入eeprom */
                taskENTER_CRITICAL();           /* 进入临界区 */
                stmflash_write(FLASH_SAVE_ADDR, (uint16_t *)&my_lvgl_cont, EEP_SIZE);
                taskEXIT_CRITICAL();            /* 退出临界区 */
                
                break;
                
            }
        }
    } else if(get_code == LV_EVENT_KEY)
    {
        if(get_obj == bg_btn)
        {
            /* 删除信息界面定时器 */
            if(lv_timer_lab != NULL)
            {
                lv_timer_del(lv_timer_lab);  //  不知道会不会释放内存
                lv_timer_lab = NULL;
            }
            
            if(lv_group_get_obj_count(key_group) > 1)
            {
                    lv_group_remove_all_objs(key_group);
                    lv_group_add_obj(key_group, bg_btn);
            }
            /* 如果没有创建提示文本 */
            if(label_def == NULL)
            {
                label_def = lv_label_create(MY_UI2);
            }
            
            /* 降低菜单透明度 */
            lv_obj_set_style_opa(content,100,LV_STATE_DEFAULT);
            
            /* 根据按钮执行相应功能 */
            num = lv_btnmatrix_get_selected_btn(get_obj);
            switch(num)
            {
                case 0 :
                    lv_label_set_text(label_def, "设置动力扭矩");
                    lv_obj_align(label_def,LV_ALIGN_CENTER,25,0);
                break;
                case 1 :
                    lv_label_set_text(label_def, "查看驱动板信息");
                    lv_obj_align(label_def,LV_ALIGN_CENTER,25,0);
                break;
                case 2 :
                    lv_label_set_text(label_def, "自动扭矩.灯光");
                    lv_obj_align(label_def,LV_ALIGN_CENTER,25,0);
                break;
                case 3 :
                    lv_label_set_text(label_def, "返回主界面");
                    lv_obj_align(label_def,LV_ALIGN_CENTER,25,0);
                break;
            }
        }else if(get_obj == sport_lab)
        {
            uint32_t i = lv_indev_get_key(lv_indev_get_act());
            my_lvgl_cont.sta = 0;
            if(i == LV_KEY_LEFT)
            {
                my_lvgl_cont.sport_num += 25;
                if(my_lvgl_cont.sport_num > 100) my_lvgl_cont.sport_num = 100;
            }else if(i == LV_KEY_RIGHT)
            {
                my_lvgl_cont.sport_num -= 25;
                if(my_lvgl_cont.sport_num < 50) my_lvgl_cont.sport_num = 50;
            }
            my_lvgl_cont.sta = 1;
            switch(my_lvgl_cont.sport_num)  /* 设置运动模式 */
            {
                case 50 :
                    lv_label_set_text(sport_lab,"D");
                break;
                
                case 75 :
                    lv_label_set_text(sport_lab,"S");
                break;
                
                case 100 :
                    lv_label_set_text(sport_lab,"R");
                break;
            }
        }
    }else if(get_code == LV_EVENT_FOCUSED)
    {
        if(get_obj == mode_lab)
        {
            ebike_lvgl.mode++;
            if(ebike_lvgl.mode > e_mode_leg) ebike_lvgl.mode = e_mode_ebike;
            
            switch(ebike_lvgl.mode)
            {
                case e_mode_ebike :
                lv_label_set_text(mode_lab,"E-BIKE");
                break;
                
                case e_mode_motor :
                lv_label_set_text(mode_lab,"MOTOR");
                break;
                
                case e_mode_leg :
                lv_label_set_text(mode_lab,"LEG");
                break;
            }
            
            my_lvgl_cont.mode = ebike_lvgl.mode;
            lv_group_focus_obj(sport_lab);
        }
    }
}

/**
 * @brief  主界面
 * @param 
 * @return 
 */
static void my_gui_v1(void)
{
    /* 创建UI基础对象 */
    MY_UI1 = lv_obj_create(lv_scr_act());
    lv_obj_clear_flag(MY_UI1,LV_OBJ_FLAG_SCROLLABLE);       /* 去除滚动属性 */
    lv_obj_set_size(MY_UI1,lcd_x,lcd_y);
    
    /***********SPEED部分**********/
    lv_obj_t *my_img1 = lv_img_create(MY_UI1);
    lv_img_set_src(my_img1,&bl_0);
    lv_obj_align(my_img1,LV_ALIGN_CENTER,0,0);
    
   /* 创建仪指针 */
    pi_img1 = lv_img_create(MY_UI1);
    lv_img_set_src(pi_img1,&bl_1);
    lv_obj_set_style_img_recolor(pi_img1,lv_color_hex(0xff0000),LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(pi_img1,150,LV_STATE_DEFAULT);
    lv_img_set_pivot(pi_img1, 5, 6);
    lv_obj_align_to(pi_img1,my_img1,LV_ALIGN_CENTER,-85,0);
    lv_img_set_angle(pi_img1,300*10);
    
    /* 创建刻度数字标签 */
    speed_lab = lv_label_create(my_img1);
    lv_label_set_text_fmt(speed_lab,"KM/H");
    lv_obj_align(speed_lab,LV_ALIGN_CENTER,-85,35);
    speed_lab = lv_label_create(my_img1);
    lv_obj_set_style_text_font(speed_lab,&my_num_35,LV_STATE_DEFAULT);
    lv_label_set_text_fmt(speed_lab,"%d",00);
    lv_obj_align(speed_lab,LV_ALIGN_CENTER,-85,0);
    
    /***********RPM部分**********/
    /* 创建仪指针 */
    pi_img2 = lv_img_create(MY_UI1);
    lv_img_set_src(pi_img2,&bl_1);
    lv_obj_set_style_img_recolor(pi_img2,lv_color_hex(0xff0000),LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(pi_img2,150,LV_STATE_DEFAULT);
    lv_img_set_pivot(pi_img2, 5, 6);
    lv_obj_align_to(pi_img2,my_img1,LV_ALIGN_CENTER,225,-143);
    lv_img_set_angle(pi_img2,175*10);
    /* 创建踏频标签 */
    rpm_lab = lv_label_create(my_img1);
    lv_label_set_text_fmt(rpm_lab,"RPM");
    lv_obj_align(rpm_lab,LV_ALIGN_CENTER,90,-80);
    rpm_lab = lv_label_create(my_img1);
    lv_obj_set_style_text_font(rpm_lab,&my_num_24,LV_STATE_DEFAULT);
    lv_obj_align(rpm_lab,LV_ALIGN_CENTER,90,-105);
    lv_label_set_text_fmt(rpm_lab,"%d",0);
    
    /***********POWER部分**********/
    
    /* 创建电池百分比 */
    Battery_lab = lv_label_create(MY_UI1);
    lv_obj_align(Battery_lab,LV_ALIGN_BOTTOM_RIGHT,-10,-85);
    lv_label_set_text_fmt(Battery_lab,"%d%%",99);
    /* 创建电池进度条 */
    Battery_bar = lv_bar_create(MY_UI1);
    lv_bar_set_range(Battery_bar,0,100);
    lv_bar_set_value(Battery_bar,99, LV_ANIM_OFF);
    lv_obj_set_size(Battery_bar,35,60);

    lv_obj_set_style_radius(Battery_bar,5,LV_PART_MAIN);
    lv_obj_set_style_radius(Battery_bar,5,LV_PART_INDICATOR);
    lv_obj_align_to(Battery_bar,Battery_lab,LV_ALIGN_CENTER,0,50);

    lv_obj_set_style_bg_color(Battery_bar,lv_color_hex(0xffffff),LV_PART_INDICATOR); /* 设置效果 */
    lv_obj_set_style_outline_width(Battery_bar,4,LV_STATE_DEFAULT);
    lv_obj_set_style_outline_color(Battery_bar,lv_color_hex(0xffffff),LV_STATE_DEFAULT);
    
    /* 创建助力模式 */
    mode_lab = lv_label_create(MY_UI1);
    lv_obj_align(mode_lab,LV_ALIGN_BOTTOM_MID,0,0);
    lv_label_set_text(mode_lab,"E-BIKE");
    lv_obj_add_event_cb(mode_lab,back_bg_btn,LV_EVENT_FOCUSED,NULL);

    /* 创建运动模式 */
    sport_lab = lv_label_create(MY_UI1);
    lv_obj_set_style_text_font(sport_lab,&my_num_24,LV_STATE_DEFAULT);
    lv_obj_align(sport_lab,LV_ALIGN_BOTTOM_MID,0,-35);
    lv_label_set_text(sport_lab,"S");
    lv_obj_add_event_cb(sport_lab,back_bg_btn,LV_EVENT_KEY,NULL);
    lv_obj_add_event_cb(sport_lab,back_bg_btn,LV_EVENT_CLICKED,NULL);
}

/**
 * @brief  菜单界面
 * @param 
 * @return 
 */
static void my_gui_v2(void)
{
    /* 创建UI基础对象 */
    MY_UI2 = lv_obj_create(lv_scr_act());
    lv_obj_set_size(MY_UI2,lcd_x,lcd_y);
    lv_obj_remove_style(MY_UI2, NULL, LV_PART_SCROLLBAR); /* 移除滚动条 */
    
    /* 创建tabview */
    tabview = lv_tabview_create(MY_UI2,LV_DIR_LEFT, lcd_x/4); /* 创建选项卡 */
    lv_obj_set_size(tabview,lcd_x,lcd_y);
    lv_obj_center(tabview);
    u_set_bg_grad(tabview,lv_color_hex(0x000000),\
    lv_color_hex(0x280378),LV_GRAD_DIR_VER,255);
//    lv_obj_set_style_bg_grad_color(tabview,lv_color_hex(0x1A517E),LV_STATE_DEFAULT);
//    lv_obj_set_style_bg_grad_dir(tabview,LV_GRAD_DIR_VER,LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(tabview, &my_font_16, LV_STATE_DEFAULT);/* 设置字体 */
    
//    /* 用于提示选择项的功能 */
//    lv_obj_t *label_def = lv_label_create(MY_UI2);/* 设置文本内容 */
//    lv_obj_set_style_text_font(label_def, &my_font_16, LV_STATE_DEFAULT);/* 设置字体 */
//    lv_obj_set_pos(label_def,120,140);
    
    /* 创建按钮矩阵 */
    tab1 = lv_tabview_add_tab(tabview, "动力"); /* 添加选项卡 1 */
    tab2 = lv_tabview_add_tab(tabview, "信息"); /* 添加选项卡 2 */
    tab3 = lv_tabview_add_tab(tabview, "灯光"); /* 添加选项卡 3 */
    tab4 = lv_tabview_add_tab(tabview, "返回"); /* 添加选项卡 3 */
    
    /* 获取右边容器句柄 */
    content = lv_tabview_get_content(tabview);
    /* 获取按钮矩阵句柄 */
    bg_btn = lv_tabview_get_tab_btns(tabview);
    lv_obj_add_event_cb(bg_btn,back_bg_btn,LV_EVENT_KEY,NULL);
    lv_obj_add_event_cb(bg_btn,back_bg_btn,LV_EVENT_CLICKED,NULL);
    lv_obj_set_style_radius(bg_btn,20,LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bg_btn,75,LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(bg_btn,0,LV_STATE_FOCUS_KEY | LV_PART_ITEMS);
    lv_obj_set_style_bg_color(bg_btn,lv_color_hex(0x2196F3),LV_STATE_FOCUS_KEY | LV_PART_ITEMS);
    lv_obj_set_style_bg_opa(bg_btn,100,LV_STATE_FOCUS_KEY | LV_PART_ITEMS);
    
    /* 去掉动画,只能去官方文件设置lv_tabview_set_act这个参数*/
//    lv_obj_set_style_anim_time(tabview,0,LV_STATE_ANY);

    /* 动力页面 */
    lv_obj_t *label1 = lv_label_create(tab1);/* 设置文本内容 */
    lv_label_set_text(label1, "助力");
    lv_obj_align(label1,LV_ALIGN_TOP_MID,-40,0);
    label1 = lv_label_create(tab1);
    lv_label_set_text(label1, "自动");
    lv_obj_align(label1,LV_ALIGN_TOP_MID,0,0);
    label1 = lv_label_create(tab1);
    lv_label_set_text(label1, "纯电");
    lv_obj_align(label1,LV_ALIGN_TOP_MID,40,0);
    /* 设置滑动条 */
    slider1 = lv_slider_create(tab1);
    slider2 = lv_slider_create(tab1);
    slider3 = lv_slider_create(tab1);
    lv_obj_add_event_cb(slider1,set_cb,LV_EVENT_VALUE_CHANGED,NULL);
    lv_obj_add_event_cb(slider2,set_cb,LV_EVENT_VALUE_CHANGED,NULL);
    lv_obj_add_event_cb(slider3,set_cb,LV_EVENT_VALUE_CHANGED,NULL);
    
    lv_slider_set_range(slider1,40, 100);  /* 转换成百分比 */
    lv_slider_set_range(slider2,0, 25);    /* 转换成百分比 */
    lv_slider_set_range(slider3,40, 100);  /* 转换成百分比 */
    
    lv_obj_set_size(slider1,10,150);
    lv_obj_set_size(slider2,10,150);
    lv_obj_set_size(slider3,10,150);
    lv_obj_align(slider1,LV_ALIGN_CENTER,-40,0);
    lv_obj_align(slider2,LV_ALIGN_CENTER,0,0);
    lv_obj_align(slider3,LV_ALIGN_CENTER,40,0);
    /* 设置滑动条效果 */
    lv_obj_set_style_bg_color(slider1,lv_color_hex(0x2007F0),LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_color(slider1,lv_color_hex(0x2196F3),LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_dir(slider1,LV_GRAD_DIR_VER,LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider1,lv_color_hex(0xFFFFFF),LV_PART_KNOB |LV_STATE_FOCUS_KEY );

    lv_obj_set_style_bg_color(slider2,lv_color_hex(0x2007F0),LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_color(slider2,lv_color_hex(0x2196F3),LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_dir(slider2,LV_GRAD_DIR_VER,LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider2,lv_color_hex(0xFFFFFF),LV_PART_KNOB |LV_STATE_FOCUS_KEY );

    lv_obj_set_style_bg_color(slider3,lv_color_hex(0x2007F0),LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_color(slider3,lv_color_hex(0x2196F3),LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_dir(slider3,LV_GRAD_DIR_VER,LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider3,lv_color_hex(0xFFFFFF),LV_PART_KNOB |LV_STATE_FOCUS_KEY );

    /* 信息界面 */
    tab_meg = lv_table_create(tab2);
    lv_obj_clear_flag(tab_meg,LV_OBJ_FLAG_CLICK_FOCUSABLE);
    /* 他妈的居然有两层背景LV_PART_ITEMS+LV_PART_MAIN */
    lv_obj_set_style_bg_opa(tab_meg, 0, LV_PART_ITEMS);
    lv_obj_set_style_bg_opa(tab_meg, 100, LV_PART_MAIN);
    lv_obj_set_style_border_width(tab_meg,0,LV_PART_ITEMS);
    lv_obj_set_style_border_width(tab_meg,1,LV_PART_MAIN);
    lv_obj_set_style_bg_color(tab_meg,lv_color_hex(0x1A517E),LV_PART_MAIN);
    /* 单元格宽度 */
    lv_table_set_col_width(tab_meg, 0, 80);
    lv_table_set_col_width(tab_meg, 1, 120);
    lv_obj_center(tab_meg);
    lv_table_set_cell_value(tab_meg,0,0,"电压");
    lv_table_set_cell_value(tab_meg,1,0,"温度");
    lv_table_set_cell_value(tab_meg,2,0,"电流");
    lv_table_set_cell_value(tab_meg,3,0,"总里程");
    lv_table_set_cell_value_fmt(tab_meg,0,1,"%d",0);
    lv_table_set_cell_value_fmt(tab_meg,1,1,"%d",0);
    lv_table_set_cell_value_fmt(tab_meg,2,1,"%d",0);
    lv_table_set_cell_value_fmt(tab_meg,3,1,"%d",0);

    /* 自动页面标签 */
    lv_obj_t *BTN1_T = lv_label_create(tab3);
    lv_label_set_text(BTN1_T, "速度控制 \r\n自动灯光 \r\n"); /* 设置文本内容 */
    lv_obj_align(BTN1_T,LV_ALIGN_TOP_LEFT,0,5);
    /* PWM控制开关 */
    switch_pwm = lv_switch_create(tab3);
    lv_obj_set_size(switch_pwm,40,20);
    lv_obj_align(switch_pwm,LV_ALIGN_TOP_RIGHT,-10,5);
    lv_obj_set_style_bg_color(switch_pwm,lv_color_hex(0x2007F0),LV_PART_KNOB |LV_STATE_DEFAULT );
    lv_obj_set_style_bg_color(switch_pwm,lv_color_hex(0xFFFFFF),LV_PART_KNOB |LV_STATE_FOCUS_KEY );
    lv_obj_add_event_cb(switch_pwm,set_cb,LV_EVENT_VALUE_CHANGED,NULL);
    /* 自动灯光开关 */
    switch_light_auto = lv_switch_create(tab3);
    lv_obj_set_size(switch_light_auto,40,20);
    lv_obj_align(switch_light_auto,LV_ALIGN_TOP_RIGHT,-10,35);
    lv_obj_set_style_bg_color(switch_light_auto,lv_color_hex(0x2007F0),LV_PART_KNOB |LV_STATE_DEFAULT );
    lv_obj_set_style_bg_color(switch_light_auto,lv_color_hex(0xFFFFFF),LV_PART_KNOB |LV_STATE_FOCUS_KEY );
    lv_obj_add_event_cb(switch_light_auto,set_cb,LV_EVENT_VALUE_CHANGED,NULL);
    /* 灵敏度圆弧 */
    set_arc = lv_arc_create(tab3);
    lv_obj_set_style_arc_width(set_arc,8,LV_PART_MAIN);
    lv_obj_set_style_arc_width(set_arc,8,LV_PART_INDICATOR);
    lv_arc_set_range(set_arc,0,20);
    lv_obj_align(set_arc,LV_ALIGN_CENTER,0,30);
    lv_obj_set_style_bg_color(set_arc,lv_color_hex(0xFFFFFF),LV_PART_KNOB |LV_STATE_FOCUS_KEY );
    lv_obj_add_event_cb(set_arc,set_cb,LV_EVENT_VALUE_CHANGED,NULL);
    /* 灵敏度变化数值 */
    arc_text = lv_label_create(tab3);
    lv_obj_align_to(arc_text,set_arc,LV_ALIGN_CENTER,0,0);
    lv_obj_set_style_text_font(arc_text,&my_num_24,LV_PART_MAIN);

    BTN1_T = lv_label_create(tab3);
    lv_label_set_text(BTN1_T,"灵敏度");
    lv_obj_align_to(BTN1_T,set_arc,LV_ALIGN_CENTER,0,30);
    
}



void lv_timer_ui1_cb(lv_timer_t *e)
{
    
    static uint32_t time = 0;
    static uint8_t i = 0;
    static uint16_t  battey_num = 0;
    static uint16_t  rpm_angle = 0,speed_angle = 0;
    static uint16_t  rpm_angle_temp = 175,speed_angle_temp = 300;
    
    /* 设置电池 */
    if(time % 20 == 0)
    {
        if(ebike_lvgl.voltage > (EBIKE_VOLTAGE_S * 4000))   //  4.2v - 4.0v =  100%-90%
        {
            battey_num = ((float)(ebike_lvgl.voltage - (EBIKE_VOLTAGE_S * 4000)) / (EBIKE_VOLTAGE_S * 200)) * 10;
            battey_num += 90;
        }else if(ebike_lvgl.voltage > (EBIKE_VOLTAGE_S * 3400))   //  4.0v - 3.4v =  90%-10%
        {
            battey_num = ((float)(ebike_lvgl.voltage - (EBIKE_VOLTAGE_S * 3400)) / (EBIKE_VOLTAGE_S * 600)) * 80;
            battey_num += 10;
        }else if(ebike_lvgl.voltage > (EBIKE_VOLTAGE_S * 3200))      //  3.4v - 3.2v =  10%-0%
        {
            battey_num = ((float)(ebike_lvgl.voltage - (EBIKE_VOLTAGE_S * 3200)) / (EBIKE_VOLTAGE_S * 200)) * 10;
        }else
        {
            battey_num  = 0;
        }
        
        if(battey_num > 100) battey_num = 100;
        lv_label_set_text_fmt(Battery_lab,"%d%%",battey_num);
        lv_bar_set_value(Battery_bar,battey_num, LV_ANIM_OFF);
    }
    
    /* 计算角度 */
    if(time % 5 == 0)
    {
        if(ebike_lvgl.mode == e_mode_motor)
        {
            rpm_angle   = 175 + (ebike_lvgl.get_motor_rpm * (-65.0f / EBIKE_MOTOR_MAXRPM));
        }else
        {
            rpm_angle   = 175 + ((ebike_lvgl.rpm * -65)/100);
        }
        speed_angle = 300 + ((ebike_lvgl.speed * 24)/10);
    }
    
    for(i = 0; i < 5; i++ )    /* 为了角度相差大于i时能快速响应同时又能没误差 */
    {
        if(rpm_angle_temp > rpm_angle)
        {
            rpm_angle_temp--;
        }else if(rpm_angle_temp < rpm_angle){rpm_angle_temp++;}
            
        if(speed_angle_temp > speed_angle)
        {
            speed_angle_temp--;
        }else if(speed_angle_temp < speed_angle){speed_angle_temp++;}
    }
    
    /* 设置转速踏频角度 */
    lv_img_set_angle(pi_img2, rpm_angle_temp * 10);
    if(ebike_lvgl.mode == e_mode_motor)
    {
        lv_label_set_text_fmt(rpm_lab,"%d",ebike_lvgl.get_motor_rpm);
    }else
    {
        lv_label_set_text_fmt(rpm_lab,"%d",ebike_lvgl.rpm);
    }
    
    /* 设置车速角度 */
    lv_img_set_angle(pi_img1, speed_angle_temp * 10);
    lv_label_set_text_fmt(speed_lab,"%d",ebike_lvgl.speed );
    
    
    
    if(time % 100 == 0)    // 间隔10分钟存入eepram
    {
        my_lvgl_cont.mileage = ebike_lvgl.mileage; //复制过来方便存入EEPROM
    }
    
    time++;    //时基计数，时间取决于进入中断的时间间隔
    
}


# define use_anim 0

#if use_anim == 1


void anim_x_cb(void* var, int32_t v)
{
    lv_timer_ui1_cb(NULL);
}

void set_anim(void)
{
    lv_anim_t a;
    /* 第二步：动画初始化 */
    lv_anim_init(&a);
    /* 第三步：设置动画目标为标签对象 */
    lv_anim_set_var(&a, NULL);
    /* 第四步：设置动画起点和终点 */
    lv_anim_set_values(&a, 0, 1);
    /* 第五步：设置动画时间 */
    lv_anim_set_time(&a, 100);
    /* 第六步：设置动画回调函数 */
    lv_anim_set_exec_cb(&a, anim_x_cb);
    /* 第七步：设置动画轨道 */
    lv_anim_set_path_cb(&a, lv_anim_path_linear);;
    /* 重复的数量。默认值为 1。LV_ANIM_REPEAT_INFINITE 用于无限重复 */
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    /* 第八步：开启动画 */
    lv_anim_start(&a);
}
#endif

/**
* @brief  设置人力模式  为电量不够时设置为人力模式
 * @param  
 * @return 
 */
void lv_setleg(void)
{
    
    lv_label_set_text(mode_lab,"LEG");
    ebike_lvgl.mode = e_mode_leg;
    
}



void lv_mainstart(void)
{
    lvgl_set_typedef  *set_temp;
    
    /* 创建按键组 */
    key_group = lv_group_create();
    lv_indev_set_group(indev_keypad, key_group);
    my_gui_v1();
    my_gui_v2();
    lv_group_add_obj(key_group,sport_lab);
    lv_group_add_obj(key_group,mode_lab);
    lv_obj_move_foreground(MY_UI1);
    
    #if use_anim == 1
    /* 看能不能用动画循环的方式 */
    set_anim();
    #else
    /* 用定时器实现 */
    lv_timer_ui1 = lv_timer_create(&lv_timer_ui1_cb,100,NULL);
    #endif
    
    /* 为参数控制设置初始值，有些在eeprom中获取 */
    stmflash_read(FLASH_SAVE_ADDR,eeprom_buf,EEP_SIZE);
    set_temp = (lvgl_set_typedef*)&eeprom_buf[0];
    if(set_temp->eeprom_sta != 12345)
    {
        my_lvgl_cont.sta = 0;
        my_lvgl_cont.pwm_sta = 0;
        my_lvgl_cont.auto_light_sta = 0;
        my_lvgl_cont.light_Threshold = 0;
        my_lvgl_cont.auto_cun  = 20;
        my_lvgl_cont.e_maxcun  =  50;
        my_lvgl_cont.m_maxcun  =  50;
        my_lvgl_cont.sport_num = 50;
        my_lvgl_cont.mode = 1;
        my_lvgl_cont.mileage = 0;
        my_lvgl_cont.size = 2100;
        my_lvgl_cont.sta = 1;
        my_lvgl_cont.eeprom_sta = 12345;      /* 标志下已经写入eeprom */
        stmflash_write(FLASH_SAVE_ADDR, (uint16_t *)&my_lvgl_cont, EEP_SIZE);
        
    }else
    {
        my_lvgl_cont.sta = 0;
        my_lvgl_cont.pwm_sta = set_temp->pwm_sta;
        my_lvgl_cont.auto_light_sta = set_temp->auto_light_sta;
        my_lvgl_cont.light_Threshold = set_temp->light_Threshold;
        my_lvgl_cont.auto_cun  = set_temp->auto_cun;
        my_lvgl_cont.e_maxcun  =  set_temp->e_maxcun;
        my_lvgl_cont.m_maxcun  =  set_temp->m_maxcun;
        my_lvgl_cont.sport_num = set_temp->sport_num;
        my_lvgl_cont.mode = set_temp->mode;
        my_lvgl_cont.mileage = set_temp->mileage;
        my_lvgl_cont.size = set_temp->size;
        my_lvgl_cont.sta = 1;
    }
    
    
    /*************** 根据EEPROM参数设置值 **************************/
    /* 设置滑动条 */
    lv_slider_set_value(slider1,my_lvgl_cont.e_maxcun, LV_ANIM_OFF);
    lv_slider_set_value(slider2,my_lvgl_cont.auto_cun, LV_ANIM_OFF);
    lv_slider_set_value(slider3,my_lvgl_cont.m_maxcun, LV_ANIM_OFF);
    /* 设置运动 */
    switch(my_lvgl_cont.sport_num)
    {
        case 50 :
            lv_label_set_text(sport_lab,"D");
        break;
        
        case 75 :
            lv_label_set_text(sport_lab,"S");
        break;
        
        case 100 :
            lv_label_set_text(sport_lab,"R");
        break;
    }
    
    /* 设置ebike */
    switch(my_lvgl_cont.mode)
    {
        case e_mode_ebike :
        lv_label_set_text(mode_lab,"E-BIKE");
        ebike_lvgl.mode = e_mode_ebike;
        break;
        
        case e_mode_motor :
        lv_label_set_text(mode_lab,"MOTOR");
        ebike_lvgl.mode = e_mode_motor;
        break;
        
        case e_mode_leg :
        lv_label_set_text(mode_lab,"LEG");
        ebike_lvgl.mode = e_mode_leg;
        break;
    }
    ebike_lvgl.mileage = my_lvgl_cont.mileage;
    ebike_lvgl.size = my_lvgl_cont.size;
    
    /* pwm开关自动大灯开关 */
    if(my_lvgl_cont.pwm_sta == 1)
    {
        lv_obj_add_state(switch_pwm,LV_STATE_CHECKED);
    }else
    {
        lv_obj_clear_state(switch_pwm,LV_STATE_CHECKED);
    }
    
    if(my_lvgl_cont.auto_light_sta == 1)
    {
        lv_obj_add_state(switch_light_auto,LV_STATE_CHECKED);
    }else
    {
        lv_obj_clear_state(switch_light_auto,LV_STATE_CHECKED);
    }
    
    /* 自动大灯灵敏度 */
    lv_label_set_text_fmt(arc_text,"%d",my_lvgl_cont.light_Threshold);
    lv_arc_set_value(set_arc,my_lvgl_cont.light_Threshold);
    
    
    my_ebike.e_maxcun = ebike_lvgl.e_maxcun;
    my_ebike.m_maxcun = ebike_lvgl.m_maxcun;
    my_ebike.mode = ebike_lvgl.mode;
    my_ebike.size = ebike_lvgl.size;
    my_ebike.mileage = ebike_lvgl.mileage;
    
}


