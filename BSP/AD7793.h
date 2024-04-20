/*
 * @Author: Haolin Wang
 * @Date: 2023-03-20 15:56:14
 * @LastEditTime: 2023-05-03 15:12:56
 * @Description: 模拟温度传感器AD7793驱动模块
 */

#ifndef __AD7793_H
#define __AD7793_H

#include "stm32f1xx.h"
#include "spi.h"

/*SPI 片选输入*/
#define CS_LOW HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET)
#define CS_HIGH HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET)

/*通信寄存器命令 8bit*/
#define WRITE 0x00 // 下一个操作对指定寄存器写
#define READ 0x40  // 下一个操作对指定寄存器读

/*寄存器地址选择 8bit，后3位都是0*/
#define COMMUNICATIONS (0x00 << 3) // 写操作期间为通信寄存器
#define STATUS (0x00 << 3)         // 读操作期间为状态寄存器
#define MODE (0x01 << 3)           // 模式寄存器
#define CONFIGURATION (0x02 << 3)  // 配置寄存器
#define DATA (0x03 << 3)           // 数据寄存器
#define ID (0x04 << 3)             // ID寄存器
#define IO (0x05 << 3)             // IO寄存器

/*状态寄存器 8bit*/
#define DATA_RDY 0x80  // 转换完成标志，0x80：数据正在转换（不可读）
#define ERR 0x40       // ADC数据寄存器结果错误

/*模式寄存器命令 16bit*/
#define MD0_AMP0 0x00   // 连续转换模式，仪放共模位清零
#define MD1_AMP0 0x20   // 单次转换模式，仪放共模位清零（AD794：当斩波置1禁用时仪放也应置1）
#define CLK0_FS417 0x0F // 斩波使能（置0斩波使能），64KHz内部时钟，ADC更新速率4.17Hz
#define CLK0_FS167 0x0A // 斩波使能（AD7793默认置零），64KHz内部时钟，ADC更新速率16.7Hz

/*配置寄存器命令 16bit*/
#define B_GAIN4 0x12  // 单极性,4倍增益
#define BUF_AIN3 0x10 // REFIN加外部基准电压，基准电压检测禁用（AD7793无基准电压检测默认为0），缓冲器使能，通道 AIN1

/*IO寄存器 8bit*/
#define IO1_210UA_D 0x0A // 电流源并联到 OUT1 ，提供 210ua*2 的激励电流 1010

/*工作状态*/
#define AD7793_OK ((uint8_t)0x00)
#define AD7793_ERROR ((uint8_t)0x01)
#define AD7793_BUSY ((uint8_t)0x02)
#define AD7793_TIMEOUT ((uint8_t)0x03)

#define AD7793_TIMEOUT_VALUE 1000 // 最大超时时间

/*接口函数声明*/
uint8_t SPI_ReadWriteByte(uint8_t TxData); // SPI读写
void AD7793_Init(SPI_HandleTypeDef *hspi); // 初始化配置
uint32_t AD7793_Get_Code(void);            // 获取ADC采集的电压
uint64_t AD7793_Code_To_Res(void);         // ADC采集的电压转阻值

#endif
