#include "MPU6500.h"
#include "HalGPIO.h"

#define	SMPLRT_DIV		0x19	//陀螺仪采样率，典型值：0x07(125Hz)
#define	CONFIG			0x1A	//低通滤波频率，典型值：0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define	ACCEL_CONFIG	0x1C	//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
#define	PWR_MGMT_1		0x6B	//电源管理，典型值：0x00(正常启用)

#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48


#define MPU6500_SCL_PIN 0x1A
#define MPU6500_SDA_PIN 0x1B
#define MPU6500_AD0_PIN 0x1D

#define MPU6500_IIC_ADDR 0xD0

//pb10
#define SCL_H         HalGPIOSetLevel(MPU6500_SCL_PIN, 1)
#define SCL_L         HalGPIOSetLevel(MPU6500_SCL_PIN, 0)

//pb11
#define SDA_H         HalGPIOSetLevel(MPU6500_SDA_PIN, 1)
#define SDA_L         HalGPIOSetLevel(MPU6500_SDA_PIN, 0)

#define SCL_read      HalGPIOGetLevel(MPU6500_SCL_PIN)
#define SDA_read      HalGPIOGetLevel(MPU6500_SDA_PIN)

static void I2C_delay(void)
{
    uint8_t i=30;

    while(i)
    {
        i--;
    }
}

static void delay5ms(void)
{
    int i=5000;
    while(i)
    {
        i--;
    }
}

static bool I2C_Start(void)
{
	SDA_H;
	SCL_H;
	I2C_delay();
	if(!SDA_read)
        return false;
	SDA_L;
	I2C_delay();
	if(SDA_read)
        return false;
	SDA_L;
	I2C_delay();
	return true;
}

static void I2C_Stop(void)
{
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_H;
	I2C_delay();
}

static void I2C_Ack(void)
{
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}

static void I2C_NoAck(void)
{
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}

static bool I2C_WaitAck(void)
{
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	if(SDA_read)
	{
        SCL_L;
        I2C_delay();
        return false;
	}
	SCL_L;
	I2C_delay();
	return true;
}

static void I2C_SendByte(uint8_t SendByte)
{
    uint8_t i=8;
    while(i--)
    {
        SCL_L;
        I2C_delay();
        if(SendByte&0x80)
        {
            SDA_H;
        }
        else
        {
            SDA_L;
        }
        SendByte<<=1;
        I2C_delay();
        SCL_H;
        I2C_delay();
    }
    SCL_L;
}

static unsigned char I2C_RadeByte(void)
{
    uint8_t i=8;
    uint8_t ReceiveByte=0;

    SDA_H;
    while(i--)
    {
        ReceiveByte<<=1;
        SCL_L;
        I2C_delay();
        SCL_H;
        I2C_delay();
        if(SDA_read)
        {
            ReceiveByte|=0x01;
        }
    }
    SCL_L;
    return ReceiveByte;
}

static bool Single_Write(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data)
{
  	if(!I2C_Start())
        return false;
    I2C_SendByte(SlaveAddress);
    if(!I2C_WaitAck())
    {
        I2C_Stop();
        return false;
    }
    I2C_SendByte(REG_Address);
    I2C_WaitAck();
    I2C_SendByte(REG_data);
    I2C_WaitAck();
    I2C_Stop();
    delay5ms();
    return true;
}

static unsigned char Single_Read(unsigned char SlaveAddress, unsigned char REG_Address)
{
    unsigned char REG_data;

    if(!I2C_Start())
    {
        return false;
    }
    I2C_SendByte(SlaveAddress); //I2C_SendByte(((REG_Address & 0x0700) >>7) | REG_Address & 0xFFFE);//éè?????eê?μ??·+?÷?tμ??·
    if(!I2C_WaitAck())
    {
        I2C_Stop();
        return false;
    }
    I2C_SendByte((uint8_t)REG_Address);   //éè??μí?eê?μ??·
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte(SlaveAddress + 1);
    I2C_WaitAck();

    REG_data= I2C_RadeByte();
    I2C_NoAck();
    I2C_Stop();
    //return true;
    return REG_data;

}

void MPU6500GyroRead(uint16_t xyz[3])
{
    xyz[0] = Single_Read(MPU6500_IIC_ADDR, GYRO_XOUT_H);
    xyz[0] = xyz[0] << 8;
    xyz[0] |= Single_Read(MPU6500_IIC_ADDR, GYRO_XOUT_L);

    xyz[1] = Single_Read(MPU6500_IIC_ADDR, GYRO_YOUT_H);
    xyz[1] = xyz[1] << 8;
    xyz[1] |= Single_Read(MPU6500_IIC_ADDR, GYRO_YOUT_L);

    xyz[2] = Single_Read(MPU6500_IIC_ADDR, GYRO_ZOUT_H);
    xyz[2] = xyz[2] << 8;
    xyz[2] |= Single_Read(MPU6500_IIC_ADDR, GYRO_ZOUT_L);
}


static void ioConfig(void)
{
    HalGPIOConfig(MPU6500_SCL_PIN, HAL_IO_OUTPUT);
    HalGPIOConfig(MPU6500_SDA_PIN, HAL_IO_OUTPUT);

    HalGPIOConfig(MPU6500_AD0_PIN, HAL_IO_OUTPUT);
    HalGPIOSetLevel(MPU6500_AD0_PIN, 0); //address 0xd0
}

static void regsConfig(void)
{
    static uint8_t check = 0;

    Single_Write(MPU6500_IIC_ADDR, PWR_MGMT_1, 0x00);
	Single_Write(MPU6500_IIC_ADDR, SMPLRT_DIV, 0x07);
	Single_Write(MPU6500_IIC_ADDR, CONFIG, 0x06);
	Single_Write(MPU6500_IIC_ADDR, GYRO_CONFIG, 0x18);
	Single_Write(MPU6500_IIC_ADDR, ACCEL_CONFIG, 0x01);

    check = Single_Read(MPU6500_IIC_ADDR, GYRO_CONFIG);
}

void MPU6500Initialize(void)
{
    ioConfig();
    regsConfig();
}

void MPU6500Poll(void)
{
}

