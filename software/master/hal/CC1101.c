#include "CC1101.h"
#include "CC1101Regs.h"
#include "HalSpi.h"
#include "HalGPIO.h"

#define WRITE_BURST     	0x40						//连续写入
#define READ_SINGLE     	0x80						//读
#define READ_BURST      	0xC0						//连续读
#define BYTES_IN_RXFIFO     0x7F  						//接收缓冲区的有效字节数
#define CRC_OK              0x80 						//CRC校验通过位标志


#define CC1101_CS_PIN  0x04
#define CC1101_CLK_PIN 0x05
#define CC1101_MI_PIN  0x06
#define CC1101_MO_PIN  0x07
#define CC1101_GOD0_PIN 0x10
#define CC1101_GOD2_PIN 0x11


#define CC1101_CS_ENABLE() HalGPIOSetLevel(CC1101_CS_PIN, HAL_GPIO_LEVEL_LOW)
#define CC1101_CS_DISABLE() HalGPIOSetLevel(CC1101_CS_PIN, HAL_GPIO_LEVEL_HIGH)

static void delay(int nCount)
{
    int i,j;
    for(j = 0; j< nCount; j++)
    {
        for(i = 0; i<10; i++);
    }
}

static uint8_t halSpiReadStatus(uint8_t addr) 
{
    uint8_t value, temp;
	temp = addr | READ_BURST;
    CC1101_CS_ENABLE();
    while(HalGPIOGetLevel(CC1101_MI_PIN));
    HalSpiReadWrite(temp);
	value = HalSpiReadWrite(0xff);
	CC1101_CS_DISABLE();
	return value;
}

static uint8_t halSpiReadReg(uint8_t addr)
{
	uint8_t value;
	CC1101_CS_ENABLE();
    while(HalGPIOGetLevel(CC1101_MI_PIN));
	HalSpiReadWrite(addr | READ_SINGLE);
	value = HalSpiReadWrite(0xff);
    CC1101_CS_DISABLE();
	return value;
}

static void halSpiReadBurstReg(uint8_t addr, uint8_t *buffer, uint8_t len)
{
    uint8_t i;
    CC1101_CS_ENABLE();
    while(HalGPIOGetLevel(CC1101_MI_PIN));
    HalSpiReadWrite(addr | READ_BURST);
    for (i = 0; i < len; i++)
    {
        buffer[i] = HalSpiReadWrite(0xff);
    }
    CC1101_CS_DISABLE();
}

static uint8_t halRfReceivePacket(uint8_t *rxBuffer, uint8_t *length) 
{
    uint8_t status[2];
    uint8_t packetLength;
	 
    if ((halSpiReadStatus(CCxxx0_RXBYTES) & BYTES_IN_RXFIFO)) 
	{
        packetLength = halSpiReadReg(CCxxx0_RXFIFO);
        rxBuffer[0] = packetLength;//读出第一个字节，此字节为该帧数据长度
        if(packetLength <= *length) 
        {
            halSpiReadBurstReg(CCxxx0_RXFIFO, rxBuffer+1, packetLength+2);
            *length = packetLength;

            halSpiStrobe(CCxxx0_SFRX);		
            return 1;//(status[1] & CRC_OK);
        }
        else 
        {
            *length = packetLength;
            halSpiStrobe(CCxxx0_SFRX);		//???????
            return 0;
        }
    } 
 	return 0;
}



static void halSpiWriteReg(uint8_t addr, uint8_t value)
{
    CC1101_CS_ENABLE();
    while(HalGPIOGetLevel(CC1101_MI_PIN));
    HalSpiReadWrite(addr);
    HalSpiReadWrite(value);
    CC1101_CS_DISABLE();
}


static void halSpiWriteBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
    uint8_t i, temp;
	temp = addr | WRITE_BURST;
    CC1101_CS_ENABLE();
    while(HalGPIOGetLevel(CC1101_MI_PIN));
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
    while(HalGPIOGetLevel(CC1101_MI_PIN));
    HalSpiReadWrite(strobe);
    CC1101_CS_DISABLE();
}

static void setRxMode(void)
{
    halSpiStrobe(CCxxx0_SFRX);  //清洗接收缓冲区
    halSpiStrobe(CCxxx0_SIDLE); //强制进入空闲模式
    halSpiStrobe(CCxxx0_SRX);   //进入接收模式
}

uint8_t CC1101ReadID(void)
{
    uint8_t id;
    CC1101_CS_ENABLE();
    while(HalGPIOGetLevel(CC1101_MI_PIN));
	HalSpiReadWrite(0xf1);
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
    while(!HalGPIOGetLevel(CC1101_GOD0_PIN));//while (!GDO0);
    // Wait for GDO0 to be cleared -> end of packet
    while(HalGPIOGetLevel(CC1101_GOD0_PIN));// while (GDO0);
    //  halSpiStrobe(CCxxx0_SFTX);
    setRxMode();
}


static void chipReset(void)
{
    HalGPIOConfig(CC1101_CS_PIN, HAL_IO_OUTPUT);

    CC1101_CS_DISABLE();
    delay(30);
    CC1101_CS_ENABLE();
    delay(30);
    CC1101_CS_DISABLE();
    delay(45);
    CC1101_CS_ENABLE();
    while(HalGPIOGetLevel(CC1101_MI_PIN));
    HalSpiReadWrite(CCxxx0_SRES);
    while(HalGPIOGetLevel(CC1101_MI_PIN));
    CC1101_CS_DISABLE();
}

static void regsConfig(void)
{
	halSpiWriteReg(CCxxx0_IOCFG0,	0x06);  //GDO0 Output Pin Configuration
	halSpiWriteReg(CCxxx0_PKTLEN,	0xFF);  //Packet Length
	halSpiWriteReg(CCxxx0_PKTCTRL0,	0x05);  //Packet Automation Control
	halSpiWriteReg(CCxxx0_ADDR,		0x00);    //Device Address
	halSpiWriteReg(CCxxx0_CHANNR,	0x0A);  //Channel Number
	halSpiWriteReg(CCxxx0_FSCTRL1,	0x06); //Frequency Synthesizer Control
	halSpiWriteReg(CCxxx0_FREQ2,	0x10);   //Frequency Control Word, High Byte
	halSpiWriteReg(CCxxx0_FREQ1,	0xB1);   //Frequency Control Word, Middle Byte
	halSpiWriteReg(CCxxx0_FREQ0,	0x3B);   //Frequency Control Word, Low Byte
	halSpiWriteReg(CCxxx0_MDMCFG4,	0x25); //Modem Configuration
	halSpiWriteReg(CCxxx0_MDMCFG3,	0x83); //Modem Configuration
	halSpiWriteReg(CCxxx0_MDMCFG2,	0x13); //Modem Configuration
	halSpiWriteReg(CCxxx0_DEVIATN,	0x60); //Modem Deviation Setting
	halSpiWriteReg(CCxxx0_MCSM0,	0x18);   //Main Radio Control State Machine Configuration
    halSpiWriteReg(CCxxx0_MCSM1,	0x3f);
	halSpiWriteReg(CCxxx0_FOCCFG,	0x16);  //Frequency Offset Compensation Configuration
	halSpiWriteReg(CCxxx0_WORCTRL,	0xFB); //Wake On Radio Control
	halSpiWriteReg(CCxxx0_FSCAL3,	0xE9);  //Frequency Synthesizer Calibration
	halSpiWriteReg(CCxxx0_FSCAL2,	0x2A);  //Frequency Synthesizer Calibration
	halSpiWriteReg(CCxxx0_FSCAL1,	0x00);  //Frequency Synthesizer Calibration
	halSpiWriteReg(CCxxx0_FSCAL0,	0x1F);  //Frequency Synthesizer Calibration
	halSpiWriteReg(CCxxx0_TEST0,	0x09);   //Various Test Settings
	halSpiWriteReg(CCxxx0_PATABLE,	0xc0);   //Various Test Settings

	halSpiStrobe(CCxxx0_SIDLE);    //CCxxx0_SIDLE	 0x36 //空闲状态
	halSpiStrobe(CCxxx0_SCAL);    //CCxxx0_SIDLE	 0x36 //空闲状态

	halSpiStrobe(CCxxx0_SRX); //enable RX.
}

static void pinExtiConfig(void)
{
    NVIC_InitTypeDef   NVIC_InitStructure;
	
    /* Enable and set EXTI3 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

    EXTI_InitTypeDef EXTI_InitStructure;
    /* Connect GDO0 EXTI Line to Button GPIO Pin */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);  

    /* Configure GDO0 EXTI line */
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

void CC1101Initialize(void)
{
    chipReset();
    regsConfig();
    pinExtiConfig();
}

void CC1101Poll(void)
{
}


void EXTI0_IRQHandler(void)  /* Key 4 */
{
    uint8_t  leng = 20;
    uint8_t i_state;
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        //SYS_EXIT_CRITICAL(i_state);
        if (halRfReceivePacket(g_rx_buf, &leng)) // 读数据并判断正确与否
        {
            halSpiStrobe(CCxxx0_SIDLE);    //CCxxx0_SIDLE	 0x36 //空闲状态
            halSpiStrobe(CCxxx0_SRX);
            g_RFRecv = 1;
        }
       // SYS_ENTER_CRITICAL(i_state);
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

