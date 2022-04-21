/*
基于STM32 HAL库编写，使用前请先在CubeMX设置IIC引脚，本代码中不进行初始化设置，只切换读写模式、读写数据等
*/

#ifndef __I2C_C
#define __I2C_C

#include <stdio.h>
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_ll_gpio.h"

// 声明IIC的端口和GPIO号，为了高效使用LL库
// 注意这里不要和CubeMX生成的代码重名
#define SDA_PORT GPIOB
#define SDA_GPIO LL_GPIO_PIN_7
#define SCL_PORT GPIOB
#define SCL_GPIO LL_GPIO_PIN_6

// 相关操作函数
// LL_GPIO_SetOutputPin(SDA_PORT, SDA_GPIO);
// LL_GPIO_ResetOutputPin(SDA_PORT, SDA_GPIO);
// LL_GPIO_SetOutputPin(SCL_PORT, SCL_GPIO);
// LL_GPIO_ResetOutputPin(SCL_PORT, SCL_GPIO);
// LL_GPIO_SetPinMode(SDA_PORT, SDA_GPIO, LL_GPIO_MODE_INPUT);
// LL_GPIO_SetPinPull(SDA_PORT, SDA_GPIO, LL_GPIO_PULL_UP);
// LL_GPIO_SetPinMode(SDA_PORT, SDA_GPIO, LL_GPIO_MODE_OUTPUT);
// LL_GPIO_IsInputPinSet(SDA_PORT, SDA_GPIO);


//I2C通信使用delay()函数
void IICDelay(void);

//总线初始化
void IICInit(void);

//启动信号
void IICStart(void);

//停止信号
void IICStop(void);

//应答信号
unsigned char IICSlaRes(void);

//由主设备发出应答
void IICMasRes(void);

//主设备不应答
void IICNoRes(void);

//写入一字节
void IICWrite(unsigned char dat);

//读取一字节
unsigned char IICRead(void);



//向I2C设备写入一个字节数据
void IICWriteByte(unsigned char DeviceAddress, unsigned char dat);

//从I2C设备读取一个字节数据
unsigned char IICReadByte(unsigned char DeviceAddress);

//向I2C设备写入多个字节数据
void IICWriteBytes(unsigned char DeviceAddress, unsigned char *dat, unsigned char length);

//从I2C设备读取多个字节数据
void IICReadBytes(unsigned char DeviceAddress, unsigned char *dat, unsigned char length);



//向I2C设备指定寄存器写入一个字节数据
void IICWriteRegByte(unsigned char DeviceAddress, unsigned char RegisterAddress, unsigned char dat);

//从I2C设备指定寄存器读取一个字节数据
unsigned char IICReadRegByte(unsigned char DeviceAddress, unsigned char RegisterAddress);

//向I2C设备指定寄存器写入多个字节数据
void IICWriteRegBytes(unsigned char DeviceAddress, unsigned char RegisterAddress, unsigned char *dat, unsigned char length);

//从I2C设备指定寄存器读取多个字节数据
void IICReadRegBytes(unsigned char DeviceAddress, unsigned char RegisterAddress, unsigned char *dat, unsigned char length);

#endif

