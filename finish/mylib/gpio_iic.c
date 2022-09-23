#include "gpio_iic.h"
#include "delay.h"
#include "bitband.h"

/*
��GPIO�ķ�ʽģ��IIC�������ַ���,��Ӳ�����ӵ�GPIO�й�
�ҵ��豸�ӵ���ͬ��GPIO��,��������Ҫ������Ӧ��GPIO
��Ϊ��������ʵ����OLED EEPROM SHT30,����Щ�豸��������������ͬ�Ĺܽ���
����,��ʹ��ʱ���豸��ַһ����Ҫд��
*/

//���´�����Ӳ����صģ���ֲʱ����Ҫ�޸�
GPIO_InitTypeDef  GPIO_InitForSCL;//������GPIO��ʼ���Ľṹ�����ͱ���(ʱ����)
GPIO_InitTypeDef  GPIO_InitForSDAOut;//������GPIO��ʼ���Ľṹ�����ͱ���(������<����>)
GPIO_InitTypeDef  GPIO_InitForSDAIn;//������GPIO��ʼ���Ľṹ�����ͱ���(������<����>)

#define IIC_SCL					PBOut(6)
#define IIC_SDA					PBOut(7)
#define IIC_SDA_STATUS 	PBIn(7)

void IIC_Init(void)
{			
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	//ͨ��APB2����ʹ��GPIOB���ʱ��
	
	//PB6 SCL	��ʼ��ʱ����,��ʱ�������Ƴ���������
  GPIO_InitForSCL.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitForSCL.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitForSCL.GPIO_Speed = GPIO_Speed_50MHz;
	
	//PB7 SDA OUT ��ʼ��������,�����������Ƴ���������
	GPIO_InitForSDAOut.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitForSDAOut.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitForSDAOut.GPIO_Speed = GPIO_Speed_50MHz;
	
  //PB7 SDA IN ��ʼ��������,�����������Ƴ�����������
  GPIO_InitForSDAIn.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitForSDAIn.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitForSDAIn.GPIO_Speed = GPIO_Speed_50MHz;
	
  GPIO_Init(GPIOB, &GPIO_InitForSDAOut);//�Ȱ������߳�ʼ���������ģʽ
  GPIO_Init(GPIOB, &GPIO_InitForSCL);//��ʼ��ʱ����
	
	IIC_SCL = 1;//����ʱ����
	IIC_SDA = 1;//����������
}

void IIC_Sda_In(void)//�������߳�ʼ��������ģʽ
{
	GPIO_Init(GPIOB, &GPIO_InitForSDAIn);
}

void IIC_Sda_Out(void)//�������߳�ʼ�������ģʽ
{
	GPIO_Init(GPIOB, &GPIO_InitForSDAOut);
}
//���ϴ�����Ӳ����صģ���ֲʱ����Ҫ�޸�

//���´���Ӳ���޹أ�ֻ��iicЭ���й�
void IIC_Start(void)//������ʼ�ź�
{
	IIC_Sda_Out();	//�����������Ƴ����ģʽ
	IIC_SDA=1;			//������������
	IIC_SCL=1;			//��ʱ��������
	delay_us(2);		//��ʱ2us
 	IIC_SDA=0;			//������������
	delay_us(2);		//��ʱ2us
	IIC_SCL=0;			//��ʱ��������
}	  

void IIC_Stop(void)//���������ź�
{
	IIC_Sda_Out();	//�����������Ƴ����ģʽ
	IIC_SCL=0;			//��ʱ��������
	IIC_SDA=0;			//������������
 	delay_us(2);		//��ʱ2us
	IIC_SCL=1;			//��ʱ��������
	delay_us(1);		//��ʱ1us
	IIC_SDA=1;			//������������
	delay_us(2);		//��ʱ2us
}

u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;	//�����˳�ʱ����
	
	IIC_Sda_In();		//�����������Ƴ�����ģʽ
	IIC_SCL = 0;		//��ʱ��������
	delay_us(1);		//��ʱ1us
	IIC_SCL = 1;		//��ʱ��������
	delay_us(1);		//��ʱ1us
	while(IIC_SDA_STATUS)	//��ȡ�����ߵĸߵ͵�ƽ
	{
		ucErrTime++;				//�ó�ʱ��������
		if(ucErrTime > 250)	//�ж��Ƿ��˳�ʱ
		{
			IIC_Stop();				//���������ź�
			return 1;					//����1,����û�еȵ�ACK
		}
	}
	IIC_SCL = 0;					//��ʱ��������
	return 0;							//����0,����ȵ���ACK
} 

void IIC_Ack(void)			//����ACK
{
	IIC_SCL=0;						//��ʱ��������
	IIC_Sda_Out();				//�����������Ƴ����ģʽ
	IIC_SDA=0;						//������������
	delay_us(2);					//��ʱ2us
	IIC_SCL=1;						//��ʱ��������
	delay_us(2);					//��ʱ2us
	IIC_SCL=0;						//��ʱ��������
}
    
void IIC_NAck(void)			//������ACK
{
	IIC_SCL=0;						//��ʱ��������
	IIC_Sda_Out();				//�����������Ƴ����ģʽ
	IIC_SDA=1;						//������������
	delay_us(2);					//��ʱ2us
	IIC_SCL=1;						//��ʱ��������
	delay_us(2);					//��ʱ2us
	IIC_SCL=0;						//��ʱ��������
}					 				     

void IIC_Send_Byte(u8 txd)
{
  u8 t;//������ѭ������t

	IIC_Sda_Out();//�����������Ƴ����ģʽ

  for(t = 0;t < 8; t++)//����8��ѭ��
	{
		IIC_SCL = 0;					//��ʱ��������
		delay_us(1);					//��ʱ1us
    IIC_SDA = (txd & 0x80) >> 7;//�����λ�����ݷŵ���������
    txd <<= 1;						//��txd���������ͬ������һλ
		delay_us(1);					//��ʱ1us
		IIC_SCL=1;						//��ʱ��������
		delay_us(2);					//��ʱ2us
   }	
	IIC_SCL = 0;						//������������
} 	    

u8 IIC_Recv_Byte(u8 ack)
{
	u8 i, receive = 0;//����u8���ͱ���
	
	IIC_Sda_In();						//�����������Ƴ�����ģʽ
  
	for(i = 0; i < 8; i++)
	{
    IIC_SCL = 0;					//��ʱ�������� 
    delay_us(2);					//��ʱ2us
		IIC_SCL=1;						//��ʱ��������
		delay_us(2);					//��ʱ2us
    receive <<= 1;				//��receive�������������ͬ������һλ
    if(IIC_SDA_STATUS)		//�жϴ��������ж����ĵ�ƽ�ĸߵ�
			receive |= 1;				//����Ǹߵ�ƽ,��1�洢��receive�����λ
  }					 
	IIC_SCL = 0;						//��ʱ��������
  if(!ack)								//�ж��βε�ֵ�Ƿ�Ҫ����ACK
		IIC_NAck();						//����βε�ֵ��0�Ͳ�����ACK
  else
		IIC_Ack();						//����βε�ֵ��1�Ͳ���ACK
  return receive;
}






