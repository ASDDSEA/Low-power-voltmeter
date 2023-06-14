#include "VKL144.h"
#include "stdio.h"
#include "stdlib.h"

#define	RESET				0
#define	SET			        1

#define VKL144_BASE100MS	        100     //实现100mS延时，根据单片机修改此常数
#define VKL144_CLK			1	//WR信号线频率 ,由VKL144_Delay_us实现， =1->400kHz =10->100kHz =100->11.11kHz

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

const unsigned char VKL144_segtab[VKL144_SEGNUM]=
{
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
};
unsigned char VKL144_addrbit=6;			//驱动ic的ram地址线数A5-A0
unsigned char VKL144_segi,VKL144_comi;
unsigned char VKL144_dispram[VKL144_RAMBUFNUM];

uint8_t        VKL144_DispArray[8]  =  {0};

const unsigned char LED_Tab[]=
{
	0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,	//0-15 = 0--f
	0xbf,0x86,0xdb,0xcf,0xe6,0xed,0xfd,0x87,0xff,0xef,                  //16-25 = 0.--9.
        0x58,	//26=c
	0x37,	//17=N
	0x40,	//18=-
	0x0E,	//19=J
	0x3E,	//20=U
	0x76,	//21=H
	0x30,	//22=I
};
unsigned char LCD_RAM_Tab[sizeof(LED_Tab)];
#define		LCD_A			0x01
#define		LCD_B			0x02
#define		LCD_C			0x04
#define		LCD_D			0x08
#define		LCD_E			0x10
#define		LCD_F			0x20
#define		LCD_G			0x40
#define		LCD_P			0x80
//显示RAM buffer使用8位，bit0,4->com0,bit1,5->com1,bit2,6->com2,bit37->com3
//高4位偶数seg，低4位奇数seg
//如果是从奇数seg开始VKL144_dispram[0]的高4位不用



/*************************************************************
*函数名称：VKL144_Delay_us
*函数功能：Inserts a delay time
*输入参数：ucDat-> 延时时间
*输出参数：None
*备    注：此程序需要自行校准	实现 2us 延时
**************************************************************/
void VKL144_Delay_7us(unsigned int ucDat)
{
	while(ucDat--) 
	{//nop指令根据单片机做相应的修改
	}
}
/*************************************************************
*函数名称：VKL144_Delay_ms
*函数功能：Inserts a delay time
*输入参数：ucDat-> 延时时间
*输出参数：None
*备    注：此程序需要自行校准 实现 1ms 延时
**************************************************************/
void VKL144_Delay_ms(unsigned int ucDat)
{
	unsigned int a,b;
	
	for(a=ucDat; a>0; a--)
	{
		for(b=198; b>0; b--);
	}
}
/*************************************************************
*函数名称：I2CStart
*函数功能：I2C开始信号
*输入参数：None
*输出参数：None
*备    注：时钟线高时，数据线由高到低的跳变，表示I2C开始信号
**************************************************************/
void VKL144_I2CStart( void )
{
	VKL144_SDA_H();
	VKL144_SCL_H();
	VKL144_Delay_7us(VKL144_CLK);
	VKL144_SDA_L();
	VKL144_Delay_7us(VKL144_CLK);
	VKL144_SCL_L();
}
/*************************************************************
*函数名称：I2CStop
*函数功能：I2C停止信号
*输入参数：None
*输出参数：None
*备    注：时钟线高时，数据线由低到高的跳变，表示I2C停止信号
**************************************************************/
void VKL144_I2CStop( void )
{
	VKL144_SDA_L();
	VKL144_Delay_7us(VKL144_CLK);
	VKL144_SCL_H();
	VKL144_Delay_7us(VKL144_CLK);
	VKL144_SDA_H();
}
/*************************************************************
*函数名称：I2CSlaveAck
*函数功能：I2C从机设备应答查询
*输入参数：None
*输出参数：None
*备    注：
**************************************************************/
unsigned char VKL144_I2CSlaveAck( void )
{
	unsigned int TimeOut;
	unsigned char RetValue;
	
	VKL144_SET_SDA_IN();
	VKL144_Delay_7us(VKL144_CLK);
	VKL144_SCL_H();
	
	TimeOut = 10000;
	while( TimeOut-- > 0 )
	{
		if( VKL144_GET_SDA() )	//SDA为高，未收到应答
		{
			RetValue = RESET;
		}
		else					//SDA为低，收到应答
		{
			RetValue = SET;
			break;
		}
	}
	VKL144_SCL_L();
	VKL144_SET_SDA_OUT();
	
	return RetValue;
}
/*************************************************************
*函数名称：I2CWriteByte
*函数功能：I2C写一字节数据
*输入参数：None
*输出参数：None
*备    注：
**************************************************************/
void VKL144_I2CWriteByte( unsigned char byte )
{
	unsigned char i=8;
	while (i--)
	{
		if(byte&0x80)
			VKL144_SDA_H();
		else
			VKL144_SDA_L();
		byte<<=1; 
		VKL144_Delay_7us(VKL144_CLK);
		VKL144_SCL_H();     
		VKL144_Delay_7us(VKL144_CLK); 
		VKL144_SCL_L();
	}
}
/*************************************************************
*函数名称：FunSet
*函数功能：功能设置
*输入参数：command-输入指令 
*输出参数：None
*备    注：
**************************************************************/
void VKL144_FunSet(unsigned char command)
{ 
	unsigned char i;
	
	i=1;
	while(i--)
	{ 
		//VKL144_Delay_7us(1);
                VKL144_Delay_ms(1);
		VKL144_I2CStop(); 
		VKL144_Delay_ms(1);
                //VKL144_Delay_7us(1);
		VKL144_I2CStart(); 
		VKL144_I2CWriteByte(VKL144_ADDR); 
		if( RESET == VKL144_I2CSlaveAck() )
		{
			continue;
		}
		VKL144_I2CWriteByte(command); 
		if( RESET == VKL144_I2CSlaveAck() )
		{
			continue;
		}
		VKL144_I2CStop(); 
	}
	VKL144_I2CStop();
}
/**
  * @brief  Write 1 data to VKL144 
  * @param  Addr->写入ram的地址 
  * @param  Databuf->写入ram的数据
  * @retval None
  */
void Write1DataVKL144(unsigned char Addr,unsigned char Dat)
{
	unsigned char i;
	
	i=1;
	while(i--)
	{ 
		VKL144_Delay_ms(1);
		VKL144_I2CStop(); 
		VKL144_Delay_ms(1);
		VKL144_I2CStart(); 
		VKL144_I2CWriteByte(VKL144_ADDR);
		if( RESET == VKL144_I2CSlaveAck() )
		{
			continue;
		}
		if(Addr>0x1f)
			VKL144_I2CWriteByte(VKL144_ICSET|BIT2_ADDR5); 
		else
			VKL144_I2CWriteByte(VKL144_ICSET);
		if( RESET == VKL144_I2CSlaveAck() )
		{
			continue;
		}
		VKL144_I2CWriteByte(Addr&0x1f);
		if( RESET == VKL144_I2CSlaveAck() )
		{
			continue;
		}
		
		VKL144_I2CWriteByte(Dat);
		if( SET == VKL144_I2CSlaveAck() )
		{
			continue;
		}
	}
	VKL144_I2CStop();
}
/**
  * @brief  Write n data to VKL144 
  * @param  Addr->写入ram的起始地址 
  * @param  Databuf->写入ram的数据buffer
  * @param  Cnt->写入ram的数据个数
  * @retval errorflag
  */
void WritenDataVKL144(unsigned char Addr,unsigned char *Databuf,unsigned char Cnt)
{
	unsigned char i,n;
	
	i=1;
	while(i--)
	{ 
		VKL144_Delay_ms(1);
		VKL144_I2CStop(); 
		VKL144_Delay_ms(1);
		VKL144_I2CStart(); 
		VKL144_I2CWriteByte(VKL144_ADDR); 
		if( RESET == VKL144_I2CSlaveAck() )
		{
			continue;
		}
		if(Addr>0x1f)
			VKL144_I2CWriteByte(VKL144_ICSET|BIT2_ADDR5); 
		else
			VKL144_I2CWriteByte(VKL144_ICSET); 
		if( RESET == VKL144_I2CSlaveAck() )
		{
			continue;
		}
		VKL144_I2CWriteByte(Addr&0x1f); 
		if( RESET == VKL144_I2CSlaveAck() )
		{
			continue;
		}
		
		for(n=0;n<Cnt;n++)
		{ 
			VKL144_I2CWriteByte(*Databuf++);
			if( SET == VKL144_I2CSlaveAck() )
			{
				continue;
			}
			else
			{
				break;
			}
		}
	 }
	 VKL144_I2CStop();
}
/**
  * @brief  lcd全显或全灭
  * @param  state=1->lcd全亮 
  * @param  state=0->lcd全灭
  * @retval None
  */
void VKL144_DisAll(unsigned char state)
{
	unsigned char segi;
	unsigned char dispram[18];
	
	for(segi=0;segi<18/2;segi++)
	{
		dispram[segi]=0;
	}
	for(segi=0;segi<VKL144_RAMBUFNUM;segi++)
	{
		if(state==1)
		{
			VKL144_dispram[segi]=0xff;
		}
		else
		{
			VKL144_dispram[segi]=0x00;
		}
		dispram[VKL144_segtab[0]/2+segi]=VKL144_dispram[segi];
	}
	WritenDataVKL144(0,dispram,18);
}
/**
  * @brief  lcd显示单个点（1comx1seg）
  * @param  Seg->点对应的seg 
  * @param  Com->点对应的com
  * @retval None
  */
void VKL144_DisSegOn(unsigned char Seg,unsigned char Com)
{
	if(Seg%2==0)
	{
		VKL144_dispram[(Seg-VKL144_segtab[0])/2]|=(1<<(Com+4));
		Write1DataVKL144(Seg,VKL144_dispram[(Seg-VKL144_segtab[0])/2]);
	}
	else
	{
		VKL144_dispram[(Seg-VKL144_segtab[0])/2]|=(1<<Com);		
		Write1DataVKL144(Seg-1,VKL144_dispram[(Seg-VKL144_segtab[0])/2]);
	}
}
/**
  * @brief  lcd关闭单个点（1comx1seg）
  * @param  None 
  * @retval None
  */
void VKL144_DisSegOff(unsigned char Seg,unsigned char Com)
{
	if(Seg%2==0)
	{
		VKL144_dispram[(Seg-VKL144_segtab[0])/2]&=~(1<<(Com+4));
		Write1DataVKL144(Seg,VKL144_dispram[(Seg-VKL144_segtab[0])/2]);
	}
	else
	{
		VKL144_dispram[(Seg-VKL144_segtab[0])/2]&=~(1<<Com);		
		Write1DataVKL144(Seg-1,VKL144_dispram[(Seg-VKL144_segtab[0])/2]);
	}
}

/**
  * @brief  VKL144进入掉电低功耗模式
  * @param  None 
  * @retval None
  */
void VKL144_Halt(void)
{		
	unsigned char i;
	
	i=1;
	while(i--)
	{
		VKL144_I2CStart();
		VKL144_I2CWriteByte(VKL144_ADDR); 
		if( RESET == VKL144_I2CSlaveAck() )
		{
			continue;
		}
		
		#if	BIAS_1_2
                    VKL144_I2CWriteByte(VKL144_MODESET_1_2_OFF);	//关闭显示 1/2bias
		#endif
		
		#if	BIAS_1_3
                    VKL144_I2CWriteByte(VKL144_MODESET_1_3_OFF);	//关闭显示 1/3bias
		#endif
		
		if( RESET == VKL144_I2CSlaveAck() )
		{
			continue;
		}
	}
	VKL144_I2CStop();
}
/**
  * @brief  VKL144 Init	退出低功耗调用此含税
  * @param  None 
  * @retval None
  */
void VKL144_Init(void)
{	
	unsigned char i,n;
	unsigned char errorflag = 1;//错误标志位，1为错误，0为正确
	
	i=1;
	while(i--)
	{
		VKL144_Delay_ms(1);                      //     IC初始化               
		VKL144_I2CStop();                        //     停止       
		VKL144_Delay_ms(1);
		VKL144_I2CStart();                      //      启动        
		VKL144_I2CWriteByte(VKL144_ADDR);       //      Slave地址发送
		if( RESET == VKL144_I2CSlaveAck() )
		{
			continue;
		}
		
		VKL144_I2CWriteByte(VKL144_ICSET|BIT1_ENSRST); //复位
		if( RESET == VKL144_I2CSlaveAck() )
		{
			continue;
		}
		VKL144_I2CWriteByte(VKL144_ICSET); 	//映射起始地址 bit5=0
		if( RESET == VKL144_I2CSlaveAck() )
		{
			continue;
		}
		VKL144_I2CWriteByte(VKL144_ADSET); //映射起始地址 bit4~bit0=0
		if( RESET == VKL144_I2CSlaveAck() )
		{
			continue;
		}
		errorflag=0;
		for(n=0;n<VKL144_RAMBUFNUM;n++)
		{ 
			VKL144_I2CWriteByte(0x00);      // 初始化的显示内存显示映射,可修改这里. 
			if( SET == VKL144_I2CSlaveAck() )
			{
				continue;
			}
			else
			{
				errorflag=1;
				break;
			}
		}
		if(errorflag==1)
		{
			continue;
		}
		
		VKL144_I2CStop(); 
                //====================================================================================
		VKL144_Delay_ms(1);  
		VKL144_I2CStart();
		
		VKL144_I2CWriteByte(VKL144_ADDR);
		if( RESET == VKL144_I2CSlaveAck() )
		{
			continue;
		}
		
		#if	BIAS_1_2
                    VKL144_I2CWriteByte(VKL144_MODESET_1_2_ON);	//打开显示 1/2bias
		#endif
		
		#if	BIAS_1_3
                    VKL144_I2CWriteByte(VKL144_MODESET_1_3_ON);	//打开显示 1/3bias
		#endif
		
		if( RESET == VKL144_I2CSlaveAck() )
		{
			continue;
		}
	}
	VKL144_I2CStop();
	VKL144_FunSet(VKL144_DISCTL_DATA);	//工作电流设置
	
	VKL144_num_init();			//自动生成液晶屏需要的数组
        VKL144_DispArray[0]     =     LCD_RAM_Tab[13];
        VKL144_DispArray[1]     =     LCD_RAM_Tab[12];
        VKL144_DispArray[2]     =     LCD_RAM_Tab[18];
        
}
/**
  * @brief  数据交换，自动生成液晶需要的数组，此程序，需要根据液晶图纸修改
  * @param  None
  * @retval None
  */

//unsigned char xdata LCD_RAM_Tab[sizeof(LED_Tab)];


void VKL144_num_init(void)
{
	unsigned char num,LCD_data;
	
	for(num=0; num<sizeof(LED_Tab); num++)
	{
		LCD_data = 0;
                LCD_data = LCD_data | ((unsigned char)((LED_Tab[num] & LCD_D)) << 4);      // 3+4
		LCD_data = LCD_data | ((unsigned char)((LED_Tab[num] & LCD_E)) << 2);      // 4+2
		LCD_data = LCD_data | ((unsigned char)((LED_Tab[num] & LCD_F)) << 0);      // 5+0
		LCD_data = LCD_data | ((unsigned char)((LED_Tab[num] & LCD_A)) << 4);      // 0+4
		
		LCD_data = LCD_data | ((unsigned char)((LED_Tab[num] & LCD_P)) >> 4);      // 7-4
		LCD_data = LCD_data | ((unsigned char)((LED_Tab[num] & LCD_C)) >> 0);      // 2-0
		LCD_data = LCD_data | ((unsigned char)((LED_Tab[num] & LCD_G)) >> 5);      // 6-5
		LCD_data = LCD_data | ((unsigned char)((LED_Tab[num] & LCD_B)) >> 1);      // 1-1
                /*
		LCD_data = LCD_data | ((unsigned char)((bit)(LED_Tab[num] & LCD_D)) << 7);
		LCD_data = LCD_data | ((unsigned char)((bit)(LED_Tab[num] & LCD_E)) << 6);
		LCD_data = LCD_data | ((unsigned char)((bit)(LED_Tab[num] & LCD_F)) << 5);
		LCD_data = LCD_data | ((unsigned char)((bit)(LED_Tab[num] & LCD_A)) << 4);
		
		LCD_data = LCD_data | ((unsigned char)((bit)(LED_Tab[num] & LCD_P)) << 3);
		LCD_data = LCD_data | ((unsigned char)((bit)(LED_Tab[num] & LCD_C)) << 2);
		LCD_data = LCD_data | ((unsigned char)((bit)(LED_Tab[num] & LCD_G)) << 1);
		LCD_data = LCD_data | ((unsigned char)((bit)(LED_Tab[num] & LCD_B)) << 0);
		*/
		LCD_RAM_Tab[num] = LCD_data;
	}
}

void DispJUHUI(void)
{
	for(uint8_t ii=0;ii<8;ii++)
        {
            Write1DataVKL144(VKL144_segtab[2*ii], LCD_RAM_Tab[VKL144_DispArray[ii]]);
        }
}

void VKL144_Test(void)
{	
	VKL144_DisAll(1);
	VKL144_Delay_ms(1000);
	
	VKL144_DisAll(0);
	VKL144_Delay_ms(1000);
	
	for(VKL144_segi=0;VKL144_segi<VKL144_SEGNUM;VKL144_segi++)		//seg
	{
		for(VKL144_comi=0;VKL144_comi<4;VKL144_comi++)				//com
		{
			VKL144_DisSegOn(VKL144_segtab[VKL144_segi],VKL144_comi);//LCD单个seg点亮
			VKL144_Delay_ms(100);
			VKL144_DisAll(0);
		}
	}
	
	VKL144_DisAll(1);
	VKL144_Delay_ms(1000);
	
	for(VKL144_segi=0;VKL144_segi<VKL144_SEGNUM;VKL144_segi++)		//seg
	{
		for(VKL144_comi=0;VKL144_comi<4;VKL144_comi++)				//com
		{
			VKL144_DisSegOff(VKL144_segtab[VKL144_segi],VKL144_comi);//LCD单个seg关闭
			VKL144_Delay_ms(100);
			VKL144_DisAll(1);
		}
	}
}
/**
  * @brief  用户显示程序，需要自己完善
  * @param  None
  * @param  None
  * @retval None
  */
void VKL144_Display(void)
{
	unsigned char Temp_Data,LCD_Data;
	
	//---------------------------------------------------------------
	
	Temp_Data = 12;
	
	LCD_Data = Temp_Data / 10;
	Write1DataVKL144(VKL144_segtab[0],	LCD_RAM_Tab[LCD_Data]);
	LCD_Data = Temp_Data % 10;
	Write1DataVKL144(VKL144_segtab[2],	LCD_RAM_Tab[LCD_Data]);
	
	
	//---------------------------------------------------------------
	
	Temp_Data = 34;
	
	LCD_Data = Temp_Data / 10;
	Write1DataVKL144(VKL144_segtab[4],	LCD_RAM_Tab[LCD_Data]);
	LCD_Data = Temp_Data % 10;
	Write1DataVKL144(VKL144_segtab[6],	LCD_RAM_Tab[LCD_Data]);
	
	//---------------------------------------------------------------
	
	Temp_Data = 56;
	
	LCD_Data = Temp_Data / 10;
	Write1DataVKL144(VKL144_segtab[8],	LCD_RAM_Tab[LCD_Data]);
	LCD_Data = Temp_Data % 10;
	Write1DataVKL144(VKL144_segtab[10],	LCD_RAM_Tab[LCD_Data]);
	
	//---------------------------------------------------------------
	
	Temp_Data = 78;
	
	LCD_Data = Temp_Data / 10;
	Write1DataVKL144(VKL144_segtab[12],	LCD_RAM_Tab[LCD_Data]);
	LCD_Data = Temp_Data % 10;
	Write1DataVKL144(VKL144_segtab[14],	LCD_RAM_Tab[LCD_Data]);
}
/************************END OF FILE****/


void    VLK_Display_Num(uint16_t num2disp,uint8_t pPos)
{
    //uint16_t    Temp_Data;
    //uint8_t     LCD_Data;
    // ===================================================
    for(uint8_t ii =0;ii<5;ii++)
    {
        if(num2disp >0)
        {  
            VKL144_DispArray[7-ii]   =   LCD_RAM_Tab[num2disp % 10];
            if(ii ==pPos)
                VKL144_DispArray[7-ii]  |=   0x08;           
            num2disp /= 10;
        }
        else if(ii <= pPos)
        {
            VKL144_DispArray[7-ii]   =   LCD_RAM_Tab[0]; 
            if(ii ==pPos)
                VKL144_DispArray[7-ii]  |=   0x08;           
        }  
        else 
            VKL144_DispArray[7-ii]   =   0;
    }     
    for(uint8_t ii =0;ii<8;ii++)
          Write1DataVKL144(VKL144_segtab[ii*2],VKL144_DispArray[ii]);
}

void VKL144_MyDisplay(long Temp_Data)
{
   int LCD_Data;   		
	
   LCD_Data =Temp_Data/10000;
   Write1DataVKL144(VKL144_segtab[0],	LCD_RAM_Tab[LCD_Data+16]);//百位

   LCD_Data =(Temp_Data/1000)%10;
   Write1DataVKL144(VKL144_segtab[2],	LCD_RAM_Tab[LCD_Data]);//十位
        
   LCD_Data =(Temp_Data/100)%10;        
   Write1DataVKL144(VKL144_segtab[4],	LCD_RAM_Tab[LCD_Data]);//个位
   
   LCD_Data =(Temp_Data/10)%10;
   Write1DataVKL144(VKL144_segtab[6],	LCD_RAM_Tab[LCD_Data]);//0.1位
	
   LCD_Data =Temp_Data%10;
   Write1DataVKL144(VKL144_segtab[8],	LCD_RAM_Tab[LCD_Data]);//0.01位
   
   //VKL144_DisSegOn(5,3);//小数点
}


