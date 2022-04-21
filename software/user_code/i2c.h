/*
����STM32 HAL���д��ʹ��ǰ������CubeMX����IIC���ţ��������в����г�ʼ�����ã�ֻ�л���дģʽ����д���ݵ�
*/

#ifndef __I2C_C
#define __I2C_C

#include <stdio.h>
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_ll_gpio.h"

// ����IIC�Ķ˿ں�GPIO�ţ�Ϊ�˸�Чʹ��LL��
// ע�����ﲻҪ��CubeMX���ɵĴ�������
#define SDA_PORT GPIOB
#define SDA_GPIO LL_GPIO_PIN_7
#define SCL_PORT GPIOB
#define SCL_GPIO LL_GPIO_PIN_6

// ��ز�������
// LL_GPIO_SetOutputPin(SDA_PORT, SDA_GPIO);
// LL_GPIO_ResetOutputPin(SDA_PORT, SDA_GPIO);
// LL_GPIO_SetOutputPin(SCL_PORT, SCL_GPIO);
// LL_GPIO_ResetOutputPin(SCL_PORT, SCL_GPIO);
// LL_GPIO_SetPinMode(SDA_PORT, SDA_GPIO, LL_GPIO_MODE_INPUT);
// LL_GPIO_SetPinPull(SDA_PORT, SDA_GPIO, LL_GPIO_PULL_UP);
// LL_GPIO_SetPinMode(SDA_PORT, SDA_GPIO, LL_GPIO_MODE_OUTPUT);
// LL_GPIO_IsInputPinSet(SDA_PORT, SDA_GPIO);


//I2Cͨ��ʹ��delay()����
void IICDelay(void);

//���߳�ʼ��
void IICInit(void);

//�����ź�
void IICStart(void);

//ֹͣ�ź�
void IICStop(void);

//Ӧ���ź�
unsigned char IICSlaRes(void);

//�����豸����Ӧ��
void IICMasRes(void);

//���豸��Ӧ��
void IICNoRes(void);

//д��һ�ֽ�
void IICWrite(unsigned char dat);

//��ȡһ�ֽ�
unsigned char IICRead(void);



//��I2C�豸д��һ���ֽ�����
void IICWriteByte(unsigned char DeviceAddress, unsigned char dat);

//��I2C�豸��ȡһ���ֽ�����
unsigned char IICReadByte(unsigned char DeviceAddress);

//��I2C�豸д�����ֽ�����
void IICWriteBytes(unsigned char DeviceAddress, unsigned char *dat, unsigned char length);

//��I2C�豸��ȡ����ֽ�����
void IICReadBytes(unsigned char DeviceAddress, unsigned char *dat, unsigned char length);



//��I2C�豸ָ���Ĵ���д��һ���ֽ�����
void IICWriteRegByte(unsigned char DeviceAddress, unsigned char RegisterAddress, unsigned char dat);

//��I2C�豸ָ���Ĵ�����ȡһ���ֽ�����
unsigned char IICReadRegByte(unsigned char DeviceAddress, unsigned char RegisterAddress);

//��I2C�豸ָ���Ĵ���д�����ֽ�����
void IICWriteRegBytes(unsigned char DeviceAddress, unsigned char RegisterAddress, unsigned char *dat, unsigned char length);

//��I2C�豸ָ���Ĵ�����ȡ����ֽ�����
void IICReadRegBytes(unsigned char DeviceAddress, unsigned char RegisterAddress, unsigned char *dat, unsigned char length);

#endif

