#include "i2c.h"

// I2Cͨ��ʹ��delay()����
// ����Ƭ��ʵ����Ƶ�޸�
void IICDelay()
{
	unsigned char i = 15;
	while(i--);
}

//���߳�ʼ��
//�����߶��������ͷ�����
void IICInit()
{
	// ��ʼ��IO����CubeMX���ɱȽϷ��㣬�����Ŀ����ֶ���ʼ��
	LL_GPIO_SetOutputPin(SDA_PORT, SDA_GPIO);
	LL_GPIO_SetOutputPin(SCL_PORT, SCL_GPIO);
	IICDelay();
}

//�����ź�
//SCL�ڸߵ�ƽ�ڼ䣬SDA�ɸߵ�ƽ��͵�ƽ�ı仯����Ϊ�����ź�
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

//ֹͣ�ź�
//SCL�ڸߵ�ƽ�ڼ䣬SDA�ɵ͵�ƽ��ߵ�ƽ�ı仯����Ϊֹͣ�ź�
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

//Ӧ���ź�
//��������ݴ����һ����SCL�ĵھŸ�������������Ӧ���źţ��͵�ƽ��ʾӦ�𣬸ߵ�ƽ��Ӧ��
//һ����˵SDA�ϵ�Ӧ���ź�Ӧ����SCL�ĵھŸ�������֮ǰ�ͱ��������ã�����ֱ�Ӷ�ȡSDA����
//�����������õĲ��Ǳ�׼IICЭ�飬��ʱ���ٶȽ������п���SCL�ߵ�ƽ�ڼ䷢��Ӧ��������볬ʱ�ȴ�
//����(SDA == 1)&&(i<255)��ʾ����һ��ʱ����û���յ���������Ӧ����������Ĭ�ϴ������Ѿ��յ����ݶ����ٵȴ�Ӧ���ź�
//��û���ⲿ�ִ��룬�����ͣ�����ʵ�������в������������﷢�������߸�Ϊreturn ture��false���������
//�ȴ����豸����Ӧ��
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

//�����豸����Ӧ��
//���豸�ڷ�����һ�ֽں�SDA���ߣ�SDA���ͷţ������豸��SDA���Ͳ���Ӧ��
void IICMasRes()
{
	LL_GPIO_SetPinMode(SDA_PORT, SDA_GPIO, LL_GPIO_MODE_OUTPUT);
	
	LL_GPIO_ResetOutputPin(SDA_PORT, SDA_GPIO);//ֱ�ӽ�SDA���ͣ��ٲ���SCL
	IICDelay();
	LL_GPIO_SetOutputPin(SCL_PORT, SCL_GPIO);
	IICDelay();
	LL_GPIO_ResetOutputPin(SCL_PORT, SCL_GPIO);
	IICDelay();
}

//���豸��Ӧ��
void IICNoRes()
{
	LL_GPIO_SetPinMode(SDA_PORT, SDA_GPIO, LL_GPIO_MODE_OUTPUT);
	
	LL_GPIO_SetOutputPin(SDA_PORT, SDA_GPIO);//SDA�ߵ�ƽΪ��Ӧ��
	IICDelay();
	LL_GPIO_SetOutputPin(SCL_PORT, SCL_GPIO);
	IICDelay();
	LL_GPIO_ResetOutputPin(SCL_PORT, SCL_GPIO);
	IICDelay();
}

//д��
//���з���һ���ֽ�ʱ����Ҫ������ֽ��е�8λһλһλ�ط���ȥ����temp=temp<<1;��
//��ʾ��temp����һλ�����λ������PSW�Ĵ�����CYλ�У�Ȼ��CY����SDA������SCL�Ŀ����·��ͳ�ȥ
//���SDA���ߣ��Եȴ����豸����Ӧ��
void IICWrite(unsigned char iicdata)
{
	unsigned char i;
	
	LL_GPIO_SetPinMode(SDA_PORT, SDA_GPIO, LL_GPIO_MODE_OUTPUT);

	for(i=0; i<8; i++)
	{
		LL_GPIO_ResetOutputPin(SCL_PORT, SCL_GPIO);
		// IICDelay(); // �����ʱ�����Ż���

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

//����
//���н���һ���ֽ�ʱ���ˡ���8λһλһλ�ؽ��ܣ�Ȼ������ϳ�һ���ֽڣ�
//���붨����k����k����һλ����SDA���С������㣬һ�ΰ�8��������λ����һ���ֽ�������ɽ���
//˵��һ�㣬Ϊʲô����Ҫk=0����䣬��Ϊ���ƺ��λ�Զ����㣬����11111111���ƺ��Ϊ11111110
unsigned char IICRead()
{
	unsigned char i, k;
	
	LL_GPIO_SetPinMode(SDA_PORT, SDA_GPIO, LL_GPIO_MODE_INPUT);
	LL_GPIO_ResetOutputPin(SCL_PORT, SCL_GPIO);
	//�е�оƬ����ʱ�������ⲿ�������裬���ⲿ��������Ͳ���Ҫ��һ��
	//���û���ⲿ������IIC����оƬû���ڲ�������MCU֧���ڲ���������һ��Ҫ��MCU�ڲ������������ȡ����ʱ�����׳���
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


//��I2C�豸д��һ���ֽ�����
void IICWriteByte(unsigned char DeviceAddress, unsigned char dat)
{
	// TODO
}

//��I2C�豸��ȡһ���ֽ�����
unsigned char IICReadByte(unsigned char DeviceAddress)
{
	// TODO
	return 0;
}

//��I2C�豸д�����ֽ�����
void IICWriteBytes(unsigned char DeviceAddress, unsigned char *dat, unsigned char length)
{
	// TODO
}

//��I2C�豸��ȡ����ֽ�����
void IICReadBytes(unsigned char DeviceAddress, unsigned char *dat, unsigned char length)
{
	// TODO
}


//��I2C�豸ָ���Ĵ���д��һ���ֽ�����
void IICWriteRegByte(unsigned char DeviceAddress, unsigned char RegisterAddress, unsigned char dat)
{
	IICStart();//��ʼ�ź�
	IICWrite(DeviceAddress);//�����豸��ַ+д�ź�
	IICSlaRes();
	IICWrite(RegisterAddress);//�ڲ��Ĵ�����ַ��
	IICSlaRes();
	IICWrite(dat);//�ڲ��Ĵ������ݣ�
	IICSlaRes();
	IICStop();//����ֹͣ�ź�
}

//��I2C�豸ָ���Ĵ�����ȡһ���ֽ�����
unsigned char IICReadRegByte(unsigned char DeviceAddress, unsigned char RegisterAddress)
{
	unsigned char dat;
	IICStart();//��ʼ�ź�
	IICWrite(DeviceAddress);//�����豸��ַ+д�ź�
	IICSlaRes();
	IICWrite(RegisterAddress);//���ʹ洢��Ԫ��ַ����0��ʼ	
	IICSlaRes();
	IICStart();//��ʼ�ź�
	IICWrite(DeviceAddress+1);//�����豸��ַ+���ź�
	IICSlaRes();
	dat = IICRead();//�����Ĵ�������
	IICNoRes();//��Ӧ���ź�
	IICStop();//ֹͣ�ź�
	
	return dat;
}

//��I2C�豸ָ���Ĵ���д�����ֽ�����
void IICWriteRegBytes(unsigned char DeviceAddress, unsigned char RegisterAddress, unsigned char *dat, unsigned char length)
{
	unsigned char i;
	
	IICStart();
	IICWrite(DeviceAddress);//�����豸��ַ+д�ź�
	IICSlaRes();
	IICWrite(RegisterAddress);//�ڲ��Ĵ�����ַ��
	IICSlaRes();

	for(i=0; i<length; i++)
	{
		IICWrite(dat[i]);
		IICSlaRes();
	}

	IICStop();
}

//��I2C�豸ָ���Ĵ�����ȡ����ֽ�����
void IICReadRegBytes(unsigned char DeviceAddress, unsigned char RegisterAddress, unsigned char *dat, unsigned char length)
{
	unsigned char i;

	IICStart();
	IICWrite(DeviceAddress);//�����豸��ַ+д�ź�
	IICSlaRes();
	IICWrite(RegisterAddress);//���ʹ洢��Ԫ��ַ
	IICSlaRes();
	
	IICStart();
	IICWrite((unsigned char)(DeviceAddress+1));//�����豸��ַ+���ź�
	//IICWrite(DeviceAddress+1);
	IICSlaRes();

	for(i=0; i<length-1; i++)
	{
		dat[i] = IICRead();
		IICMasRes();
	}
	dat[i] = IICRead();
	IICNoRes();//һ��Ҫ�в�Ӧ���ź�
	IICStop();
}

