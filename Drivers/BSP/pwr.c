/**
  ******************************************************************************
  * @author  MYJ
  * @brief   电源低电压时断电时，把数据写入eeprom中
             
  @verbatim
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
  
  @endverbatim
  ******************************************************************************
  **/
  
#include "./BSP/pwr.h"
#include "my_gui.h"
#include "./BSP/stmflash.h"
#include "stm32f1xx_hal_pwr.h"



/**
 * @brief       使能PWR电压监控在中断函数中保存数据
 * @note      
 * @param       
 * @retval      
 */
void my_pwrinit(void)
{
    PWR_PVDTypeDef  my_pwr = {0};
    
    __HAL_RCC_PWR_CLK_ENABLE();
    my_pwr.Mode      =   PWR_PVD_MODE_IT_FALLING;
    my_pwr.PVDLevel  =   PWR_CR_PLS_2V9;
    HAL_PWR_ConfigPVD(&my_pwr);
    
    HAL_NVIC_SetPriority(PVD_IRQn, 2 ,0);
    HAL_NVIC_EnableIRQ(PVD_IRQn);
    HAL_PWR_EnablePVD();
    
}

/**
* @brief PVD 中断服务函数
* @param 无
* @retval 无
*/
void PVD_IRQHandler(void)
{
 HAL_PWR_PVD_IRQHandler();
}
/**
* @brief PVD 中断服务回调函数
* @param 无
* @retval 无
*/
void HAL_PWR_PVDCallback(void)
{
    /* 把数据存入flash模拟的EEPROM中 */
//    my_lvgl_cont.eeprom_sta = 12345;      /* 标志下已经写入eeprom */
//    stmflash_fastwrite(FLASH_SAVE_ADDR, (uint16_t *)&my_lvgl_cont, EEP_SIZE);
}


