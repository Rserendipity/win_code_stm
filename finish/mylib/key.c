#include "key.h"
// key 1 PC9
void key_init(void)
{
	GPIO_InitTypeDef key_value;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	key_value.GPIO_Mode = GPIO_Mode_IPU;
	//…œ¿≠ ‰»Î
	key_value.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8;
	GPIO_Init(GPIOC, &key_value);
}
int key_stat()
{
	static int key_status = 1;
	if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9) == 0)
	{
		key_status = 1;
	}
	if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8) == 0)
	{
		key_status = 2;
	}

	// switch (a)
	// {
	// case 0:
	// 	b = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9);
	// 	break;
	// case 1:
	// 	b = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8);
	// 	break;
	// case 2:
	// 	//	b = GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9);
	// 	break;
	// }

	return key_status;
}
