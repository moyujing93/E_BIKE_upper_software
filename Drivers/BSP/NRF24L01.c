#include "./BSP/NRF24L01.h"
#include "./SYSTEM/delay/delay.h"




/*          дһ���ֽ�       */
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
/*          ��һ���ֽ�       */
static uint8_t spi_read_byte(void)
{
    uint8_t i,data = 0;
    
    NRF_MOSI_DO(1);     //���뱣֤MOSIΪ1,��Ȼ��ִ��R_REGISTER����
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
  * @brief  дNRF�Ĵ���
  * @param  �Ĵ���λ������
  * @param  ���ݳ���,���5��.��λbyte,����Ҫ����=0
  * @param  A,B,C,D,EΪ��Ҫ��������ݵ�ֵ
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
  * @brief  ��NRF�Ĵ���
  * @param  �Ĵ���λ������
  * @param  ���ݳ���,���32byte.
  * @param  ָ�����
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


/*          ��ʼ������,����NRF�Ĵ���       */
uint8_t NRF24L01_init(uint8_t mode)
{
    uint8_t STA = 0;
    /*          ��ʼ����������              */
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
    /*          ����NRF�Ĵ���              */
    NRF_CE_DO(0);
    
    if (mode == 1)
    {
        NRF_write_bufer(W_REGISTER+0x00,1,0x0e,0,0,0,0);      //����ģʽ,CRC2byte,pow_up,IRQ�ж�ȫ��
//        NRF_write_bufer(W_REGISTER+0x02,1,0x01,0,0,0,0);               //ʹ�ܽ���ͨ��0
        NRF_write_bufer(W_REGISTER+0x05,1,20,0,0,0,0);               //������Ƶͨ��,���ͺͽ���Ҫһ��
//        NRF_write_bufer(W_REGISTER+0x06,1,0x0e,0,0,0,0);             //���ݴ����� 2Mbps ������
//        NRF_write_bufer(W_REGISTER+0x0A,5,0X11,0X22,0X33,0X44,0X01);   //���ý���ͨ��P0�ĵ�ַ
//        NRF_write_bufer(W_REGISTER+0x10,5,0X11,0X22,0X33,0X44,0X01);   //���÷��͵�ַ
//        NRF_write_bufer(W_REGISTER+0x11,1,32,0,0,0,0);               //����P0���������ֽڳ���1-32
        NRF_write_bufer(W_REGISTER+0x1C,1,0X01,0,0,0,0);               //ʹ�ܽ��չܵ�0��̬���س���
        NRF_write_bufer(W_REGISTER+0x1D,1,0X04,0,0,0,0);               //ʹ�ܶ�̬���س���
    }
    else
    {
        NRF_write_bufer(W_REGISTER+0x00,1,0x0f,0,0,0,0);          //����ģʽ,CRC2byte,pow_up,IRQ�ж�ȫ��
//        NRF_write_bufer(W_REGISTER+0x01,1,0x01,0,0,0,0);               //ʹ�ܽ���ͨ��0�Զ�ȷ��
//        NRF_write_bufer(W_REGISTER+0x02,1,0x01,0,0,0,0);               //ʹ�ܽ���ͨ��0
        NRF_write_bufer(W_REGISTER+0x05,1,20,0,0,0,0);                 //������Ƶͨ��,���ͺͽ���Ҫһ��
//        NRF_write_bufer(W_REGISTER+0x06,1,0x0e,0,0,0,0);             //���ݴ����� 2Mbps ������
//        NRF_write_bufer(W_REGISTER+0x0A,5,0X11,0X22,0X33,0X44,0X01);   //���ý���ͨ��P0�ĵ�ַ
//        NRF_write_bufer(W_REGISTER+0x11,1,32,0,0,0,0);               //����P0���������ֽڳ���1-32
        NRF_write_bufer(W_REGISTER+0x1C,1,0X01,0,0,0,0);               //ʹ�ܽ��չܵ�0��̬���س���
        NRF_write_bufer(W_REGISTER+0x1D,1,0X04,0,0,0,0);               //ʹ�ܶ�̬���س���
    }
    
    NRF_CE_DO(1);
    delay_ms(2);
    NRF_read_bufer(R_REGISTER+0x05,1,&STA);   //��ȡ��Ƶͨ���ж��Ƿ�����д�Ĵ���
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
  * @brief  ͨ��NRF��������
  * @param  ���ݵ�ַ
  * @param  ���ݳ���,��λbyte
  * @retval 1=���ͳɹ�,0=����ʧ��
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
    
    while(HAL_GPIO_ReadPin(NRF_IRQ_PORT,NRF_IRQ_PIN) == 1);  //������ɻ�ﵽ����ʹ���IRQ�ᱻ����
    NRF_CE_DO(0);
    NRF_write_bufer(FLUSH_TX,0,0,0,0,0,0);  //���FIFO��ֵ,��ֹû���ͳɹ������ط����ݱ�����
    NRF_read_bufer(R_REGISTER+STATUS,1,&STA);  //��ȡ״̬�Ĵ�����ֵ.������Բ�дREG�͵õ�״̬�Ĵ�����ֵ��
    NRF_write_bufer(W_REGISTER+STATUS,1,STA,0,0,0,0);  //���״̬�Ĵ���.
    if(STA & 0x20)       //0x20 == ���ͳɹ�.
    {
        return 1;
    }
    return 0;
}

/**
  * @brief  ��ȡNRF���յ�������
  * @param  ���ݴ��λ��,
  * @retval ���յ��˶��ٸ��ֽ�
  */
uint8_t NRF_get_byte(uint8_t *data)
{
    uint8_t STA = 0,length = 0;
    NRF_read_bufer(R_REGISTER+STATUS,1,&STA);  //��ȡ״̬�Ĵ�����ֵ
    if(STA & 0x40)                  //���RX_FIFO������
    {
        length = 1;
        NRF_read_bufer(R_RX_PAYLOAD,32,data);
        NRF_write_bufer(FLUSH_RX,0,0,0,0,0,0);  //���FIFO��ֵ
        NRF_write_bufer(W_REGISTER+STATUS,1,STA,0,0,0,0);  //���״̬�Ĵ���.
    }
    
    return length;
}
