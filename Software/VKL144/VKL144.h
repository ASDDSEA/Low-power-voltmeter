#ifndef __VKL144_H__
#define __VKL144_H__

//此包含文件根据客户单片机做相应的修改
#include "main.h"
#define uint8_t                         char
#define uint16_t                        short 
#define uint32_t                        int 

//基本设置
#define VKL144_ADDR   			0x7c    // IIC地址
#define VKL144_ICSET       		0xe8    // IC设置  bit0时钟源  bit1软复位	bit2映射地址bit5
										// 默认为0 内部RC晶振  不执行软件复位  
#define BIT0_CLKEXT       		0x01    // 外部时钟
#define BIT1_ENSRST       		0x02    // 执行软件复位
#define BIT2_ADDR5	       		0x04    // 映射地址bit5
#define VKL144_ADSET       		0x00    // 映射起始地址bit4~bit0
#define VKL144_MODESET_1_3_ON  	0xc8    // 打开显示 1/3bias
#define VKL144_MODESET_1_3_OFF 	0xc0    // 关闭显示 1/3bias
#define VKL144_MODESET_1_2_ON  	0xcc    // 打开显示 1/2bias
#define VKL144_MODESET_1_2_OFF 	0xc4    // 关闭显示 1/2bias

//显示控制命令
#define VKL144_DISCTL			0xA0  	//bit7~bit5=101

//bit4-bit3省电模式FR
#define VKL144_FRNOR       		0x00  	//bit4-bit3=00   FR NORMAL 				上电默认 
#define VKL144_FRPM1       		0x08  	//bit4-bit3=01   FR POWER SAVE MODE1
#define VKL144_FRPM2       		0x10  	//bit4-bit3=10   FR POWER SAVE MODE2
#define VKL144_FRPM3       		0x18  	//bit4-bit3=11   FR POWER SAVE MODE3 	最省电

//bit2 LINE,FRAME驱动波形LINE 翻转时的工作电流>FRAME翻转的电流
#define VKL144_LINER       		0x00  	//bit2=0   LINE翻转	 					上电默认
#define VKL144_FRAMER      		0x04  	//bit2=1   FRAME翻转 					最省电

//bit1-bit0省电模式SR
#define VKL144_SRHP        		0x03  	//bit1-bit0=11   SR NORMAL 				最大
#define VKL144_SRNOR       		0x02  	//bit1-bit0=10   SR POWER SAVE MODE1 	上电默认 
#define VKL144_SRPM2       		0x01  	//bit1-bit0=01   SR POWER SAVE MODE2
#define VKL144_SRPM1       		0x00  	//bit1-bit0=00   SR POWER SAVE MODE1 	最省电

//常用显示控制组合
//VKL144_DISCTL|VKL144_FRNOR|VKL144_LINER |VKL144_SRHP      //电流最大
//VKL144_DISCTL|VKL144_FRPM1|VKL144_FRAMER|VKL144_FRAMER    //电流典型
//VKL144_DISCTL|VKL144_FRPM3|VKL144_FRAMER|VKL144_SRPM1    	//电流最省

//工作电流设置
#define	VKL144_DISCTL_DATA		VKL144_DISCTL | VKL144_FRPM3 | VKL144_FRAMER | VKL144_SRPM1

//闪烁控制
#define VKL144_BLKCTL_OFF      	0xF0  						// 关闭闪烁
#define VKL144_BLKCTL_05HZ     	0xF1  						// 闪烁频率为0.5HZ
#define VKL144_BLKCTL_1HZ      	0xF2  						// 闪烁频率为1HZ
#define VKL144_BLKCTL_2HZ      	0xF3  						// 闪烁频率为2HZ

//全屏强行开或关,与显示内存内容无关,两个功能中全屏关为优先执行
#define VKL144_APCTL_APON_ON   	0xFE 						// 全屏强行全显示_开
#define VKL144_APCTL_APON_OFF  	0xFC 						// 全屏强行全显示_关
#define VKL144_APCTL_APOFF_ON  	0xFD 						// 全屏强行关显示_开
#define VKL144_APCTL_APOFF_OFF 	0xFC 						// 全屏强行关显示_关

//驱动seg数
#define 	VKL144_SEGNUM		16
#define 	VKL144_RAMBUFNUM	VKL144_SEGNUM/2				//偶数除2，奇数除2+1

//1/3bias		1/2bias		1=有效		0=无效
#define		BIAS_1_2		        0
#define		BIAS_1_3			1


//      PB3     :       DIS_SDA,不用LCD显示
//      PB4     :       DIS_SCL,不用LCD显示
//以下管脚配置根据客户单片机做相应的修改
#define VKL144_I2C_PORT         GPIOB
#define VKL144_I2C_SDA_PIN      GPIO_Pin_3
#define VKL144_I2C_SCL_PIN      GPIO_Pin_4
#define VKL144_SDA_H()		PB_ODR|=0X08 
#define VKL144_SDA_L()		PB_ODR&=~0X08
#define VKL144_SCL_H()		PB_ODR|=0X10    
#define VKL144_SCL_L()		PB_ODR&=~0X10
#define VKL144_GET_SDA()        ((PB_IDR&0X08)==0x08)

#define VKL144_SET_SDA_IN()     GPIO_input_init('B',3)          //浮空输入,无中断        
#define VKL144_SET_SDA_OUT()    GPIO_output_init('B',3)      //推挽输出  





/* Exported functions ------------------------------------------------------- */
void VKL144_Delay_7us(unsigned int ucDat);
void VKL144_Delay_ms(unsigned int ucDat);

void VKL144_I2CStart( void );
void VKL144_I2CStop( void );
unsigned char VKL144_I2CSlaveAck( void );
void VKL144_I2CWriteByte( unsigned char byte );
void VKL144_FunSet(unsigned char command);

void Write1DataVKL144(unsigned char Addr,unsigned char Dat);
void WritenDataVKL144(unsigned char Addr,unsigned char *Databuf,unsigned char Cnt);
void VKL144_DisAll(unsigned char state);
void VKL144_DisSegOn(unsigned char Seg,unsigned char Com);
void VKL144_DisSegOff(unsigned char Seg,unsigned char Com);

void VKL144_Halt(void);
void VKL144_Init(void);
void VKL144_num_init(void);

void DispJUHUI(void);
void VKL144_Test(void);
void VKL144_Display(void);
void VKL144_MyDisplay(long Temp_Data);

void    VLK_Display_Num(uint16_t num2disp,uint8_t pPos);
#endif  /*__VKL144_H*/
/************************END OF FILE****/















