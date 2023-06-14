
#include<main.h>
/*
void lock_other()
{

    CLK_PCKENR2&=~0x01;//disable the ADC function


}
*/
void GPIO_output_init(char port,int pin)//�������
{
  switch(port)
  {
    case 'A':PA_DDR|=(0X01<<pin);PA_CR1|=(0X01<<pin);break;

    case 'B':PB_DDR|=(0X01<<pin);PB_CR1|=(0X01<<pin);break;
    case 'C':PC_DDR|=(0X01<<pin);PC_CR1|=(0X01<<pin);break;
  
  }
}
void GPIO_input_init(char port,int pin)//��������
{
  switch(port)
  {
    case 'A':PA_DDR&=~(0X01<<pin);PA_CR1&=~(0X01<<pin);break;
    case 'B':PB_DDR&=~(0X01<<pin);PB_CR1&=~(0X01<<pin);break;
    case 'C':PC_DDR&=~(0X01<<pin);PC_CR1&=~(0X01<<pin);break;
  
  }
}
void Haltmode_GPIODefault(void)//GPIO configue
{
    //PA_DDR=0X00;PA_CR1=0XFF;PA_ODR=0X00;PA_CR2=0X00;
    
   //PB_DDR=0X00;PB_CR1=0XFF;PB_ODR=0X00;PB_CR2=0X00;
    
   //PC_DDR=0X00;PC_CR1=0XFF;PC_ODR=0X00;PC_CR2=0X00;
    
    GPIO_output_init('B',3);
    GPIO_output_init('B',4);
    GPIO_output_init('A',3);
    GPIO_output_init('A',2);
    
   

}

void Halt_RTC_Init()
{

  CLK_PCKENR2|=0x04;// ʹ��RTCʱ��
  PWR_CSR2|=0x06;//�ر��ڲ��ο���ѹ�����ÿ�����Ӧ
  CLK_CRTCR=0x04;//RTCԤ����64 ѡLSI��Ϊʱ��Դ
  RTC_WPR=0xCA;//д��ǰ��Ҫִ�е���Կ
  RTC_WPR=0x53;//ͬ��
  RTC_CR2&=0XfB;//���WUTE
  while((RTC_ISR1&0X04)==0X00);
  RTC_CR1&=~0x0f;//ʮ����Ƶ
  RTC_WUTRH=0x08;//������
  RTC_WUTRL=0x47;//��  ��WUTR *1/(RTCʱ��Դ/��Ƶ)=ʱ�䣩
  RTC_CR2|=0X04;//ʹ�ܻ���ʱ��
  RTC_CR2|=0X40;//ʹ��RTC�ж�
     RTC_ISR2 &=~0X04;

   //RTC_ISR2 =~0X04;

  //RTC_ISR2&=~0X04; 

  RTC_WPR=0xFF;//���� 

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
   //CLK_SWR = 0x02;
   //CLK_CKDIVR =0x03;
    //while (CLK_SCSR!=0x01);
}


void main_Init()
{
  SysClkInit();
  Haltmode_GPIODefault();
  //lock_other();
}
long AD_VALUE=0;

long NUM=0;
long i=0;
int n =2;
int main()
{                                 
  asm( "sim" );  
  main_Init();
  VKL144_Init();
  ads1115_init();
  Halt_RTC_Init();

  
  asm( "rim" );

  while(1)
  {
    
    startch0(4);
    delay(1000);
    AD_VALUE= ads1115_ch0();
    NUM= 3037*AD_VALUE-896900;
    i=NUM/1000;
    VKL144_MyDisplay(i);
    // asm("halt");
  }
}
#pragma vector=6
__interrupt void RTC_IRQHandler(void)
{
  CLK_PCKENR2|=0x04;// ʹ��RTCʱ��
  PWR_CSR2|=0x06;//�ر��ڲ��ο���ѹ�����ÿ�����Ӧ
  CLK_CRTCR=0x04;//RTCԤ����64 ѡLSI��Ϊʱ��Դ
  RTC_WPR=0xCA;//д��ǰ��Ҫִ�е���Կ
  RTC_WPR=0x53;//ͬ��
  RTC_CR2&=0XfB;//���WUTE
  while((RTC_ISR1&0X04)==0X00);
       //PB_ODR=0x00; 
    RTC_CR1&=~0x0f;//ʮ����Ƶ
  RTC_WUTRH=0x18;//������
  RTC_WUTRL=0x47;//��  ��WUTR *1/(RTCʱ��Դ/��Ƶ)=ʱ�䣩
  RTC_CR2|=0X04;//ʹ�ܻ���ʱ��
  RTC_CR2|=0X40;//ʹ��RTC�ж�
     RTC_ISR2 &=~0X04;
  RTC_WPR=0xFF;//���� 

/*Bit 2 WUTF: Periodic wakeup flag
This flag is set by hardware when the wakeup auto-reload down-counter reaches 0.
It is cleared by software by writing 0. 
Note: This flag must be cleared by software at least 1.5 RTCCLK cycles before WUTF is set to 1 
again
*/


}
