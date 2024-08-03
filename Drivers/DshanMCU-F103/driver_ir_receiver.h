// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright (c) 2008-2023 100askTeam : Dongshan WEI <weidongshan@qq.com> 
 * Discourse:  https://forums.100ask.net
 */

 
/*  Copyright (C) 2008-2023 深圳百问网科技有限公司
 *  All rights reserved
 *
 *
 * 免责声明: 百问网编写的文档，仅供学员学习使用，可以转发或引用(请保留作者信息)，禁止用于商业用途！
 * 免责声明: 百问网编写的程序，可以用于商业用途，但百问网不承担任何后果！
 * 
 * 摘要：
 *			本程序遵循GPL V3协议，使用请遵循协议许可
 *			本程序所用的开发板：	DShanMCU-F103
 *			百问网嵌入式学习平台：https://www.100ask.net
 *			百问网技术交流社区：	https://forums.100ask.net
 *			百问网官方B站：				https://space.bilibili.com/275908810
 *			百问网官方淘宝：			https://100ask.taobao.com
 *			联系我们(E-mail)：	  weidongshan@qq.com
 *
 *			版权所有，盗版必究。
 *  
 * 修改历史     版本号           作者        修改内容
 *-----------------------------------------------------
 * 2023.08.04      v01         百问科技      创建文件
 *-----------------------------------------------------
*/
#ifndef _DRIVER_IR_RECEIVER_H
#define _DRIVER_IR_RECEIVER_H

#include <stdint.h>

#include "FreeRTOS.h"
#include "queue.h"

#define IR_QUEUE_LEN 10

#define IR_KEY_POWER  0xa2
#define IR_KEY_MENU   0xe2
#define IR_KEY_TEST   0x22
#define IR_KEY_ADD    0x02
#define IR_KEY_RETURN 0xc2
#define IR_KEY_LEFT   0xe0
#define IR_KEY_PLAY   0xa8
#define IR_KEY_RIGHT  0x90
#define IR_KEY_0      0x68
#define IR_KEY_DEC    0x98
#define IR_KEY_C      0xb0
#define IR_KEY_1      0x30
#define IR_KEY_2      0x18
#define IR_KEY_3      0x7a
#define IR_KEY_4      0x10
#define IR_KEY_5      0x38
#define IR_KEY_6      0x5a
#define IR_KEY_7      0x42
#define IR_KEY_8      0x4a
#define IR_KEY_9      0x52
#define IR_KEY_REPEAT 0x00

struct ir_data {
	uint32_t dev;
	uint32_t val;
};

/**********************************************************************
 * 函数名称： GetQueueIR
 * 功能描述： 返回红外驱动程序里的队列句柄
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 队列句柄
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/09/02	     V1.0	  韦东山	      创建
 ***********************************************************************/
QueueHandle_t GetQueueIR(void);

/**********************************************************************
 * 函数名称： IRReceiver_Init
 * 功能描述： 红外接收器的初始化函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void IRReceiver_Init(void);

/**********************************************************************
 * 函数名称： IRReceiver_Read
 * 功能描述： 红外接收器的读取函数
 * 输入参数： 无
 * 输出参数： pDev  - 用来保存设备ID
 *            pData - 用来保存按键码
 * 返 回 值： 0 - 成功, (-1) - 失败(无数据)
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
int IRReceiver_Read(uint8_t *pDev, uint8_t *pData);

/**********************************************************************
 * 函数名称： IRReceiver_CodeToString
 * 功能描述： 把接收到的按键码转换为按键名字
 * 输入参数： code - 按键码
 * 输出参数： 无
 * 返 回 值： NULL - 未识别的按键码; 其他值 - 按键名字
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
const char *IRReceiver_CodeToString(uint8_t code);


/**********************************************************************
 * 函数名称： IRReceiver_Test
 * 功能描述： 红外接收器测试程序
 * 输入参数： 无
 * 输出参数： 无
 *            无
 * 返 回 值： 无
 * 修改日期        版本号     修改人        修改内容
 * -----------------------------------------------
 * 2023/08/04        V1.0     韦东山       创建
 ***********************************************************************/
void IRReceiver_Test(void);


#endif /* _DRIVER_IR_RECEIVER_H */

