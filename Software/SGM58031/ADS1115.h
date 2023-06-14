#ifndef _ADS_1115_H_
#define _ADS_1115_H_

#include "main.h"
#define int16_t short 
#define uint8_t char 
#define int32_t int 
//      PA2     :       IIC_SCL,用于接收ADS1115数据
//      PA3     :       IIC_SDA,用于接收ADS1115数据

//#define SDA_INIT	GPIO_Init(GPIOA,GPIO_Pin_3,GPIO_Mode_Out_PP_Low_Slow)
//#define SCL_INIT	GPIO_Init(GPIOA,GPIO_Pin_2,GPIO_Mode_Out_PP_Low_Slow)

#define SDA_INIT	GPIO_output_init('A',3)

#define SCL_INIT	GPIO_output_init('A',2)
#define SDA_IN()        	GPIO_input_init('A',3)
#define SDA_OUT() 	GPIO_output_init('A',3)



#define SCL_H           PA_ODR|=0X04
#define SCL_L           PA_ODR&=~0X04
#define SDA_H           PA_ODR|=0X08
#define SDA_L           PA_ODR&=~0X08
#define SDA_read        (PA_IDR&0X08)==0x08 
//#define AD_drdy       GPIOA->IDR & (uint8_t)GPIO_Pin_5  //AD结束信号
//以上为ADS1115管脚硬件配置
#define         FALSE 0
#define         TRUE 1

//////////////////////////////////////////////////////////////
void    I2C_Delay(void);				//延时
void    delay5ms(void);				//延时5ms
void    I2C_Start(void);				//发送I2C开始信号
void    I2C_Stop(void);	  			//发送I2C停止信号
void    I2C_SendByte(uint8_t txd);			//I2C发送一个字节
uint8_t I2C_ReadByte(void);//I2C读取一个字节
//int Single_Write(uchar SlaveAddress, uchar REG_Address,uchar REG_data);
uint8_t Single_Read(unsigned char SlaveAddress, unsigned char REG_Address);
int     I2C_Wait_Ack(void); 				//I2C等待ACK信号
void    I2C_Ack(void);					//I2C发送ACK信号
void    I2C_NAck(void);				//I2C不发送ACK信号
void    ads1115_init(void);    //初始化ADS1115
void    startch0(uint8_t ch);
int16_t ads1115_ch0(void); //ADS1115采样


#endif

