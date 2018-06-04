#include "MPUI2C.h"
#include "HalGPIO.h"

#define MPU6500_SCL_PIN 0x1A
#define MPU6500_SDA_PIN 0x1B

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

bool MPUI2CWriteByte(unsigned char slaveAddress,unsigned char REG_Address,unsigned char REG_data)
{
  	if(!I2C_Start())
        return false;
    I2C_SendByte(slaveAddress);
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

int MPUI2CWriteBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data)
{
    uint8_t i;

    if(!I2C_Start())
        return false;
    I2C_SendByte(devAddr);
    if(!I2C_WaitAck())
    {
        I2C_Stop();
        return false;
    }
    I2C_SendByte(regAddr);
    I2C_WaitAck();

    for(i = 0; i < length; i++)
    {
        I2C_SendByte(data[i]);
        I2C_WaitAck();
    }
    I2C_Stop();
    delay5ms();
    return 0;
}

unsigned char MPUI2CReadByte(unsigned char slaveAddress, unsigned char REG_Address)
{
    unsigned char REG_data;

    if(!I2C_Start())
    {
        return false;
    }
    I2C_SendByte(slaveAddress); //I2C_SendByte(((REG_Address & 0x0700) >>7) | REG_Address & 0xFFFE);//¨¦¨¨?????e¨º?¦Ì??¡¤+?¡Â?t¦Ì??¡¤
    if(!I2C_WaitAck())
    {
        I2C_Stop();
        return false;
    }
    I2C_SendByte((uint8_t)REG_Address);   //¨¦¨¨??¦Ì¨ª?e¨º?¦Ì??¡¤
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte(slaveAddress + 1);
    I2C_WaitAck();

    REG_data = I2C_RadeByte();
    I2C_NoAck();
    I2C_Stop();
    //return true;
    return REG_data;

}

int8_t MPUI2CReadBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data)
{
    uint8_t i;

    if(!I2C_Start())
    {
        return false;
    }
    I2C_SendByte(devAddr); //I2C_SendByte(((REG_Address & 0x0700) >>7) | REG_Address & 0xFFFE);//¨¦¨¨?????e¨º?¦Ì??¡¤+?¡Â?t¦Ì??¡¤
    if(!I2C_WaitAck())
    {
        I2C_Stop();
        return false;
    }
    I2C_SendByte(regAddr);   //¨¦¨¨??¦Ì¨ª?e¨º?¦Ì??¡¤
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte(devAddr + 1);
    I2C_WaitAck();

    for(i = 0; i < length - 1; i++)
    {
        data[i] = I2C_RadeByte();
        I2C_WaitAck();
    }
    data[length - 1] = I2C_RadeByte();
    I2C_NoAck();

    I2C_Stop();
    return length;
}

void MPUI2CInitialize(void)
{
    HalGPIOConfig(MPU6500_SCL_PIN, HAL_IO_OUTPUT);
    HalGPIOConfig(MPU6500_SDA_PIN, HAL_IO_OUTPUT);
}

void MPUI2CPoll(void)
{
}

