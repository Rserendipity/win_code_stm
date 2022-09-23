#include "gpio_iic.h"
#include "delay.h"
#include "bitband.h"

/*
用GPIO的方式模拟IIC总线这种方法,和硬件连接的GPIO有关
我的设备接到不同的GPIO上,将来就需要配置响应的GPIO
因为我们做的实验有OLED EEPROM SHT30,而这些设备都是连接在了相同的管脚上
所以,在使用时从设备地址一定不要写错
*/

//以下代码是硬件相关的，移植时候需要修改
GPIO_InitTypeDef  GPIO_InitForSCL;//定义了GPIO初始化的结构体类型变量(时钟线)
GPIO_InitTypeDef  GPIO_InitForSDAOut;//定义了GPIO初始化的结构体类型变量(数据线<发送>)
GPIO_InitTypeDef  GPIO_InitForSDAIn;//定义了GPIO初始化的结构体类型变量(数据线<接收>)

#define IIC_SCL					PBOut(6)
#define IIC_SDA					PBOut(7)
#define IIC_SDA_STATUS 	PBIn(7)

void IIC_Init(void)
{			
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	//通过APB2总线使能GPIOB组的时钟
	
	//PB6 SCL	初始化时钟线,把时钟线配制成推挽的输出
  GPIO_InitForSCL.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitForSCL.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitForSCL.GPIO_Speed = GPIO_Speed_50MHz;
	
	//PB7 SDA OUT 初始化数据线,把数据线配制成推挽的输出
	GPIO_InitForSDAOut.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitForSDAOut.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitForSDAOut.GPIO_Speed = GPIO_Speed_50MHz;
	
  //PB7 SDA IN 初始化数据线,把数据线配制成上拉的输入
  GPIO_InitForSDAIn.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitForSDAIn.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitForSDAIn.GPIO_Speed = GPIO_Speed_50MHz;
	
  GPIO_Init(GPIOB, &GPIO_InitForSDAOut);//先把数据线初始化成输出的模式
  GPIO_Init(GPIOB, &GPIO_InitForSCL);//初始化时钟线
	
	IIC_SCL = 1;//拉高时钟线
	IIC_SDA = 1;//拉高数据线
}

void IIC_Sda_In(void)//把数据线初始化成输入模式
{
	GPIO_Init(GPIOB, &GPIO_InitForSDAIn);
}

void IIC_Sda_Out(void)//把数据线初始化成输出模式
{
	GPIO_Init(GPIOB, &GPIO_InitForSDAOut);
}
//以上代码是硬件相关的，移植时候需要修改

//以下代码硬件无关，只跟iic协议有关
void IIC_Start(void)//产生开始信号
{
	IIC_Sda_Out();	//把数据线配制成输出模式
	IIC_SDA=1;			//把数据线拉高
	IIC_SCL=1;			//把时钟线拉高
	delay_us(2);		//延时2us
 	IIC_SDA=0;			//把数据线拉低
	delay_us(2);		//延时2us
	IIC_SCL=0;			//把时钟线拉低
}	  

void IIC_Stop(void)//产生结束信号
{
	IIC_Sda_Out();	//把数据线配制成输出模式
	IIC_SCL=0;			//把时钟线拉低
	IIC_SDA=0;			//把数据线拉低
 	delay_us(2);		//延时2us
	IIC_SCL=1;			//把时钟线拉高
	delay_us(1);		//延时1us
	IIC_SDA=1;			//把数据线拉高
	delay_us(2);		//延时2us
}

u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;	//定义了超时变量
	
	IIC_Sda_In();		//把数据线配制成输入模式
	IIC_SCL = 0;		//把时钟线拉低
	delay_us(1);		//延时1us
	IIC_SCL = 1;		//把时钟线拉高
	delay_us(1);		//延时1us
	while(IIC_SDA_STATUS)	//读取数据线的高低电平
	{
		ucErrTime++;				//让超时变量自增
		if(ucErrTime > 250)	//判断是否到了超时
		{
			IIC_Stop();				//产生结束信号
			return 1;					//返回1,代表没有等到ACK
		}
	}
	IIC_SCL = 0;					//把时钟线拉低
	return 0;							//返回0,代表等到了ACK
} 

void IIC_Ack(void)			//产生ACK
{
	IIC_SCL=0;						//把时钟线拉低
	IIC_Sda_Out();				//把数据线配制成输出模式
	IIC_SDA=0;						//把数据线拉低
	delay_us(2);					//延时2us
	IIC_SCL=1;						//把时钟线拉高
	delay_us(2);					//延时2us
	IIC_SCL=0;						//把时钟线拉低
}
    
void IIC_NAck(void)			//不产生ACK
{
	IIC_SCL=0;						//把时钟线拉低
	IIC_Sda_Out();				//把数据线配制成输出模式
	IIC_SDA=1;						//把数据线拉高
	delay_us(2);					//延时2us
	IIC_SCL=1;						//把时钟线拉高
	delay_us(2);					//延时2us
	IIC_SCL=0;						//把时钟线拉低
}					 				     

void IIC_Send_Byte(u8 txd)
{
  u8 t;//定义了循环变量t

	IIC_Sda_Out();//把数据线配制成输出模式

  for(t = 0;t < 8; t++)//进入8次循环
	{
		IIC_SCL = 0;					//把时钟线拉低
		delay_us(1);					//延时1us
    IIC_SDA = (txd & 0x80) >> 7;//把最高位的数据放到数据线上
    txd <<= 1;						//让txd保存的数据同步左移一位
		delay_us(1);					//延时1us
		IIC_SCL=1;						//把时钟线拉高
		delay_us(2);					//延时2us
   }	
	IIC_SCL = 0;						//把竖中线拉低
} 	    

u8 IIC_Recv_Byte(u8 ack)
{
	u8 i, receive = 0;//定义u8类型变量
	
	IIC_Sda_In();						//把数据线配制成输入模式
  
	for(i = 0; i < 8; i++)
	{
    IIC_SCL = 0;					//把时钟线拉低 
    delay_us(2);					//延时2us
		IIC_SCL=1;						//把时钟线拉高
		delay_us(2);					//延时2us
    receive <<= 1;				//把receive变量保存的数据同步左移一位
    if(IIC_SDA_STATUS)		//判断从数据线中读出的电平的高低
			receive |= 1;				//如果是高电平,把1存储到receive的最低位
  }					 
	IIC_SCL = 0;						//把时钟线拉低
  if(!ack)								//判断形参的值是否要产生ACK
		IIC_NAck();						//如果形参的值是0就不产生ACK
  else
		IIC_Ack();						//如果形参的值是1就产生ACK
  return receive;
}






