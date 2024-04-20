/**
  ******************************************************************************
  * 模块名称：软件算法模块
  * 文件名	：ALGORITHM.h
  * 说明	：头文件，提供用户自定义数据类型以及供外部调用的接口函数的声明
  * 版本号	：个人版
  * 修改记录：算法参考 https://acuity.blog.csdn.net/article/details/114808763
  ******************************************************************************
  */

#ifndef __ALGORITHM_H_
#define __ALGORITHM_H_

#include "stm32f1xx_hal.h"

/*接口函数声明*/
uint8_t binary_search(const float *buf, uint32_t buf_size, float s_value, uint32_t *out_data);	//二分法查找：阻值-温度区间
float resi_temp_calc(float resi);																//插值法：阻值-温度换算

#endif
