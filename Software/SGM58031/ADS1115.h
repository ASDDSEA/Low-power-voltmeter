#ifndef _ADS_1115_H_
#define _ADS_1115_H_

#include "main.h"
#define int16_t short 
#define uint8_t char 
#define int32_t int 
//      PA2     :       IIC_SCL,���ڽ���ADS1115����
//      PA3     :       IIC_SDA,���ڽ���ADS1115����

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
//#define AD_drdy       GPIOA->IDR & (uint8_t)GPIO_Pin_5  //AD�����ź�
//����ΪADS1115�ܽ�Ӳ������
#define         FALSE 0
#define         TRUE 1

//////////////////////////////////////////////////////////////
void    I2C_Delay(void);				//��ʱ
void    delay5ms(void);				//��ʱ5ms
void    I2C_Start(void);				//����I2C��ʼ�ź�
void    I2C_Stop(void);	  			//����I2Cֹͣ�ź�
void    I2C_SendByte(uint8_t txd);			//I2C����һ���ֽ�
uint8_t I2C_ReadByte(void);//I2C��ȡһ���ֽ�
//int Single_Write(uchar SlaveAddress, uchar REG_Address,uchar REG_data);
uint8_t Single_Read(unsigned char SlaveAddress, unsigned char REG_Address);
int     I2C_Wait_Ack(void); 				//I2C�ȴ�ACK�ź�
void    I2C_Ack(void);					//I2C����ACK�ź�
void    I2C_NAck(void);				//I2C������ACK�ź�
void    ads1115_init(void);    //��ʼ��ADS1115
void    startch0(uint8_t ch);
int16_t ads1115_ch0(void); //ADS1115����


#endif

