#ifndef __key_H
#define __key_H
#include "stm32f10x_conf.h"

//��ʼ��
void key_init(void);

//
// int key_stat(int a);
// //ɨ��a����״̬ ���� 1    ���� 0

/// <summary>
/// ��ⰴ��״̬��û�а�������ʱ���ص�����һ�εİ���
/// Ĭ���ǰ��� 1
/// ���簴�°��� 2����һ��û�а����������ذ��� 2
/// </summary>
/// <returns> ���ص��ǰ��������£�ȡֵֻ���� 1�� 2  </returns>
int key_stat(void);

#endif
