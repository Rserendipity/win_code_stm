#ifndef __DHT_H
#define __DHT_H
/*
DHT11		PC10
既需要输入模式,也需要输出模式
但是在同一时间是一种模式
*/
#include "stm32f10x_conf.h"

extern void dht_init(void);

extern void get_dht_value(char *buf);//获取DHT11传感器的数据

#endif







