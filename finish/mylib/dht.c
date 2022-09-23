#include "dht.h"
#include "bitband.h"
#include "delay.h"

void set_gpio_output(void)//把PC10管脚初始化成输出模式
{
	GPIO_InitTypeDef Gpio_Value;//定义了GPIO初始化的结构体变量
	
	Gpio_Value.GPIO_Mode = GPIO_Mode_Out_PP;//选择了推挽的输出模式
	Gpio_Value.GPIO_Pin = GPIO_Pin_10;//选择了10号管脚
	Gpio_Value.GPIO_Speed = GPIO_Speed_50MHz;//选择了50MHz的输出速度
	GPIO_Init(GPIOC, &Gpio_Value);//按照上述配置初始化GPIOC组的管脚
}

void set_gpio_input(void)//把PC10管脚初始化成输入模式
{
	GPIO_InitTypeDef Gpio_Value;//定义了GPIO初始化的结构体变量
	
	Gpio_Value.GPIO_Mode = GPIO_Mode_IN_FLOATING;//选择了浮空的输入模式
	Gpio_Value.GPIO_Pin = GPIO_Pin_10;//选择了10号管脚
	GPIO_Init(GPIOC, &Gpio_Value);//按照上述配置初始化GPIOC组的管脚
}

void dht_output_status(int state)
{
	set_gpio_output();//把PC10管脚初始化成输出模式
	if(state == 1)
		PCOut(10) = 1;
	else
		PCOut(10) = 0;
}

int dht_input_status(void)
{
	set_gpio_input();//把PC10管脚初始化成输入模式
	return PCIn(10);//把PC10管脚的电平高低返回
}

void dht_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	//通过APB2总线使能GPIOC组的时钟
}

void get_dht_value(char *buf)//获取DHT11传感器的数据
{
	u8 flag = 0, time = 0;
	u8 i = 0, tmp = 0;
	
	dht_output_status(1);//把数据线拉高
	dht_output_status(0);//把数据线拉低
	delay_ms(20);//延时20ms
	dht_output_status(1);//把数据线拉高
	do
	{
		flag = dht_input_status();//读取数据线的高低电平状态
		time++;//计数变量自增
		delay_us(2);//延时2us
	}while(flag != 0 && time <= 20);//以超时处理的方式等待数据线被拉低
	if(time > 20)//判断是否超时
		return;//如果超时直接退出
	while(!dht_input_status());//以死等的方式等待数据线被拉高
	///////////////开始传输40bit数据////////////////
	for(i = 0; i < 40; i++)
	{
		while(dht_input_status());//等待数据线被拉低
		while(!dht_input_status());//等待数据线被拉高
		delay_us(40);//延时40us
		tmp <<= 1;//让tmp变量每一位都左移1位
		if(dht_input_status() == 1)//判断数据线是否还是高电平
			tmp |= 1;//接收数据
		if((i + 1) % 8 == 0)//判断是否接收到了8bit数据
		{
			buf[i / 8] = tmp;//把tmp变量存储的数据进行另存
			tmp = 0;//把tmp清零
		}
	}
	dht_output_status(1);//把数据线拉高
}














