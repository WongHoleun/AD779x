/**
  ******************************************************************************
  * ģ�����ƣ�����㷨ģ��
  * �ļ���	��ALGORITHM.h
  * ˵��	��ͷ�ļ����ṩ�û��Զ������������Լ����ⲿ���õĽӿں���������
  * �汾��	�����˰�
  * �޸ļ�¼���㷨�ο� https://acuity.blog.csdn.net/article/details/114808763
  ******************************************************************************
  */

#ifndef __ALGORITHM_H_
#define __ALGORITHM_H_

#include "stm32f1xx_hal.h"

/*�ӿں�������*/
uint8_t binary_search(const float *buf, uint32_t buf_size, float s_value, uint32_t *out_data);	//���ַ����ң���ֵ-�¶�����
float resi_temp_calc(float resi);																//��ֵ������ֵ-�¶Ȼ���

#endif
