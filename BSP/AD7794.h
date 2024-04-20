/**
  ******************************************************************************
  * 模块名称：模拟温度传感器AD7794驱动模块
  * 文件名	：AD7794.h
  * 说明	：头文件，提供用户自定义数据类型以及供外部调用的接口函数的声明
  * 版本号	：个人版
  * 修改记录：
  ******************************************************************************
  */

#ifndef __AD7794_H
#define __AD7794_H

#include "stm32f1xx.h"
#include "spi.h"

/*SPI 片选输入*/
#define CS_LOW 		HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET)
#define CS_HIGH 		HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET)

/*通信寄存器命令 8bit*/
#define WRITE			0x00	//下一个操作对指定寄存器写
#define READ			0x40	//下一个操作对指定寄存器读

/*寄存器地址选择 8bit，后3位都是0*/
#define COMMUNICATIONS	(0x00<<3)	//写操作期间为通信寄存器
#define STATUS			(0x00<<3)	//读操作期间为状态寄存器
#define MODE			(0x01<<3)	//模式寄存器
#define CONFIGURATION	(0x02<<3)	//配置寄存器
#define DATA			(0x03<<3)	//数据寄存器
#define ID				(0x04<<3)	//ID寄存器
#define IO				(0x05<<3)	//IO寄存器

/*状态寄存器 8bit*/
#define DATA_RDY 	0x80	//转换完成标志，0x80：数据正在转换（不可读）
#define ERR		 	0x40	//ADC数据寄存器结果错误
#define NOXREF	 	0x20	//无效基准电压
#define SR4		 	0x10	//AD7794自动置 0
#define SR3		 	0x08	//AD7794自动置 1
#define CH_AIN3	 	0x0002	//通道3（AIN+,AIN-）

	
/*模式寄存器命令 16bit*/
#define MD0_AMP0		0x00	//连续转换模式，仪放共模不使能，斩波使能
#define MD1_AMP0		0x20	//单次转换模式，仪放共模不使能，斩波使能
#define CLK0_FS417		0x0F	//64KHz内部时钟，ADC更新速率4.17Hz
#define CLK0_FS167		0x0A	//64KHz内部时钟，ADC更新速率16.7Hz
//---------------
#define MD0_AMP1		0x02	//连续转换模式，仪表放大器共模位使能
#define CLK0_CHOP1_FS0	0x12	//64KHz 内部时钟，CLK 不提供时钟，斩波放大器禁用，更新速率不变

/*配置寄存器命令 16bit*/
#define B_GAIN4 		0x12	//单极性,4倍增益
#define U_GAIN4 		0x02	//双极性,4倍增益
#define B_GAIN2 		0x11	//单极性,2倍增益
#define BUF_AIN3 	0x12	//REFIN1加外部基准电压，基准电压检测禁用，缓冲器使能，通道 AIN3
#define BUF_DET 		0x32	//REFIN1加外部基准电压，基准电压检测启用，缓冲器使能，通道 AIN3
//---------------
#define VBIAS3_B_G2 		0xDA	//偏置电压发生器连接 AIN3，熔断电流禁用，单极性，偏置电压发生器功耗提高，增益4
#define REFSEL0_BUF1_CH3 	0x12	//REFIN1加外部基准电压，基准电压检测禁用，缓冲器使能，通道 AIN3
	
/*数据存器命令 24bit*/	
#define DATA_MASK		0xFF	//全双工，发送 3 次 0xff，即可返回 24bit 测量数据
	
/*IO寄存器 8bit*/
#define IO2_210UA	0x02	//电流源IEXC2 连接到 OUT2 ，提供 210 的激励电流 
#define IO2_210UA_D 	0x0E	//电流源并联到 OUT2 ，提供 210ua*2 的激励电流 1110 

/*工作状态*/
#define AD7794_OK			((uint8_t)0x00)
#define AD7794_ERROR		((uint8_t)0x01)
#define AD7794_BUSY			((uint8_t)0x02)
#define AD7794_TIMEOUT		((uint8_t)0x03)

#define AD7794_TIMEOUT_VALUE 1000				//最大超时时间


/*接口函数声明*/
uint8_t SPI_ReadWriteByte(uint8_t TxData);	//SPI读写
uint8_t AD7794_Reset(void);						//复位
uint8_t AD7794_Mode(void);						//配置 模式寄存器
uint8_t AD7794_Configuration(void);				//配置 配置寄存器
uint8_t AD7794_IO(void);						//配置 IO寄存器
void AD7794_Init(void);						//初始化配置
uint32_t AD7794_Get_Code(void);				//获取ADC采集的电压
uint64_t AD7794_Code_To_Res(void);			//ADC采集的电压转阻值

#endif
