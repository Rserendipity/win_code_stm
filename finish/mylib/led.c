#include "led.h"
#include "bitband.h"

void led_init(void)
{
	GPIO_InitTypeDef Led_Value;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	/*通过APB2总线使能GPIOC组的时钟*/

	Led_Value.GPIO_Mode = GPIO_Mode_Out_PP;					   //选择推挽的输出模式
	Led_Value.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3; //选择1号管脚2号管脚3号管脚
	Led_Value.GPIO_Speed = GPIO_Speed_50MHz;				   //选择50MHz的输出速度

	GPIO_Init(GPIOC, &Led_Value); //按照上述配置初始化GPIOC组的管脚

	PCOut(1) = 0;
	PCOut(2) = 0;
	PCOut(3) = 0;
	//把GPIOC组的相应管脚拉低
}

void led_on(int nu)
{
	switch (nu)
	{
	case 0:
		PCOut(1) = 1;
		break;
	case 1:
		PCOut(2) = 1;
		break;
	case 2:
		PCOut(3) = 1;
		break;
	}
}

void led_off(int nu)
{
	switch (nu)
	{
	case 0:
		PCOut(1) = 0;
		break;
	case 1:
		PCOut(2) = 0;
		break;
	case 2:
		PCOut(3) = 0;
		break;
	}
}
void led_ss(int a)
{
	int i, n;
	led_init();
	for (i = 0; i < 3; i = (i + 1) % 3)
	{
		led_on(i);
		for (n = 500000; n > 0; n--)
			;
		led_off(i);
	}
}
