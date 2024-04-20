/**
  ******************************************************************************
  * 模块名称：AD7794驱动模块
  * 文件名	：AD7794.c
  * 说明	：c文件，提供用户自定义数据类型、宏和变量以及供外部调用的接口函数的实现
  * 版本号	：个人版
  * 修改记录：1. GPIO_InitStruct.Mode = GPIO_MODE_INPUT ; 模式设置成GPIO_MODE_AF_PP
  *			  2. 等价 HAL_SPI_Transmit(&hspi2,&liming2,sizeof(liming2),1000);     HAL_SPI_Receive(&hspi2,&lmGET_data,sizeof(lmGET_data),1000);
  *			  等价 HAL_SPI_Transmit(&hspi2,&liming,sizeof(liming),1000);		HAL_SPI_TransmitReceive(&hspi2, &liming2, &lmGET_data, 1, 1000);
  *			  3. 写一个SPI字节的时候居然忘记要读取数据了，因为SPI为双线结构，只要时钟线有信号，两条线上都会有信号，
  *			  我就只发送数据，要读取的数据的时候再读取，这样子会造成溢出标志OVR被置1，
  *			  因为平时的数据没有被读取，又来一个，真正要读取的时候，就会发现，怎么都是FF，
  *			  因为发送OVR之后，还是DR的内容不会被修改，所以都是FF。
  ******************************************************************************
  */

#include "AD7794.h"
#include "spi.h"
#include "stdio.h"
#include "tim.h"

#define DLY_TIM_Handle  (&htim5)

/**
  * 函数名称：delay_us
  * 函数功能：微妙延时
  * 入口参数：
  * 返回值	：
  * 注意事项：时钟主频84M，分频84。定时器设置为1MHZ的计数频率，定时器计一个数就是1us
  */
void delay_us(uint16_t nus)
{
	__HAL_TIM_SET_COUNTER(DLY_TIM_Handle, 0);
	__HAL_TIM_ENABLE(DLY_TIM_Handle);
	while (__HAL_TIM_GET_COUNTER(DLY_TIM_Handle) < nus)
	{
	}
	__HAL_TIM_DISABLE(DLY_TIM_Handle);
}

/**
  * 函数名称：SPI2_ReadWriteByte
  * 函数功能：SPI读写函数，发送一个字节，并从寄存器返回一个字节
  * 入口参数：
  * 返回值	：
  * 注意事项：无论读写时，片选信号 跟 时钟信号都是 主spi 提供的
  */
uint8_t SPI_ReadWriteByte(uint8_t TxData)
{
	uint8_t Rxdata;
	HAL_SPI_TransmitReceive(&hspi2, &TxData, &Rxdata, 1, AD7794_TIMEOUT_VALUE);  //参数：SPI号、目标数据地址、接收数据地址、数据长度、超时时间    
 	return Rxdata;          		    
}

/**
  * 函数名称：AD7794_Reset
  * 函数功能：AD7794复位
  * 入口参数：
  * 返回值  ：无
  * 注意事项：可以通过在 DIN 输入上写入一系列 1 来重置串行接口。如果将逻辑 1 写入 AD7794/AD7795 线路至少 32 个串行时钟周期，串行接口将复位。
  * 		  串行时钟周期SCK, 一个串行时钟周期有4个时钟周期 CLK
  */
uint8_t AD7794_Reset(void)
{
	uint8_t status;
	uint8_t cmd_Reset[4] = {0xff,0xff,0xff,0xff};
	uint8_t	buff[4];

	status = HAL_SPI_TransmitReceive(&hspi2, cmd_Reset, buff, 4, AD7794_TIMEOUT_VALUE);//获取SPI通信状态
	switch (status)
	{
		case HAL_OK: printf("AD7794_Reset: Ok \r\n");break;
		case HAL_ERROR: printf("AD7794_Reset: Error \r\n");break;
		case HAL_BUSY: printf("AD7794_Reset: Busy \r\n");break;
		case HAL_TIMEOUT: printf("AD7794_Reset: Timeout \r\n");break;
	}

	return status;
}

/**
  * 函数名称：AD7794_Mode
  * 函数功能：配置 模式寄存器
  * 入口参数：
  * 返回值	：
  * 注意事项：大小 16bit 
  */
uint8_t AD7794_Mode(void)
{
	uint8_t cmd_Mode[3];
	uint8_t buff[3];
	uint8_t	status;
	cmd_Mode[0] = WRITE|MODE;	
	cmd_Mode[1] = MD0_AMP0;		
	cmd_Mode[2] = CLK0_FS167;	

	status = HAL_SPI_TransmitReceive(&hspi2, cmd_Mode, buff, 3, AD7794_TIMEOUT_VALUE);//获取SPI通信状态
	switch (status)
	{
		case HAL_OK: printf("AD7794_Mode: Ok \r\n");break;
		case HAL_ERROR: printf("AD7794_Mode: Error \r\n");break;
		case HAL_BUSY: printf("AD7794_Mode: Busy \r\n");break;
		case HAL_TIMEOUT: printf("AD7794_Mode: Timeout \r\n");break;
	}

	HAL_Delay(1);
	return status;
}

/**
  * 函数名称：AD7794_Configuration
  * 函数功能：配置 配置寄存器
  * 入口参数：
  * 返回值	：
  * 注意事项：大小 16bit 
  */
uint8_t AD7794_Configuration(void)
{
	uint8_t cmd_Configuration[3];
	uint8_t buff[3];
	uint8_t status;
	cmd_Configuration[0] = WRITE|CONFIGURATION;	
	cmd_Configuration[1] = B_GAIN4;				
	cmd_Configuration[2] = BUF_DET;				

	status = HAL_SPI_TransmitReceive(&hspi2, cmd_Configuration, buff, 3, AD7794_TIMEOUT_VALUE);//获取SPI通信状态
	switch (status)
	{
		case HAL_OK: printf("AD7794_Configuration: Ok \r\n");break;
		case HAL_ERROR: printf("AD7794_Configuration: Error \r\n");break;
		case HAL_BUSY: printf("AD7794_Configuration: Busy \r\n");break;
		case HAL_TIMEOUT: printf("AD7794_Configuration: Timeout \r\n");break;
	}

	HAL_Delay(1);
	return status;
}

/**
  * 函数名称：AD7794_IO
  * 函数功能：配置 IO口寄存器
  * 入口参数：
  * 返回值	：
  * 注意事项：大小 8bit 
  */
uint8_t AD7794_IO(void)
{
	uint8_t cmd_IO[2];
	uint8_t buff[2];
	uint8_t status;
	cmd_IO[0] = WRITE|IO;						
	cmd_IO[1] = IO2_210UA_D;				
	
	status = HAL_SPI_TransmitReceive(&hspi2, cmd_IO, buff, 2, AD7794_TIMEOUT_VALUE);//获取SPI通信状态
	switch (status)
	{
		case HAL_OK: printf("AD7794_IO: Ok \r\n");break;
		case HAL_ERROR: printf("AD7794_IO: Error \r\n");break;
		case HAL_BUSY: printf("AD7794_IO: Busy \r\n");break;
		case HAL_TIMEOUT: printf("AD7794_IO: Timeout \r\n");break;
	}

	HAL_Delay(1);
	return status;
}

/**
  * 函数名称：AD7794_Init
  * 函数功能：AD7794 初始化
  * 入口参数：
  * 返回值	：
  * 注意事项：ADC 所有通信必须以对通信寄存器写操作开始，写入通信寄存器的数据决定下一个操作是读还是写
  */
void AD7794_Init(void)
{
	/*等待ADC 复位完成 一连串1 需要32个时钟周期*/
	CS_LOW;
	delay_us(5);
	SPI_ReadWriteByte(0xff);	
	SPI_ReadWriteByte(0xff);	
	SPI_ReadWriteByte(0xff);	
	delay_us(5);
	CS_HIGH;
	HAL_Delay(10);


	CS_LOW;
	delay_us(5);
	/*等待 模式寄存器配置完成*/
	SPI_ReadWriteByte(WRITE|MODE);	//写通信寄存器，告知下一个操作是写模式寄存器	
	SPI_ReadWriteByte(MD0_AMP0);		//连续转换模式，禁用仪放共模，斩波放大器使能
	SPI_ReadWriteByte(CLK0_FS167);	//64KHz 内部时钟，ADC更新速率16.7Hz
	HAL_Delay(1);

	/*等待 配置寄存器配置完成*/
	SPI_ReadWriteByte(WRITE|CONFIGURATION);	//写通信寄存器，告知下一个操作是写配置寄存器
	SPI_ReadWriteByte(B_GAIN4);				//双极性，增益 设置为4
	SPI_ReadWriteByte(BUF_DET);				//缓冲器使能，开启基准电压检测
	HAL_Delay(1);

	/*等待 IO口寄存器配置完成*/
	SPI_ReadWriteByte(WRITE|IO);			//写通信寄存器，告知下一个操作是写IO口寄存器
	SPI_ReadWriteByte(IO2_210UA_D);		//电流源并联到 OUT2 ，提供 210ua*2 的激励电流
	HAL_Delay(1);

	delay_us(5);
	CS_HIGH;
	HAL_Delay(241);			// 片内震荡器上电需要1ms，转换需要2*120ms
}

/**
  * 函数名称：AD7794_GetStatus
  * 函数功能：读取状态寄存器，检查ADC是否转换完成
  * 入口参数：
  * 返回值	：检测结果
  * 注意事项：
  */
uint8_t AD7794_GetStatus(void)
{
	uint8_t status;

	/*读状态寄存器 8bit*/
	SPI_ReadWriteByte(READ|STATUS);  //写通信寄存器，告知下一个操作是读状态寄存器					
	status = SPI_ReadWriteByte(0xff);

	/*检查ADC就绪位，1表示不可读数，0可以读数*/
	if((DATA_RDY & status) != 0)
	{
		printf("AD7794_GetStatus: No ready \r\n");
		return DATA_RDY;
	}

	/*检查ADC错误位，1表示ADC数据寄存器结果全为0或1，可能超量程、欠量程或缺少基准电压*/
	else if((ERR & status) != 0)
	{
		printf("AD7794_GetStatus: An error has occurred \r\n");
		return ERR;
	}
	/*检查ADC基准电压位，1表示ADC无外部基准电压*/
	else if((NOXREF & status) != 0)
	{
		printf("AD7794_GetStatus: No external reference voltage \r\n");
		return NOXREF;
	}
	else
	{
		return AD7794_OK;
	}	
}

/**
  * 函数名称：AD7794_Get_Code
  * 函数功能：ADC 测试数据读取
  * 入口参数：
  * 返回值	：ADC 转换的温度代码
  * 注意事项：每次读取8位，通过位与合成一个24位数据，则定义一个32位的临时变量
  */
uint32_t AD7794_Get_Code()
{
	uint8_t status_GetStatus;
	uint8_t MSB,MINSB,LSB;
	uint32_t temp_Code;

	CS_LOW;
	delay_us(5);

	/*获取寄存器状态*/
	status_GetStatus = AD7794_GetStatus();
	while (AD7794_OK != status_GetStatus)
	{
		HAL_Delay(240);
		status_GetStatus = AD7794_GetStatus();
	}

	/*读数据寄存器 24bit*/
	SPI_ReadWriteByte(READ|DATA);

	/*ADC数据合成 24bit*/
	MSB = SPI_ReadWriteByte(0xff);
	MINSB = SPI_ReadWriteByte(0xff);
	LSB = SPI_ReadWriteByte(0xff);

	delay_us(5);
	CS_HIGH;

	printf("MSB = %#x \r\n", MSB);
	printf("MINSB = %#x \r\n", MINSB);
	printf("LSB = %#x \r\n", LSB);
	temp_Code = (MSB<<16) | (MINSB<<8) | LSB;
	printf("voltage Code = %#x \r\n", temp_Code);

	return temp_Code;
} 

/**
  * 函数名称：AD7794_Code_To_Res
  * 函数功能：ADC 数据代码转换为具体阻值
  * 入口参数：
  * 返回值	：阻值大小
  * 注意事项：先让公式的数值都*1000，这样的话计算出来的阻值也会 *1000，这样计算的小数位就会多三位，精度较为准确，
  * 		  在赋给最后结果变量前再 *0.001，将结果变回正常值即可
  */
uint64_t AD7794_Code_To_Res()
{
	uint64_t Res;
	uint64_t Code;
	Code = AD7794_Get_Code();						//获取数字通信原始代码（电压比值）
	Res = ((Code * 5100000) / ((1<<24) * 4));		//电压转换为电阻值，2^24 = 1<<24,扩大一千倍 单精度 增益为4
//	Res = (((Code / (1<<23)) - 1) * 5100000) / 4;	//电压转换为电阻值，2^24 = 1<<24,扩大一千倍 双精度 增益为4
	printf("Resistance = %lld \r\n", Res);
	return Res;
}

