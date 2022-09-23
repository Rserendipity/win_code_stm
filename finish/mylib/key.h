#ifndef __key_H
#define __key_H
#include "stm32f10x_conf.h"

//初始化
void key_init(void);

//
// int key_stat(int a);
// //扫描a按键状态 返回 1    返回 0

/// <summary>
/// 检测按键状态，没有按键按下时返回的是上一次的按键
/// 默认是按键 1
/// 例如按下按键 2，下一次没有按按键，返回按键 2
/// </summary>
/// <returns> 返回的是按键几按下，取值只会有 1和 2  </returns>
int key_stat(void);

#endif
