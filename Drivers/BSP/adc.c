/**
  ******************************************************************************
  * @file    adc.c
  * @author  MYJ
  * @brief   获取所有的ADC,通过DMA并存入相关结构体中,
             读取时判断DMA是否搬运完成后直接使用.
  @verbatim
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
  1.先初始化:adc_dma_init,使用的时DMA循环模式
  2.判断DMA是否搬运完成后直接使用.
  @endverbatim
  ******************************************************************************
  **/
#include "./BSP/adc.h"
#include "./SYSTEM/delay/delay.h"

/***************************************** 结构体和变量 *************************************************/
DMA_HandleTypeDef g_dma_adc_handle = {0};                                   /* 定义要搬运ADC数据的DMA句柄 */
ADC_HandleTypeDef g_adc_dma_handle = {0};                                   /* 定义ADC（DMA读取）句柄 */

GET_ADC_typedef  EBIKE_GetADC = {0};



/***************************************** 函数部分 *************************************************/
/**
 * @brief       ADC1 DMA 初始化函数
 * @param       mar         : 存储器地址
 * @retval      无
 */
void adc1_dma_init(void)
{
    /*   初始化结构体    */
    GPIO_InitTypeDef gpio_init_struct;
    RCC_PeriphCLKInitTypeDef adc_clk_init = {0};
    ADC_ChannelConfTypeDef adc_ch_conf = {0};
    
    /*      时钟使能     */
    __HAL_RCC_ADC1_CLK_ENABLE();                                          /* 使能ADCx时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();                                         /* 开启GPIO时钟 */
    __HAL_RCC_DMA1_CLK_ENABLE();                                          /* DMA1时钟使能 */

    /* ADC时钟配置 */
    adc_clk_init.PeriphClockSelection = RCC_PERIPHCLK_ADC;                  /* ADC外设时钟 */
    adc_clk_init.AdcClockSelection = RCC_ADCPCLK2_DIV6;                     /* 分频因子6时钟为72M/6=12MHz */
    HAL_RCCEx_PeriphCLKConfig(&adc_clk_init);                               /* 设置ADC时钟 */

    /* IO引脚配置 */
    gpio_init_struct.Pin   = LSEN_PIN | ACC_PIN;
    gpio_init_struct.Mode  = GPIO_MODE_ANALOG;
    gpio_init_struct.Pull  = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);
    
    /* 初始化DMA */
    g_dma_adc_handle.Instance = DMA1_Channel1;                              /* 设置DMA通道 */
    g_dma_adc_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;                 /* 从外设到存储器模式 */
    g_dma_adc_handle.Init.PeriphInc = DMA_PINC_DISABLE;                     /* 外设非增量模式 */
    g_dma_adc_handle.Init.MemInc = DMA_MINC_ENABLE;                         /* 存储器增量模式 */
    g_dma_adc_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;    /* 外设数据长度:16位 */
    g_dma_adc_handle.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;       /* 存储器数据长度:16位 */
    g_dma_adc_handle.Init.Mode = DMA_NORMAL;                                /* 循环模式选择 */
    g_dma_adc_handle.Init.Priority = DMA_PRIORITY_MEDIUM;                   /* 中等优先级 */
    HAL_DMA_Init(&g_dma_adc_handle);
    
    /* 初始化ADC */
    g_adc_dma_handle.Instance = ADC1;                                       /* 选择哪个ADC */
    g_adc_dma_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;                  /* 数据对齐方式：右对齐 */
    g_adc_dma_handle.Init.ScanConvMode = ADC_SCAN_ENABLE;                   /* 是否使用扫描模式 */
    g_adc_dma_handle.Init.ContinuousConvMode = DISABLE;                     /* 是否使能连续转换模式 */
    g_adc_dma_handle.Init.NbrOfConversion = DATA_NUM;                       /* 赋值范围是1~16通道 */
    g_adc_dma_handle.Init.DiscontinuousConvMode = DISABLE;                  /* 间断模式 */
    g_adc_dma_handle.Init.NbrOfDiscConversion = 0;                          /* 配置间断模式通道个数 */
    g_adc_dma_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;            /* 触发转换方式：软件触发 */
    HAL_ADC_Init(&g_adc_dma_handle);                                        /* 初始化 */
    
    /* 配置DMA数据流请求中断优先级 */
//    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 4, 0);
//    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
    __HAL_LINKDMA(&g_adc_dma_handle, DMA_Handle, g_dma_adc_handle);         /* 将DMA与adc联系起来 */

    /* 配置ADC通道 */
    adc_ch_conf.Channel = ADC_CHANNEL_0;                                     /* 通道 */
    adc_ch_conf.Rank = ADC_REGULAR_RANK_1;                                  /* 序列 */
    adc_ch_conf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;                  /* 采样时间，设置最大采样周期:239.5个ADC周期 */
    HAL_ADC_ConfigChannel(&g_adc_dma_handle, &adc_ch_conf);                 /* 通道配置 */
    /* 配置ADC通道 */
    adc_ch_conf.Channel = ADC_CHANNEL_1;                                     /* 通道 */
    adc_ch_conf.Rank = ADC_REGULAR_RANK_2;                                  /* 序列 */
    adc_ch_conf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;                  /* 采样时间，设置最大采样周期:239.5个ADC周期 */
    HAL_ADC_ConfigChannel(&g_adc_dma_handle, &adc_ch_conf);                 /* 通道配置 */

    
    HAL_ADCEx_Calibration_Start(&g_adc_dma_handle);                         /* 校准ADC */
    /* 会频繁中断，影响freertos */
    HAL_ADC_Start_DMA(&g_adc_dma_handle, (uint32_t*)&EBIKE_GetADC, DATA_NUM);                /* 先开启DMA，再开启ADC的DMA，后面两个参数其实没用 */
//    HAL_ADC_Start(&g_adc_dma_handle);
//    HAL_DMA_Start(g_adc_dma_handle.DMA_Handle, (uint32_t)&g_adc_dma_handle.Instance->DR, (uint32_t)&EBIKE_GetADC, DATA_NUM);
}

/**
 * @brief       使能一次ADC转换
 * @note      该函数用寄存器来操作
 * @param       cndtr: DMA传输的次数
 * @retval      1=成功,0=失败
 */
void adc_dma_conver(GET_ADC_typedef *get_adc)
{
    ADC1->CR2 &= ~(1 << 0);                  /* 先关闭ADC */
    DMA1_Channel1->CCR &= ~(1 << 0);         /* 关闭DMA通道 */
    while (DMA1_Channel1->CCR & (1 << 0)); /* 确保 DMA 可以被设置 */
    DMA1_Channel1->CNDTR = DATA_NUM;         /* 设置传输数量,需要先关闭DMA */
    DMA1_Channel1->CCR |= 1 << 0;            /* 非循环模式,传输数量为0时,开启通道也不会启动传输 */
    ADC1->CR2 |= 1 << 0;                    /* 重新启动ADC */
    
    ADC1->CR2 |= 1 << 22;                   /* 启动规则转换通道 */

    while((DMA1->ISR & (1 << 1)) == RESET)      /* 等待DMA1_CHANNEL1 传输完成 */
    DMA1->IFCR |= 1 << 1;                     /* 清除DMA1 CHANNEL1完成中断 */
    
    get_adc->ACC_cun = EBIKE_GetADC.ACC_cun;
    get_adc->LSEN_cun = EBIKE_GetADC.LSEN_cun;
}




