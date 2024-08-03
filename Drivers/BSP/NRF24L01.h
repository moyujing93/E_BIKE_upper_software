#ifndef __NRF24L01_H
#define __NRF24L01_H
#include "./SYSTEM/sys/sys.h"

#define   NRF_TX    0           //  1为发送模式，0为接收模式
#define   spi_delay()         do{ delay_us(1); }while(0)
/*                  定义命令         */
#define   R_REGISTER         0x00
#define   W_REGISTER         0x20
#define   R_RX_PAYLOAD       0x61
#define   W_TX_PAYLOAD       0xA0
#define   FLUSH_TX           0xE1
#define   FLUSH_RX           0xE2
#define   R_RX_PL_WID        0x60    //读取收到的数据字节数

#define   STATUS             0x07
/*                  定义引脚链接         */
#define   NRF_IRQ_PIN        GPIO_PIN_9
#define   NRF_IRQ_PORT       GPIOC
#define   NRF_CE_PIN         GPIO_PIN_4
#define   NRF_CE_PORT        GPIOC
#define   NRF_CLK_PIN        GPIO_PIN_6
#define   NRF_CLK_PORT       GPIOC
#define   NRF_MISO_PIN       GPIO_PIN_8
#define   NRF_MISO_PORT      GPIOC
#define   NRF_MOSI_PIN       GPIO_PIN_7
#define   NRF_MOSI_PORT      GPIOC
#define   NRF_CS_PIN         GPIO_PIN_5
#define   NRF_CS_PORT        GPIOC

//#define   NRF_CS_DO(x)    do{x?  (NRF_CS_PORT->BSRR = NRF_CS_PIN) : \
//                                 (NRF_CS_PORT->BSRR = (uint32_t)NRF_CS_PIN << 16);}while(0)
//#define   NRF_CE_DO(x)    do{x?  (NRF_CE_PORT->BSRR = NRF_CE_PIN) : \
//                                 (NRF_CE_PORT->BSRR = (uint32_t)NRF_CE_PIN << 16);}while(0)
//#define   NRF_CLK_DO(x)    do{x?  (NRF_CLK_PORT->BSRR = NRF_CLK_PIN) : \
//                                  (NRF_CLK_PORT->BSRR = (uint32_t)NRF_CLK_PIN << 16);}while(0)
//#define   NRF_MOSI_DO(x)    do{x?  (NRF_MOSI_PORT->BSRR = NRF_MOSI_PIN) : \
//                                   (NRF_MOSI_PORT->BSRR = (uint32_t)NRF_MOSI_PIN << 16);}while(0)

#define   NRF_CS_DO(x)      do{x?  HAL_GPIO_WritePin(NRF_CS_PORT,NRF_CS_PIN,GPIO_PIN_SET) : \
                                HAL_GPIO_WritePin(NRF_CS_PORT,NRF_CS_PIN,GPIO_PIN_RESET);}while(0)
#define   NRF_CE_DO(x)      do{x?  HAL_GPIO_WritePin(NRF_CE_PORT,NRF_CE_PIN,GPIO_PIN_SET) : \
                                HAL_GPIO_WritePin(NRF_CE_PORT,NRF_CE_PIN,GPIO_PIN_RESET);}while(0)
#define   NRF_CLK_DO(x)     do{x?  HAL_GPIO_WritePin(NRF_CLK_PORT,NRF_CLK_PIN,GPIO_PIN_SET) : \
                                HAL_GPIO_WritePin(NRF_CLK_PORT,NRF_CLK_PIN,GPIO_PIN_RESET);}while(0)
#define   NRF_MOSI_DO(x)    do{x?  HAL_GPIO_WritePin(NRF_MOSI_PORT,NRF_MOSI_PIN,GPIO_PIN_SET) : \
                                HAL_GPIO_WritePin(NRF_MOSI_PORT,NRF_MOSI_PIN,GPIO_PIN_RESET);}while(0)

uint8_t NRF24L01_init(uint8_t mode);
uint8_t NRF_send_byte(uint8_t *data,uint8_t length);
uint8_t NRF_get_byte(uint8_t *data);









#endif
