/**
****************************************************************************************
* @Description: AD7793驱动模块
* @Author: Haolin Wang
* @Date: 2023-03-20 15:56:14
* @LastEditTime: 2023-03-25 17:33:42
* @Note:
* 			1. GPIO_InitStruct.Mode = GPIO_MODE_INPUT ; 模式设置成GPIO_MODE_AF_PP
*			2. HAL_SPI_Transmit(&hspi2,&liming2,sizeof(liming2),1000);     HAL_SPI_Receive(&hspi2,&lmGET_data,sizeof(lmGET_data),1000);
*			等价 HAL_SPI_Transmit(&hspi2,&liming,sizeof(liming),1000);		HAL_SPI_TransmitReceive(&hspi2, &liming2, &lmGET_data, 1, 1000);
*			3. 写一个SPI字节的时候居然忘记要读取数据了，因为SPI为双线结构，只要时钟线有信号，两条线上都会有信号，
*			我就只发送数据，要读取的数据的时候再读取，这样子会造成溢出标志OVR被置1，
*			因为平时的数据没有被读取，又来一个，真正要读取的时候，就会发现，怎么都是FF，
*			因为发送OVR之后，还是DR的内容不会被修改，所以都是FF。
****************************************************************************************
*/

#include "AD7793.h"
#include "spi.h"
#include "stdio.h"

#define DLY_TIM_Handle (&htim3)

SPI_HandleTypeDef *AD7793_SPI;

/**
****************************************************************************************
* @Funticon name: SPI读写函数
* @Berif: 发送一个字节，并从寄存器返回一个字节
* @Note: 无论读写时，片选信号 跟 时钟信号都是 主spi 提供的
* @param {uint8_t} TxData
* @return {*}
****************************************************************************************
*/
uint8_t SPI_ReadWriteByte(uint8_t TxData)
{
	uint8_t Rxdata;
	HAL_SPI_TransmitReceive(&hspi2, &TxData, &Rxdata, 1, AD7793_TIMEOUT_VALUE); // 参数：SPI号、目标数据地址、接收数据地址、数据长度、超时时间
	return Rxdata;
}

/**
****************************************************************************************
* @Funticon name: AD7794 初始化
* @Berif:
* @Note: ADC 所有通信必须以对通信寄存器写操作开始，写入通信寄存器的数据决定下一个操作是读还是写
* @param {SPI_HandleTypeDef} *hspi
* @return {*}
****************************************************************************************
*/
void AD7793_Init(SPI_HandleTypeDef *hspi)
{
	AD7793_SPI = hspi;
	/*等待ADC 复位完成 一连串1 需要32个时钟周期*/
	CS_LOW;
	HAL_Delay(1);
	SPI_ReadWriteByte(0xff);
	SPI_ReadWriteByte(0xff);
	SPI_ReadWriteByte(0xff);
	HAL_Delay(1);
	CS_HIGH;
	HAL_Delay(10);

	CS_LOW;
	HAL_Delay(1);
	/*等待 模式寄存器配置完成*/
	SPI_ReadWriteByte(WRITE | MODE); // 写通信寄存器，告知下一个操作是写模式寄存器
	SPI_ReadWriteByte(MD0_AMP0);	 // 连续转换模式，禁用仪放共模，斩波放大器使能
	SPI_ReadWriteByte(CLK0_FS167);	 // 64KHz 内部时钟，ADC更新速率16.7Hz
	HAL_Delay(1);

	/*等待 配置寄存器配置完成*/
	SPI_ReadWriteByte(WRITE | CONFIGURATION); // 写通信寄存器，告知下一个操作是写配置寄存器
	SPI_ReadWriteByte(B_GAIN4);				  // 单极性，增益 设置为4
	SPI_ReadWriteByte(BUF_AIN3);				  // 缓冲器使能，开启基准电压检测
	HAL_Delay(1);

	/*等待 IO口寄存器配置完成*/
	SPI_ReadWriteByte(WRITE | IO);	// 写通信寄存器，告知下一个操作是写IO口寄存器
	SPI_ReadWriteByte(IO1_210UA_D); // 电流源并联到 OUT2 ，提供 210ua*2 的激励电流
	HAL_Delay(1);

	HAL_Delay(1);
	CS_HIGH;
	HAL_Delay(241); // 片内震荡器上电需要1ms，转换需要2*120ms
}

/**
****************************************************************************************
* @Funticon name: 读取状态寄存器，检查ADC是否转换完成
* @Berif:
* @Note:
* @return {*}	检测结果
****************************************************************************************
*/
uint8_t AD7793_GetStatus(void)
{
	uint8_t status;

	/*读状态寄存器 8bit*/
	SPI_ReadWriteByte(READ | STATUS); // 写通信寄存器，告知下一个操作是读状态寄存器
	status = SPI_ReadWriteByte(0xff);

	/*检查ADC就绪位，1表示不可读数，0可以读数*/
	if ((DATA_RDY & status) != 0)
	{
		return DATA_RDY;
	}

	/*检查ADC错误位，1表示ADC数据寄存器结果全为0或1，可能超量程、欠量程或缺少基准电压*/
	else if ((ERR & status) != 0)
	{
		return ERR;
	}
	else
	{
		return AD7793_OK;
	}
}

/**
****************************************************************************************
* @Funticon name: ADC 测试数据读取
* @Berif: 获取电压值
* @Note: 每次读取8位，通过位与合成一个24位数据，则定义一个32位的临时变量
* @return {*}		ADC 转换的温度代码
****************************************************************************************
*/
uint32_t AD7793_Get_Code()
{
	uint8_t status_GetStatus;
	uint8_t MSB, MINSB, LSB;
	uint32_t temp_Code;

	CS_LOW;
	HAL_Delay(1);

	/*获取寄存器状态*/
	status_GetStatus = AD7793_GetStatus();
	while (AD7793_OK != status_GetStatus)
	{
		HAL_Delay(240);
		status_GetStatus = AD7793_GetStatus();
	}

	/*读数据寄存器 24bit*/
	SPI_ReadWriteByte(READ | DATA);

	/*ADC数据合成 24bit*/
	MSB = SPI_ReadWriteByte(0xff);
	MINSB = SPI_ReadWriteByte(0xff);
	LSB = SPI_ReadWriteByte(0xff);

	HAL_Delay(1);
	CS_HIGH;
	temp_Code = (MSB << 16) | (MINSB << 8) | LSB;

	return temp_Code;
}

/**
****************************************************************************************
* @Funticon name: ADC 数据代码转换为具体阻值
* @Berif:
* @Note: 先让公式的数值都*1000，这样的话计算出来的阻值也会 *1000，这样计算的小数位就会多三位，精度较为准确，
* 在赋给最后结果变量前再 *0.001，将结果变回正常值即可
* @return {*}		阻值大小
****************************************************************************************
*/
uint64_t AD7793_Code_To_Res()
{
	uint64_t Res;
	uint64_t Code;
	Code = AD7793_Get_Code();					// 获取数字通信原始代码（电压比值）
	Res = ((Code * 5100000) / ((1 << 24) * 4)); // 电压转换为电阻值，2^24 = 1<<24,扩大一千倍 单精度 增益为4
	return Res;
}
