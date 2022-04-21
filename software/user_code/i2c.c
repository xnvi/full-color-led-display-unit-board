#include "i2c.h"

// I2C通信使用delay()函数
// 按单片机实际主频修改
void IICDelay()
{
	unsigned char i = 15;
	while(i--);
}

//总线初始化
//将总线都拉高以释放总线
void IICInit()
{
	// 初始化IO口由CubeMX生成比较方便，不放心可以手动初始化
	LL_GPIO_SetOutputPin(SDA_PORT, SDA_GPIO);
	LL_GPIO_SetOutputPin(SCL_PORT, SCL_GPIO);
	IICDelay();
}

//启动信号
//SCL在高电平期间，SDA由高电平向低电平的变化定义为启动信号
void IICStart()
{
	LL_GPIO_SetPinMode(SDA_PORT, SDA_GPIO, LL_GPIO_MODE_OUTPUT);

	LL_GPIO_SetOutputPin(SDA_PORT, SDA_GPIO);
	IICDelay();
	LL_GPIO_SetOutputPin(SCL_PORT, SCL_GPIO);
	IICDelay();
	LL_GPIO_ResetOutputPin(SDA_PORT, SDA_GPIO);
	IICDelay();
}

//停止信号
//SCL在高电平期间，SDA由低电平向高电平的变化定义为停止信号
void IICStop()
{
	LL_GPIO_SetPinMode(SDA_PORT, SDA_GPIO, LL_GPIO_MODE_OUTPUT);

	LL_GPIO_ResetOutputPin(SDA_PORT, SDA_GPIO);
	IICDelay();
	LL_GPIO_SetOutputPin(SCL_PORT, SCL_GPIO);
	IICDelay();
	LL_GPIO_SetOutputPin(SDA_PORT, SDA_GPIO);
	IICDelay();
}

//应答信号
//在完成数据传输后，一般是SCL的第九个周期用来传输应答信号，低电平表示应答，高电平不应答
//一般来说SDA上的应答信号应该在SCL的第九个上升沿之前就被器件设置，这里直接读取SDA即可
//部分器件采用的不是标准IIC协议，或时钟速度较慢，有可能SCL高电平期间发送应答，这里加入超时等待
//其中(SDA == 1)&&(i<255)表示若在一段时间内没有收到从器件的应答则主器件默认从器件已经收到数据而不再等待应答信号
//若没有这部分代码，程序会停在这里，实际运用中不允许这种情里发生，或者改为return ture或false，方便调试
//等待从设备产生应答
unsigned char IICSlaRes()
{
	unsigned char i = 255;
	
	LL_GPIO_SetPinMode(SDA_PORT, SDA_GPIO, LL_GPIO_MODE_INPUT);
	LL_GPIO_SetPinPull(SDA_PORT, SDA_GPIO, LL_GPIO_PULL_UP);

	LL_GPIO_SetOutputPin(SCL_PORT, SCL_GPIO);
	IICDelay();

	while(LL_GPIO_IsInputPinSet(SDA_PORT, SDA_GPIO) && i)
	{
		i--;
	}

	if(i == 0)
	{
		IICStop();
		printf("no sla res \n");
		return 1;
	}

	LL_GPIO_ResetOutputPin(SCL_PORT, SCL_GPIO);
	IICDelay();

	return 0;
}

//由主设备发出应答
//从设备在发送完一字节后将SDA拉高（SDA被释放），主设备将SDA拉低产生应答
void IICMasRes()
{
	LL_GPIO_SetPinMode(SDA_PORT, SDA_GPIO, LL_GPIO_MODE_OUTPUT);
	
	LL_GPIO_ResetOutputPin(SDA_PORT, SDA_GPIO);//直接将SDA拉低，再操纵SCL
	IICDelay();
	LL_GPIO_SetOutputPin(SCL_PORT, SCL_GPIO);
	IICDelay();
	LL_GPIO_ResetOutputPin(SCL_PORT, SCL_GPIO);
	IICDelay();
}

//主设备不应答
void IICNoRes()
{
	LL_GPIO_SetPinMode(SDA_PORT, SDA_GPIO, LL_GPIO_MODE_OUTPUT);
	
	LL_GPIO_SetOutputPin(SDA_PORT, SDA_GPIO);//SDA高电平为不应答
	IICDelay();
	LL_GPIO_SetOutputPin(SCL_PORT, SCL_GPIO);
	IICDelay();
	LL_GPIO_ResetOutputPin(SCL_PORT, SCL_GPIO);
	IICDelay();
}

//写入
//串行发送一个字节时，需要把这个字节中的8位一位一位地发出去，“temp=temp<<1;”
//表示将temp左移一位，最高位将移入PSW寄存器的CY位中，然后将CY赋给SDA进而在SCL的控制下发送出去
//最后将SDA拉高，以等待从设备产生应答
void IICWrite(unsigned char iicdata)
{
	unsigned char i;
	
	LL_GPIO_SetPinMode(SDA_PORT, SDA_GPIO, LL_GPIO_MODE_OUTPUT);

	for(i=0; i<8; i++)
	{
		LL_GPIO_ResetOutputPin(SCL_PORT, SCL_GPIO);
		// IICDelay(); // 这个延时可以优化掉

		if(iicdata & 0x80)
		{
			LL_GPIO_SetOutputPin(SDA_PORT, SDA_GPIO);
		}
		else
		{
			LL_GPIO_ResetOutputPin(SDA_PORT, SDA_GPIO);
		}
		iicdata <<= 1;
		IICDelay();
		
		LL_GPIO_SetOutputPin(SCL_PORT, SCL_GPIO);
		IICDelay();
	}
	LL_GPIO_ResetOutputPin(SCL_PORT, SCL_GPIO);
	IICDelay();
	LL_GPIO_SetOutputPin(SDA_PORT, SDA_GPIO);
	IICDelay();
}

//读出
//串行接受一个字节时需了、将8位一位一位地接受，然后再组合成一个字节，
//代码定义了k，将k左移一位后与SDA进行“或”运算，一次把8个独立的位放入一个字节中来完成接收
//说明一点，为什么不需要k=0的语句，因为左移后低位自动补零，例如11111111左移后变为11111110
unsigned char IICRead()
{
	unsigned char i, k;
	
	LL_GPIO_SetPinMode(SDA_PORT, SDA_GPIO, LL_GPIO_MODE_INPUT);
	LL_GPIO_ResetOutputPin(SCL_PORT, SCL_GPIO);
	//有的芯片输入时必须有外部上拉电阻，有外部上拉电阻就不需要这一句
	//如果没有外部上拉，IIC器件芯片没有内部上拉，MCU支持内部上拉，则一定要让MCU内部上拉，否则读取数据时很容易出错
	LL_GPIO_SetPinPull(SDA_PORT, SDA_GPIO, LL_GPIO_PULL_UP);
	
	IICDelay();
	for(i=0; i<8; i++)
	{
		LL_GPIO_SetOutputPin(SCL_PORT, SCL_GPIO);
		IICDelay();
		k <<= 1;
		k |= LL_GPIO_IsInputPinSet(SDA_PORT, SDA_GPIO);
		LL_GPIO_ResetOutputPin(SCL_PORT, SCL_GPIO);
		IICDelay();
	}
	IICDelay();
	return k;
}


//向I2C设备写入一个字节数据
void IICWriteByte(unsigned char DeviceAddress, unsigned char dat)
{
	// TODO
}

//从I2C设备读取一个字节数据
unsigned char IICReadByte(unsigned char DeviceAddress)
{
	// TODO
	return 0;
}

//向I2C设备写入多个字节数据
void IICWriteBytes(unsigned char DeviceAddress, unsigned char *dat, unsigned char length)
{
	// TODO
}

//从I2C设备读取多个字节数据
void IICReadBytes(unsigned char DeviceAddress, unsigned char *dat, unsigned char length)
{
	// TODO
}


//向I2C设备指定寄存器写入一个字节数据
void IICWriteRegByte(unsigned char DeviceAddress, unsigned char RegisterAddress, unsigned char dat)
{
	IICStart();//起始信号
	IICWrite(DeviceAddress);//发送设备地址+写信号
	IICSlaRes();
	IICWrite(RegisterAddress);//内部寄存器地址，
	IICSlaRes();
	IICWrite(dat);//内部寄存器数据，
	IICSlaRes();
	IICStop();//发送停止信号
}

//从I2C设备指定寄存器读取一个字节数据
unsigned char IICReadRegByte(unsigned char DeviceAddress, unsigned char RegisterAddress)
{
	unsigned char dat;
	IICStart();//起始信号
	IICWrite(DeviceAddress);//发送设备地址+写信号
	IICSlaRes();
	IICWrite(RegisterAddress);//发送存储单元地址，从0开始	
	IICSlaRes();
	IICStart();//起始信号
	IICWrite(DeviceAddress+1);//发送设备地址+读信号
	IICSlaRes();
	dat = IICRead();//读出寄存器数据
	IICNoRes();//非应答信号
	IICStop();//停止信号
	
	return dat;
}

//向I2C设备指定寄存器写入多个字节数据
void IICWriteRegBytes(unsigned char DeviceAddress, unsigned char RegisterAddress, unsigned char *dat, unsigned char length)
{
	unsigned char i;
	
	IICStart();
	IICWrite(DeviceAddress);//发送设备地址+写信号
	IICSlaRes();
	IICWrite(RegisterAddress);//内部寄存器地址，
	IICSlaRes();

	for(i=0; i<length; i++)
	{
		IICWrite(dat[i]);
		IICSlaRes();
	}

	IICStop();
}

//从I2C设备指定寄存器读取多个字节数据
void IICReadRegBytes(unsigned char DeviceAddress, unsigned char RegisterAddress, unsigned char *dat, unsigned char length)
{
	unsigned char i;

	IICStart();
	IICWrite(DeviceAddress);//发送设备地址+写信号
	IICSlaRes();
	IICWrite(RegisterAddress);//发送存储单元地址
	IICSlaRes();
	
	IICStart();
	IICWrite((unsigned char)(DeviceAddress+1));//发送设备地址+读信号
	//IICWrite(DeviceAddress+1);
	IICSlaRes();

	for(i=0; i<length-1; i++)
	{
		dat[i] = IICRead();
		IICMasRes();
	}
	dat[i] = IICRead();
	IICNoRes();//一定要有不应答信号
	IICStop();
}

