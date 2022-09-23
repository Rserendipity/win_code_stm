#include "dht.h"
#include "bitband.h"
#include "delay.h"

void set_gpio_output(void)//��PC10�ܽų�ʼ�������ģʽ
{
	GPIO_InitTypeDef Gpio_Value;//������GPIO��ʼ���Ľṹ�����
	
	Gpio_Value.GPIO_Mode = GPIO_Mode_Out_PP;//ѡ������������ģʽ
	Gpio_Value.GPIO_Pin = GPIO_Pin_10;//ѡ����10�Źܽ�
	Gpio_Value.GPIO_Speed = GPIO_Speed_50MHz;//ѡ����50MHz������ٶ�
	GPIO_Init(GPIOC, &Gpio_Value);//�����������ó�ʼ��GPIOC��Ĺܽ�
}

void set_gpio_input(void)//��PC10�ܽų�ʼ��������ģʽ
{
	GPIO_InitTypeDef Gpio_Value;//������GPIO��ʼ���Ľṹ�����
	
	Gpio_Value.GPIO_Mode = GPIO_Mode_IN_FLOATING;//ѡ���˸��յ�����ģʽ
	Gpio_Value.GPIO_Pin = GPIO_Pin_10;//ѡ����10�Źܽ�
	GPIO_Init(GPIOC, &Gpio_Value);//�����������ó�ʼ��GPIOC��Ĺܽ�
}

void dht_output_status(int state)
{
	set_gpio_output();//��PC10�ܽų�ʼ�������ģʽ
	if(state == 1)
		PCOut(10) = 1;
	else
		PCOut(10) = 0;
}

int dht_input_status(void)
{
	set_gpio_input();//��PC10�ܽų�ʼ��������ģʽ
	return PCIn(10);//��PC10�ܽŵĵ�ƽ�ߵͷ���
}

void dht_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	//ͨ��APB2����ʹ��GPIOC���ʱ��
}

void get_dht_value(char *buf)//��ȡDHT11������������
{
	u8 flag = 0, time = 0;
	u8 i = 0, tmp = 0;
	
	dht_output_status(1);//������������
	dht_output_status(0);//������������
	delay_ms(20);//��ʱ20ms
	dht_output_status(1);//������������
	do
	{
		flag = dht_input_status();//��ȡ�����ߵĸߵ͵�ƽ״̬
		time++;//������������
		delay_us(2);//��ʱ2us
	}while(flag != 0 && time <= 20);//�Գ�ʱ����ķ�ʽ�ȴ������߱�����
	if(time > 20)//�ж��Ƿ�ʱ
		return;//�����ʱֱ���˳�
	while(!dht_input_status());//�����ȵķ�ʽ�ȴ������߱�����
	///////////////��ʼ����40bit����////////////////
	for(i = 0; i < 40; i++)
	{
		while(dht_input_status());//�ȴ������߱�����
		while(!dht_input_status());//�ȴ������߱�����
		delay_us(40);//��ʱ40us
		tmp <<= 1;//��tmp����ÿһλ������1λ
		if(dht_input_status() == 1)//�ж��������Ƿ��Ǹߵ�ƽ
			tmp |= 1;//��������
		if((i + 1) % 8 == 0)//�ж��Ƿ���յ���8bit����
		{
			buf[i / 8] = tmp;//��tmp�����洢�����ݽ������
			tmp = 0;//��tmp����
		}
	}
	dht_output_status(1);//������������
}














