#include "ADS1115.h"
uint8_t   ADS1115_SendBuf1[4];
uint8_t   ADS1115_SendBuf2[4];
uint8_t   ADS1115_ReceiBuf[2];

//////////////////////////////////////////////////////////////
//      I2C_Delay :     
void I2C_Delay(void)
{
	int i=8;
	while(i)
	{
		i--;
	}
}
//////////////////////////////////////////////////////////////
void delay5ms(void)
{
	int i=8;
	while(i)
	{
		i--;
	}
}
//////////////////////////////////////////////////////////////
void I2C_Start(void)
{
	SDA_OUT();
	SDA_H;
	SCL_H;
	I2C_Delay();	
	SDA_L;
	I2C_Delay();
	SCL_L;		
}
//////////////////////////////////////////////////////////////
void I2C_Stop(void)
{
	SDA_OUT();
	SCL_L;
	SDA_L;
	I2C_Delay();
	
	SCL_H;
	I2C_Delay();
	SDA_H;
}
//////////////////////////////////////////////////////////////
void I2C_Ack(void)
{
	SCL_L;
	SDA_OUT();
	SDA_L;
	I2C_Delay();
	
	SCL_H;
	I2C_Delay();
	SCL_L;
}
//////////////////////////////////////////////////////////////
void I2C_NoAck(void)
{
	SCL_L;
	SDA_OUT();
	
	SDA_H;
	I2C_Delay();
	
	SCL_H;
	I2C_Delay();
	SCL_L;

}
//////////////////////////////////////////////////////////////
int I2C_Wait_Ack(void)
{
        uint8_t ucErrTime=0;
        SCL_L;
	SDA_H;
	I2C_Delay();
	SCL_H;
	I2C_Delay();
        SDA_IN();

	if(SDA_read)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			I2C_Stop();
			return FALSE;
		}
	}
	SCL_L;
	return TRUE;
}
//////////////////////////////////////////////////////////////
void I2C_SendByte(uint8_t SendByte)
{
	uint8_t i=8;
	SDA_OUT();
	
	SCL_L;
	for(i=0;i<8;i++)
        {              
            if(SendByte & 0x80)
		SDA_H;
            else
		SDA_L;
            SendByte<<=1;	  
            I2C_Delay();   //这三个延时都是必须的
            SCL_H;
            I2C_Delay(); 
            SCL_L;	
            I2C_Delay();
        }
}
//////////////////////////////////////////////////////////////
uint8_t I2C_ReadByte(void)
{
	uint8_t i;
	uint8_t ReceiveByte=0;
        SDA_IN();
	for(i=0;i<8;i++)
	{
            SCL_H;
            I2C_Delay();
            ReceiveByte<<=1;
            if(SDA_read)
		ReceiveByte++;
            I2C_Delay();
            SCL_L;		
	}
	return ReceiveByte;
}
//////////////////////////////////////////////////////////////
//ADS1115初始化
//ADDR接地
void ads1115_init(void)
{
	uint8_t i;

	SDA_INIT;
	SCL_INIT;
	//配置 ADS1115I/O
	SDA_H;
	SCL_H;
	
	//设置寄存器使能输出脉冲/DRDY	
	//Lo_thresh 最高位=0
	I2C_Start();
	ADS1115_SendBuf2[0]=0x90;
	ADS1115_SendBuf2[1]=0x02;
	ADS1115_SendBuf2[2]=0x00;
	ADS1115_SendBuf2[3]=0x00;

	for(i=0;i<4;i++)
	{
		I2C_SendByte(ADS1115_SendBuf2[i]);
		I2C_Wait_Ack();
	}
	I2C_Stop();
        
        delay5ms();
	//Hi_thresh 最高位=1
	I2C_Start();
	ADS1115_SendBuf2[0]=0x90;
	ADS1115_SendBuf2[1]=0x03;
	ADS1115_SendBuf2[2]=0x80;
	ADS1115_SendBuf2[3]=0x00;

	for(i=0;i<4;i++)
	{
		I2C_SendByte(ADS1115_SendBuf2[i]);
		I2C_Wait_Ack();
	}
	I2C_Stop();
        delay5ms();	
	
	//设置配置寄存器
	I2C_Start();
	//写命令
	ADS1115_SendBuf1[0]=0x90;
	//写配置寄存器01号
	ADS1115_SendBuf1[1]=0x01;
	//先写高8位
	//D15 默认0,写入1位开始一次单次转换
	//D14-D12 输入模拟开关选择
	//000 : AINP = AIN0 and AINN = AIN1 (default)   100 : AINP = AIN0 and AINN = GND
	//001 : AINP = AIN0 and AINN = AIN3             101 : AINP = AIN1 and AINN = GND
	//010 : AINP = AIN1 and AINN = AIN3             110 : AINP = AIN2 and AINN = GND
	//011 : AINP = AIN2 and AINN = AIN3             111 : AINP = AIN3 and AINN = GND
	//D11-D9 增益控制
	//000 : FS = ±6.144V (1) 			100 : FS = ±0.512V
	//001 : FS = ±4.096V (1) 			101 : FS = ±0.256V
	//010 : FS = ±2.048V (default) 	                110 : FS = ±0.256V
	//011 : FS = ±1.024V 				111 : FS = ±0.256V
	//D8 转换模式       0:Continuous conversion mode;
         //                 1:Power-down single-shot mode (default)
	//实际设置:0100 0101,单次转换+AN0单端输入+2.048V
	ADS1115_SendBuf1[2]=0xD5;//45
        
	//再写低8位
	//D7-D5 转换速率
	//000 : 8SPS 100 : 128SPS (default)
	//001 : 16SPS 101 : 250SPS
	//010 : 32SPS 110 : 475SPS
	//011 : 64SPS 111 : 860SPS
	//D4    比较器控制0   : Traditional comparator with hysteresis (default);1 : Window comparator
	//D3    比较器控制0   : Active low (default);1 : Active high
	//D2    比较器控制0   : Non-latching comparator (default);1 : Latching comparator
	//D1-D0 比较器控制 00 : Assert after one conversion;01 : Assert after two conversions
        //                 10 : Assert after four conversions;11 : Disable comparator (default)
	//实际设置:1110 1000
	ADS1115_SendBuf1[3]=0xe8;	
	for(i=0;i<4;i++)
	{
		I2C_SendByte(ADS1115_SendBuf1[i]);
		I2C_Wait_Ack();
	}
	I2C_Stop();
        delay5ms();
}
//////////////////////////////////////////////////////////////
//启动通道xxx单次转换
//D14-D12 输入模拟开关选择
//000 : AINP = AIN0 and AINN = AIN1 (default) 100 : AINP = AIN0 and AINN = GND
//001 : AINP = AIN0 and AINN = AIN3           101 : AINP = AIN1 and AINN = GND
//010 : AINP = AIN1 and AINN = AIN3           110 : AINP = AIN2 and AINN = GND
//011 : AINP = AIN2 and AINN = AIN3           111 : AINP = AIN3 and AINN = GND
//对应0--7
void startch0(uint8_t ch)
{
	uint8_t i;
	//启动一次转换
	//设置配置寄存器
	I2C_Start();
	//写命令
	ADS1115_SendBuf1[0]=0x90;
	//写配置寄存器01号
	ADS1115_SendBuf1[1]=0x01;
	//先写高8位
	//D15 默认0,写入1位开始一次单次转换
	//D14-D12 输入模拟开关选择
	//000 : AINP = AIN0 and AINN = AIN1 (default) 100 : AINP = AIN0 and AINN = GND
	//001 : AINP = AIN0 and AINN = AIN3           101 : AINP = AIN1 and AINN = GND
	//010 : AINP = AIN1 and AINN = AIN3           110 : AINP = AIN2 and AINN = GND
	//011 : AINP = AIN2 and AINN = AIN3           111 : AINP = AIN3 and AINN = GND
	//D11-D9 增益控制
	//000 : FS = ±6.144V (1) 		      100 : FS = ±0.512V
	//001 : FS = ±4.096V (1) 		      101 : FS = ±0.256V
	//010 : FS = ±2.048V (default) 	              110 : FS = ±0.256V
	//011 : FS = ±1.024V 			      111 : FS = ±0.256V
	//D8 转换模式   0: Continuous conversion mode;
        //              1: Power-down single-shot mode (default)
	//实际设置:1xxx 0101,单次转换+AN0单端输入+2.048V
	switch(ch)
	{
            case 0:
		ADS1115_SendBuf1[2]=0x85;
		break;
            case 1:
		ADS1115_SendBuf1[2]=0x95;
		break;
            case 2:
		ADS1115_SendBuf1[2]=0xA5;
		break;
            case 3:
		ADS1115_SendBuf1[2]=0xB5;
		break;
            case 4:
		ADS1115_SendBuf1[2]=0xC5;
		break;
	    case 5:
		ADS1115_SendBuf1[2]=0xD5;
		break;
	    case 6:
		ADS1115_SendBuf1[2]=0xE5;
		break;
	    case 7:
		ADS1115_SendBuf1[2]=0xF5;
		break;
            default:					
		break;
        }
	//再写低8位
	//D7-D5 转换速率
	//000 : 8SPS  100 : 128SPS (default)
	//001 : 16SPS 101 : 250SPS
	//010 : 32SPS 110 : 475SPS
	//011 : 64SPS 111 : 860SPS
	//D4 比较器控制0 : Traditional comparator with hysteresis (default);1 : Window comparator
	//D3 比较器控制0 : Active low (default);1 : Active high
	//D2 比较器控制0 : Non-latching comparator (default);            1 : Latching comparator
	//D1-D0 比较器控制 00 : Assert after one conversion;            01 : Assert after two conversions
	//                 10 : Assert after four conversions;          11 : Disable comparator (default)
	//实际设置:1110 1000
	ADS1115_SendBuf1[3]=0xe8;
	
	for(i=0;i<4;i++)
	{
		I2C_SendByte(ADS1115_SendBuf1[i]);
		I2C_Wait_Ack();
	}
	I2C_Stop();
	delay5ms();
	
	//发命令读数据
	//设置寄存器读数据
	I2C_Start();
	ADS1115_SendBuf2[0]=0x90;
	ADS1115_SendBuf2[1]=0x00;
	for(i=0;i<2;i++)
	{
		I2C_SendByte(ADS1115_SendBuf2[i]);
		I2C_Wait_Ack();
	}
		I2C_Stop();
		delay5ms();
}


//ADS1115采样0通道
//////////////////////////////////////////////////////////////
uint8_t hh,ll;
int16_t ads1115_ch0(void)
{
    uint8_t i,device_flag;
    uint8_t ww,yy,zz;
    
    uint16_t j;
    int16_t k;

    device_flag=0;
				

    //开启I2C总线
    SDA_H;
    SCL_H;
					
    for(ww=0;ww<3;ww++);       //总线等待
    SDA_L;
    for(ww=0;ww<3;ww++);       //总线等待
    SCL_L;
    //写入0x91表示读取
    yy=8;
    zz=0x91;
    while(yy--)
    {
	SCL_L;
	if((zz&0x80)==0x80)
	{
            SDA_H;
        }
	else
	{
            SDA_L;
	}
        zz <<= 1;
        for(ww=0;ww<3;ww++);       //总线等
	SCL_H;
        for(ww=0;ww<3;ww++);       //总线等
    }
    SCL_L;
    //等待器件应答，如果器件错误标志置位
    yy=0;
    SDA_H;
    for(ww=0;ww<3;ww++);       //总线等待
    SCL_H;
    for(ww=0;ww<3;ww++);       //总线等待
    while(((SDA_read)!=0)&&(yy<=200))
    {
	yy=yy+1;
    }		
    if(yy>=200)
    {
	device_flag=1; 
    }					
    SCL_L;
    //读出高位字节
    hh=I2C_ReadByte();
    SDA_OUT();
        //主机拉低准备读取第二个字节		
	SDA_L;
	SCL_H;
	for(ww=0;ww<3;ww++);       //总线等待
	SCL_L;
	SDA_H;
	//准备读取第二个字节
	ll=I2C_ReadByte();
	//主机拉高不做应答,第二个字读取结束		
	SDA_H;
	SCL_H;
	for(ww=0;ww<3;ww++);       //总线等待
	SCL_L;
	SDA_L;
	for(ww=0;ww<3;ww++);       //总线等待
        //读取结束退出总线
	SCL_H;
	for(ww=0;ww<3;ww++);       //总线等待
	SDA_H;
	for(ww=0;ww<3;ww++);       //总线等待
	j=256*hh+ll;
	//处理正负
	k=j;
				 
	return k;
}