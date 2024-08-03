 /*===============================================================================
              ##### C8T6 #####
 ===============================================================================
C8T6
LED=PC13 没有TIM,低电平点亮
PB4+AFIO=TIM3_CH1
用PB4初始化然后用线连接PC13

 ===============================================================================*/
 
 #include "timer_pwm.h"

TIM_HandleTypeDef pwm_init_hander;
uint16_t ccr_get;

void timer_pwm_init(uint16_t arr)
{
    ccr_get = arr;
    pwm_init_hander.Instance = TIM3;
    pwm_init_hander.Init.Prescaler = 7200-1;
    pwm_init_hander.Init.Period = arr - 1;
    pwm_init_hander.Init.CounterMode = TIM_COUNTERMODE_UP;
    HAL_TIM_PWM_Init(&pwm_init_hander);
    
    TIM_OC_InitTypeDef pwm_conf_hander;
    pwm_conf_hander.OCMode = TIM_OCMODE_PWM1;
    pwm_conf_hander.Pulse = arr/2;
    pwm_conf_hander.OCPolarity = TIM_OCPOLARITY_LOW;
    
    HAL_TIM_PWM_ConfigChannel(&pwm_init_hander,&pwm_conf_hander,TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&pwm_init_hander,TIM_CHANNEL_2);
    
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef  led_struct;
    led_struct.Mode = GPIO_MODE_AF_PP;
    led_struct.Pin = GPIO_PIN_5;
    led_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB,&led_struct);
    
    __HAL_RCC_TIM3_CLK_ENABLE();
    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_AFIO_REMAP_TIM3_PARTIAL();
}
void pwm_setcompare(uint16_t duty)
{
    __HAL_TIM_SET_COMPARE(&pwm_init_hander,TIM_CHANNEL_2,ccr_get*(duty/100.00));
}
