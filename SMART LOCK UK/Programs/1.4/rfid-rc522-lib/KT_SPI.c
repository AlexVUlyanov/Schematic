#include "KT_SPI.h"
#include "stm8l15x_conf.h"
#include <stdio.h>


uint8_t spi_send_byte(uint8_t data) 
{
	while (SPI_GetFlagStatus(SPI1,SPI_FLAG_TXE)== RESET);
	SPI_SendData(SPI1,data);
	while (SPI_GetFlagStatus(SPI1,SPI_FLAG_RXNE) == RESET);
	return SPI_ReceiveData(SPI1);
}

void spi_init(uint8_t setup) {
	SPI_DeInit(SPI1);
	SPI_Init(SPI1,SPI_FirstBit_MSB,SPI_BaudRatePrescaler_2,SPI_Mode_Master,SPI_CPOL_Low,SPI_CPHA_1Edge,SPI_Direction_2Lines_FullDuplex, SPI_NSS_Soft,(uint8_t)0x07);
	SPI_Cmd(SPI1,ENABLE);
}


uint8_t spi_read_byte(uint8_t data) 
{	while (SPI_GetFlagStatus(SPI1,SPI_FLAG_TXE)== RESET);
	SPI_SendData(SPI1,data);
	while (SPI_GetFlagStatus(SPI1,SPI_FLAG_RXNE) == RESET);
	return SPI_ReceiveData(SPI1);
}