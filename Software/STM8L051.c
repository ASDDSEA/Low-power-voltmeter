#include<iostm8l051f3.h>
#include<main.h>
/*
void lock_other()
{

    CLK_PCKENR2&=~0x01;//disable the ADC function


}
*/
void GPIO_output_init(char port,int pin)//推挽输出
{
  switch(port)
  {
    case "A":PA_DDR|=(0X01<<pin);PA_CR1=(0X01<<pin);break;
    case "B":PB_DDR|=(0X01<<pin);PB_CR1=(0X01<<pin);break;
    case "C":PC_DDR|=(0X01<<pin);PC_CR1=(0X01<<pin);break;
  
  }
}
void GPIO_input_init(char,int pin)//悬浮输入
{
      switch(port)
  {
    case "A":PA_DDR&=~(0X01<<pin);PA_CR1&=~(0X01<<pin);break;
    case "B":PB_DDR&=~(0X01<<pin);PB_CR1&=~(0X01<<pin);break;
    case "C":PC_DDR&=~(0X01<<pin);PC_CR1&=~(0X01<<pin);break;
  
  }
}
void Haltmode_GPIODefault(void)//GPIO configue
{
    PB_DDR=0XFF;
    PB_CR1=0XFF;
    PB_ODR=0X00;

   

}

void Halt_RTC_Init()
{

  CLK_PCKENR2|=0x04;// 使能RTC时钟
  PWR_CSR2|=0x06;//关闭内部参考电压，配置快速相应
  CLK_CRTCR=0x04;//RTC预分配64 选LSI作为时钟源
  RTC_WPR=0xCA;//写入前需要执行的密钥
  RTC_WPR=0x53;//同上
  RTC_CR2&=0XfB;//清除WUTE
  RTC_ISR1|=0X04;//置位WUTWF
  RTC_CR1&=0XF0;//十六分频
  RTC_WUTRH=0x18;//计数高
  RTC_WUTRL=0x47;//低  （WUTR *1/(RTC时钟源/分频)=时间）
  RTC_CR2|=0X04;//使能唤醒时钟
  RTC_CR2|=0X40;//使能RTC中断
  //RTC_ISR2&=~0X04; 

  //RTC_WPR=0xFF;//上锁 

}

void delay(int Count)
{
    int Count1,Count2;
    while(Count--)
    {
        for(Count1=0;Count1<50;Count1++)
            for(Count2=0;Count2<20;Count2++);
    }
}

void SysClkInit( void )
{
                   
    ///CLK_ICKCR|=0X20;
    CLK_SWR = 0x02;
    CLK_CKDIVR =0x03;
    //while (CLK_SCSR!=0x01);
}
void main_Init()
{
  SysClkInit();
  Halt_RTC_Init();
  Haltmode_GPIODefault();
  //lock_other();
}

int i=0;
int main()
{
  asm( "sim" );  

  main_Init();
  asm( "rim" );
  while(1)
  {
    PB_ODR=0X00;

          asm("HALT");


  }

}
#pragma vector=6
__interrupt void RTC_IRQHandler(void)
{
  
      PB_ODR=0XFF;

    RTC_ISR2 &=~0X04;
/*Bit 2 WUTF: Periodic wakeup flag
This flag is set by hardware when the wakeup auto-reload down-counter reaches 0.
It is cleared by software by writing 0. 
Note: This flag must be cleared by software at least 1.5 RTCCLK cycles before WUTF is set to 1 
again
*/


}
