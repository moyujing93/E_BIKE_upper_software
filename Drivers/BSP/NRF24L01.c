#include "./BSP/NRF24L01.h"
#include "./SYSTEM/delay/delay.h"




/*          写一个字节       */
static void spi_write_byte(uint8_t data)
{
    uint8_t i;
    NRF_CLK_DO(0);
    for(i=0;i<8;i++)
    {
        if(data & 0x80)
        {
            NRF_MOSI_DO(1);
        }else
        {
            NRF_MOSI_DO(0);
        }
        spi_delay();
        NRF_CLK_DO(1);
        spi_delay();
        NRF_CLK_DO(0);
        data <<= 1;
    }
}
/*          读一个字节       */
static uint8_t spi_read_byte(void)
{
    uint8_t i,data = 0;
    
    NRF_MOSI_DO(1);     //必须保证MOSI为1,不然会执行R_REGISTER命令
    NRF_CLK_DO(0);
    for(i=0;i<8;i++)
    {
        spi_delay();
        NRF_CLK_DO(1);
        data <<= 1;
        data |= HAL_GPIO_ReadPin(NRF_MISO_PORT,NRF_MISO_PIN);
        spi_delay();
        NRF_CLK_DO(0);
    }
    return data;
}

/**
  * @brief  写NRF寄存器
  * @param  寄存器位置命令
  * @param  数据长度,最大5个.单位byte,不需要数据=0
  * @param  A,B,C,D,E为需要传输的数据的值
  * @retval 
  */
static void NRF_write_bufer(uint8_t cmd,uint8_t length,uint8_t a,uint8_t b,uint8_t c,uint8_t d,uint8_t e)
{
    
    uint8_t i;
    uint8_t temp[5] = {0xff};
    temp[0] = a;
    temp[1] = b;
    temp[2] = c;
    temp[3] = d;
    temp[4] = e;
    
    NRF_CS_DO(0);
    spi_write_byte(cmd);
    for(i=0;i<length;i++)
    {
        spi_write_byte(temp[i]);
    }
    NRF_CS_DO(1);
}
/**
  * @brief  读NRF寄存器
  * @param  寄存器位置命令
  * @param  数据长度,最大32byte.
  * @param  指针入口
  * @retval 
  */
static void NRF_read_bufer(uint8_t cmd,uint8_t length,uint8_t *p)
{
    uint8_t i;
    NRF_CS_DO(0);
    spi_write_byte(cmd);
    for(i=0;i<length;i++)
    {
        *p = spi_read_byte();
        p++;
    }
    NRF_CS_DO(1);
}


/*          初始化引脚,配置NRF寄存器       */
uint8_t NRF24L01_init(uint8_t mode)
{
    uint8_t STA = 0;
    /*          初始化连接引脚              */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef gpio_typedef;
    gpio_typedef.Mode   =  GPIO_MODE_OUTPUT_PP;
    gpio_typedef.Pin    =  NRF_CE_PIN | NRF_CLK_PIN | NRF_CS_PIN | NRF_MOSI_PIN;
    gpio_typedef.Pull   =  GPIO_PULLUP;
    gpio_typedef.Speed  =  GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOC,&gpio_typedef);
    gpio_typedef.Pin    =  NRF_IRQ_PIN | NRF_MISO_PIN;
    gpio_typedef.Mode   =  GPIO_MODE_INPUT;
    HAL_GPIO_Init(GPIOC,&gpio_typedef);
    /*          配置NRF寄存器              */
    NRF_CE_DO(0);
    
    if (mode == 1)
    {
        NRF_write_bufer(W_REGISTER+0x00,1,0x0e,0,0,0,0);      //发送模式,CRC2byte,pow_up,IRQ中断全开
//        NRF_write_bufer(W_REGISTER+0x02,1,0x01,0,0,0,0);               //使能接收通道0
        NRF_write_bufer(W_REGISTER+0x05,1,20,0,0,0,0);               //设置射频通道,发送和接收要一样
//        NRF_write_bufer(W_REGISTER+0x06,1,0x0e,0,0,0,0);             //数据传输率 2Mbps 及功率
//        NRF_write_bufer(W_REGISTER+0x0A,5,0X11,0X22,0X33,0X44,0X01);   //设置接收通道P0的地址
//        NRF_write_bufer(W_REGISTER+0x10,5,0X11,0X22,0X33,0X44,0X01);   //设置发送地址
//        NRF_write_bufer(W_REGISTER+0x11,1,32,0,0,0,0);               //设置P0缓存区的字节长度1-32
        NRF_write_bufer(W_REGISTER+0x1C,1,0X01,0,0,0,0);               //使能接收管道0动态负载长度
        NRF_write_bufer(W_REGISTER+0x1D,1,0X04,0,0,0,0);               //使能动态负载长度
    }
    else
    {
        NRF_write_bufer(W_REGISTER+0x00,1,0x0f,0,0,0,0);          //接收模式,CRC2byte,pow_up,IRQ中断全开
//        NRF_write_bufer(W_REGISTER+0x01,1,0x01,0,0,0,0);               //使能接收通道0自动确认
//        NRF_write_bufer(W_REGISTER+0x02,1,0x01,0,0,0,0);               //使能接收通道0
        NRF_write_bufer(W_REGISTER+0x05,1,20,0,0,0,0);                 //设置射频通道,发送和接收要一样
//        NRF_write_bufer(W_REGISTER+0x06,1,0x0e,0,0,0,0);             //数据传输率 2Mbps 及功率
//        NRF_write_bufer(W_REGISTER+0x0A,5,0X11,0X22,0X33,0X44,0X01);   //设置接收通道P0的地址
//        NRF_write_bufer(W_REGISTER+0x11,1,32,0,0,0,0);               //设置P0缓存区的字节长度1-32
        NRF_write_bufer(W_REGISTER+0x1C,1,0X01,0,0,0,0);               //使能接收管道0动态负载长度
        NRF_write_bufer(W_REGISTER+0x1D,1,0X04,0,0,0,0);               //使能动态负载长度
    }
    
    NRF_CE_DO(1);
    delay_ms(2);
    NRF_read_bufer(R_REGISTER+0x05,1,&STA);   //读取射频通道判断是否正常写寄存器
    if(STA == 20)
    {
        STA = 1;
    }
    else
    {
        STA = 0;
    }
    return STA;
}

/**
  * @brief  通过NRF发送数据
  * @param  数据地址
  * @param  数据长度,单位byte
  * @retval 1=发送成功,0=发送失败
  */
uint8_t NRF_send_byte(uint8_t *data,uint8_t length)
{

    uint8_t i,STA;
    
    NRF_CE_DO(0);
    NRF_CS_DO(0);
    spi_write_byte(W_TX_PAYLOAD);
    for(i=0;i<length;i++)
    {
        spi_write_byte(*data);
        data++;
    }
    NRF_CS_DO(1);
    NRF_CE_DO(1);
    
    while(HAL_GPIO_ReadPin(NRF_IRQ_PORT,NRF_IRQ_PIN) == 1);  //发送完成或达到最大发送次数IRQ会被拉低
    NRF_CE_DO(0);
    NRF_write_bufer(FLUSH_TX,0,0,0,0,0,0);  //清空FIFO的值,防止没发送成功后面重发数据被叠加
    NRF_read_bufer(R_REGISTER+STATUS,1,&STA);  //读取状态寄存器的值.这里可以不写REG就得到状态寄存器的值。
    NRF_write_bufer(W_REGISTER+STATUS,1,STA,0,0,0,0);  //清空状态寄存器.
    if(STA & 0x20)       //0x20 == 发送成功.
    {
        return 1;
    }
    return 0;
}

/**
  * @brief  读取NRF接收到的数据
  * @param  数据存放位置,
  * @retval 接收到了多少个字节
  */
uint8_t NRF_get_byte(uint8_t *data)
{
    uint8_t STA = 0,length = 0;
    NRF_read_bufer(R_REGISTER+STATUS,1,&STA);  //读取状态寄存器的值
    if(STA & 0x40)                  //如果RX_FIFO有数据
    {
        length = 1;
        NRF_read_bufer(R_RX_PAYLOAD,32,data);
        NRF_write_bufer(FLUSH_RX,0,0,0,0,0,0);  //清空FIFO的值
        NRF_write_bufer(W_REGISTER+STATUS,1,STA,0,0,0,0);  //清空状态寄存器.
    }
    
    return length;
}
