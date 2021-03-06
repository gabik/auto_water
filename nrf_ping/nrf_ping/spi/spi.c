/*
 * spi.c
 *
 * Created: 31/07/2015 16:26:01
 *  Author: Gabi Kazav
 */ 

#include "spi.h"

#include <avr/io.h>

#define PORT_SPI    PORTB
#define DDR_SPI     DDRB
#define DD_MISO     PB4
#define DD_MOSI     PB3
#define DD_SS       PB2
#define DD_SCK      PB5

void spi_init()
// Initialize pins for SPI communication
{
	DDR_SPI &= ~(1<<DD_MISO);
	// Define the following pins as output
	DDR_SPI |= ((1<<DD_MOSI)|(1<<DD_SS)|(1<<DD_SCK));
	PORT_SPI |= (1<<DD_MISO) | (1<<DD_SS);
	PORT_SPI &= ~((1<<DD_SCK) | (1<<DD_MOSI)) ;

	SPCR = ((1<<SPE)|       // SPI Enable
	(0<<SPIE)|              // SPI Interupt disable
	(0<<DORD)|              // Data Order (0:MSB first / 1:LSB first)
	(1<<MSTR)|              // Master/Slave select
	(0<<SPR1)|(0<<SPR0)|    // SPI Clock Rate
	(0<<CPOL)|              // Clock Polarity (0:SCK low / 1:SCK hi when idle)
	(0<<CPHA));             // Clock Phase (0:leading / 1:trailing edge sampling)

	SPSR = 0; //(1<<SPI2X);              // Double Clock Rate
}

void spi_transfer_sync (uint8_t * dataout, uint8_t * datain, uint8_t len)
// Shift full array through target device
{
	for (uint8_t i = 0; i < len; i++) {
		SPDR = dataout[i];
		while((SPSR & (1<<SPIF))==0);
		datain[i] = SPDR;
	}
}

void spi_transmit_sync (uint8_t * dataout, uint8_t len)
// Shift full array to target device without receiving any byte
{
	for (uint8_t i = 0; i < len; i++) {
		SPDR = dataout[i];
		while((SPSR & (1<<SPIF))==0);
	}
}

uint8_t spi_fast_shift (uint8_t data)
// Clocks only one byte to target device and returns the received one
{
	SPDR = data;
	while((SPSR & (1<<SPIF))==0);
	return SPDR;
}