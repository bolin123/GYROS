#include "HalSpi.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"


void HalSpiInitialize(void)
{
    SPI_InitTypeDef   SPI_InitStruct;

    GPIO_InitTypeDef  GPIO_InitStructure;
   /* < Configure SD_SPI pins: SCK MOSI*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
#if 0
	/*< Configure SD_SPI pins: MOSI */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*< Configure SD_SPI pins: MISO */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
    /*!< SD_SPI Config */
	SPI_I2S_DeInit(SPI1);

    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;//SPI_BaudRatePrescaler_32;//chg cbl 0310
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_CRCPolynomial = 7;

    SPI_Init(SPI1, &SPI_InitStruct);
    //SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);
    SPI_I2S_ClearFlag(SPI1, SPI_I2S_FLAG_TXE);
    SPI_I2S_ClearFlag(SPI1, SPI_I2S_FLAG_RXNE);
    SPI_Cmd(SPI1, ENABLE); /*!< SD_SPI enable */

}

void HalSpiPoll(void)
{

}

unsigned char HalSpiReadWrite(unsigned char data)
{
	/* Loop while DR register in not emplty */
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) == RESET);
	/* Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(SPI1, data);
	/* Wait to receive a byte */
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	//while((SPI1->SR & SPI_I2S_FLAG_BSY) == RESET);
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI1);
}
