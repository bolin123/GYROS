#include "CC1101.h"
#include "CC1101Regs.h"
#include "HalSpi.h"
#include "HalGPIO.h"

#define CC1101_CS_PIN  0x04
#define CC1101_CLK_PIN 0x05
#define CC1101_MI_PIN  0x06
#define CC1101_MO_PIN  0x07
#define CC1101_GOD0_PIN 0x10
#define CC1101_GOD2_PIN 0x11


#define CC1101_CS_ENABLE() HalGPIOSet(CC1101_CS_PIN, HAL_GPIO_LEVEL_LOW)
#define CC1101_CS_DISABLE() HalGPIOSet(CC1101_CS_PIN, HAL_GPIO_LEVEL_HIGH)

static void delay(int nCount)
{
    int i,j;
    for(j = 0; j< nCount; j++)
    {
        for(i = 0; i<10; i++);
    }
}

static void halSpiWriteReg(uint8_t addr, uint8_t value)
{
    CC1101_CS_ENABLE();
    while(HalGPIOGet(CC1101_MO_PIN));
    HalSpiReadWrite(addr);
    HalSpiReadWrite(value);
    CC1101_CS_DISABLE();
}


static void halSpiWriteBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
    uint8_t i, temp;
	temp = addr | WRITE_BURST;
    CC1101_CS_ENABLE();
    while(HalGPIOGet(CC1101_MO_PIN));
    HalSpiReadWrite(temp);
    for (i = 0; i < count; i++)
 	{
        HalSpiReadWrite(buffer[i]);
    }
    CC1101_CS_DISABLE();
}

static void halSpiStrobe(uint8_t strobe)
{
    CC1101_CS_ENABLE();
    while(HalGPIOGet(CC1101_MO_PIN));
    HalSpiReadWrite(strobe);
    CC1101_CS_DISABLE();
}

uint8_t CC1101ReadID(void)
{
	 uint8_t id;

	 CC1101_CS_ENABLE();
	 HalSpiReadWrite(CCxxx0_SFSTXON);
	 id = HalSpiReadWrite(0xff);
	 CC1101_CS_DISABLE();
	 return id;
}

void CC1101SendData(uint8_t *data, uint16_t len)
{
    halSpiStrobe(CCxxx0_SFTX);
    halSpiWriteBurstReg(CCxxx0_TXFIFO, data, len);
    halSpiStrobe(CCxxx0_STX);

    // Wait for GDO0 to be set -> sync transmitted
    while(!HalGPIOGet(CC1101_GOD0_PIN));//while (!GDO0);
    // Wait for GDO0 to be cleared -> end of packet
    while(HalGPIOGet(CC1101_GOD0_PIN));// while (GDO0);
    //  halSpiStrobe(CCxxx0_SFTX);
}


static void chipReset(void)
{
    CC1101_CS_DISABLE();
    delay(30);
    CC1101_CS_ENABLE();
    delay(30);
    CC1101_CS_DISABLE();
    delay(45);
    CC1101_CS_ENABLE();
    while(HalGPIOGet(CC1101_MO_PIN));
    HalSpiReadWrite(CCxxx0_SRES);
    while(HalGPIOGet(CC1101_MO_PIN));
    CC1101_CS_DISABLE();
}

static void regsConfig(void)
{
	halSpiWriteReg(CCxxx0_IOCFG0,		0x06);  //GDO0 Output Pin Configuration
	halSpiWriteReg(CCxxx0_PKTLEN,		0xFF);  //Packet Length
	halSpiWriteReg(CCxxx0_PKTCTRL0,	    0x05);  //Packet Automation Control
	halSpiWriteReg(CCxxx0_ADDR,			0x00);    //Device Address
	halSpiWriteReg(CCxxx0_CHANNR,		0x0A);  //Channel Number
	halSpiWriteReg(CCxxx0_FSCTRL1,	    0x06); //Frequency Synthesizer Control
	halSpiWriteReg(CCxxx0_FREQ2,		0x10);   //Frequency Control Word, High Byte
	halSpiWriteReg(CCxxx0_FREQ1,		0xB1);   //Frequency Control Word, Middle Byte
	halSpiWriteReg(CCxxx0_FREQ0,		0x3B);   //Frequency Control Word, Low Byte
	halSpiWriteReg(CCxxx0_MDMCFG4,	0x25); //Modem Configuration
	halSpiWriteReg(CCxxx0_MDMCFG3,	0x83); //Modem Configuration
	halSpiWriteReg(CCxxx0_MDMCFG2,	0x13); //Modem Configuration
	halSpiWriteReg(CCxxx0_DEVIATN,	0x60); //Modem Deviation Setting
	halSpiWriteReg(CCxxx0_MCSM0,		0x18);   //Main Radio Control State Machine Configuration
    halSpiWriteReg(CCxxx0_MCSM1,		0x3f);
	halSpiWriteReg(CCxxx0_FOCCFG,		0x16);  //Frequency Offset Compensation Configuration
	halSpiWriteReg(CCxxx0_WORCTRL,	    0xFB); //Wake On Radio Control
	halSpiWriteReg(CCxxx0_FSCAL3,		0xE9);  //Frequency Synthesizer Calibration
	halSpiWriteReg(CCxxx0_FSCAL2,		0x2A);  //Frequency Synthesizer Calibration
	halSpiWriteReg(CCxxx0_FSCAL1,		0x00);  //Frequency Synthesizer Calibration
	halSpiWriteReg(CCxxx0_FSCAL0,		0x1F);  //Frequency Synthesizer Calibration
	halSpiWriteReg(CCxxx0_TEST0,		0x09);   //Various Test Settings
	halSpiWriteReg(CCxxx0_PATABLE,	    0xc0);   //Various Test Settings

	halSpiStrobe(CCxxx0_SIDLE);    //CCxxx0_SIDLE	 0x36 //¿ÕÏÐ×´Ì¬
	halSpiStrobe(CCxxx0_SCAL);    //CCxxx0_SIDLE	 0x36 //¿ÕÏÐ×´Ì¬

	halSpiStrobe(CCxxx0_SRX); //enable RX.
}

void CC1101Initialize(void)
{
    chipReset();
    regsConfig();
}

void CC1101Poll(void)
{
}

