#ifndef __DHT_H
#define __DHT_H
/*
DHT11		PC10
����Ҫ����ģʽ,Ҳ��Ҫ���ģʽ
������ͬһʱ����һ��ģʽ
*/
#include "stm32f10x_conf.h"

extern void dht_init(void);

extern void get_dht_value(char *buf);//��ȡDHT11������������

#endif







