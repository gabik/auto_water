/*
 * nrf.c
 *
 * Implementing NRF
 *
 * Created: 31/07/2015 11:39:48
 *  Author: Gabi Kazav
 */ 

#define F_CPU 1000000L

#include "../spi/spi.h"
#include "nrf.h"
#include "nrf_const.h"
#include <avr/io.h>
#include <util/delay.h>

// Defines for setting the NRF registers for transmitting or receiving mode
#define TX_POWERUP nrf_config_register((1<<PWR_UP) & ~(1<<PRIM_RX))
#define RX_POWERUP nrf_config_register((1<<PWR_UP) | (1<<PRIM_RX))
#define POWERDN    nrf_config_register(0)
#define RESET_STT  nrf_write_register_1(STATUS, ((1<<RX_DR) | (1<<TX_DS) | (1<<MAX_RT)))

// Flag which denotes transmitting mode
volatile uint8_t PTX;

void nrf_init()
// Init NRF - call at start
{
	// Define CSN and CE as Output and set them to default
	NRF_DDR |= ((1<<CSN)|(1<<CE));
	
	// Initialize spi module
	spi_init();
	
	nrf_CE_lo;
	nrf_CSN_hi;
}


void nrf_config(uint8_t is_tx)
// Sets the important registers in the NRF module and powers the module
// TODO: setup SETUP_RETR=0, 
{
	uint8_t RX_PIPE[5] = {0xf0, 0xf0, 0xf0, 0xf0, 0xf0};
	uint8_t TX_PIPE[5] = {0xf1, 0xf1, 0xf1, 0xf1, 0xf1};
	nrf_write_register_1(SETUP_RETR, 0);
	nrf_write_register_1(RF_CH, nrf_CH);				// Set RF Channel
	nrf_write_register_1(RX_PW_P0, nrf_PAYLOAD);		// Set Bytes payload
	nrf_write_register_1(RF_SETUP, (0b11<<RF_PWR));		// Set 0dBm and 1MBps
	nrf_write_register_1(EN_AA, 0x00);					// Disable Auto_Ack
	nrf_write_register_1(EN_RXADDR, (1<<ERX_P0));		// Enable only data pipe 0 for RX
	nrf_write_register_1(SETUP_AW, (0b11<<AW));			// Enable 5 Byte addresses
	if (is_tx)
	{
		nrf_write_register(RX_ADDR_P0, RX_PIPE, 5);			// Set RX pipe
		nrf_write_register(TX_ADDR, TX_PIPE, 5);			// Set TX pipe
	} else {
		nrf_write_register(TX_ADDR, RX_PIPE, 5);			// Set RX pipe
		nrf_write_register(RX_ADDR_P0, TX_PIPE, 5);			// Set TX pipe
	}
	RESET_STT;											// Reset all IRQ flags on Status
	nrf_flush();										// Flush RX and TX FIFO's

	// Start receiver
	PTX = 0;        // Start in receiving mode
	RX_POWERUP;     // Power up in receiving mode
	nrf_CE_hi;      // start Listening
}

void nrf_set_RADDR(uint8_t * adr)
// Sets the receiving address
{
	nrf_CE_lo;
	nrf_write_register(RX_ADDR_P0, adr, 5);
	nrf_CE_hi;
}

void nrf_set_TADDR(uint8_t * adr)
// Sets the transmitting address
{
	nrf_write_register(TX_ADDR, adr, 5);
}

extern uint8_t nrf_data_ready()
// Checks if data is available for reading
{
	if (PTX) return 0;
	uint8_t status;
	// Read NRF status
	nrf_CSN_lo;                                // Pull down chip select
	status = spi_fast_shift(NOP);               // Read status register
	nrf_CSN_hi;                                // Pull up chip select
	return status & (1<<RX_DR);
}

void nrf_config_register(uint8_t value)
// OR with nrf_CONFIG const and write CONFIG register new value
{
	nrf_write_register_1(CONFIG, nrf_CONFIG | value);
}

void nrf_read_register(uint8_t reg, uint8_t * value, uint8_t len)
// Reads an array of bytes from the given start position in the NRF registers.
{
	for (uint8_t i=0; i< len ; i++) value[i] = NOP;
	_delay_us(10);
	nrf_CSN_lo;
	_delay_us(10);	
	spi_fast_shift(R_REGISTER | (REGISTER_MASK & reg));
	_delay_us(10);	
	spi_transfer_sync(value, value, len);
	_delay_us(10);	
	nrf_CSN_hi;
}

void nrf_write_register(uint8_t reg, uint8_t * value, uint8_t len)
// Writes an array of bytes into into the NRF registers.
{
	nrf_CSN_lo;
	spi_fast_shift(W_REGISTER | (REGISTER_MASK & reg));
	spi_transmit_sync(value,len);
	nrf_CSN_hi;
}

void nrf_write_register_1(uint8_t reg, uint8_t value)
{
	uint8_t data[1] = {value};
	nrf_write_register(reg, data, 1);
}

void nrf_flush()
// Flush TX and RX FIFOs
{
	while (PTX) {} // Not during sending..
	nrf_CSN_lo;
	spi_fast_shift(FLUSH_TX);
	nrf_CSN_hi;
	
	nrf_CSN_lo;
	spi_fast_shift(FLUSH_RX);
	nrf_CSN_hi;		
}

void nrf_send_raw(uint8_t * value)
// Sends a data package to the TX address.
{
	while (PTX) {}                  // Wait for rx mode

	PTX = 1;                        // Set to transmitter mode
	TX_POWERUP;                     // Go to TX mode
	//RESET_STT;
	//_delay_us(150);
	//nrf_CE_lo;						// Stop listening
	
	//nrf_CSN_lo;                    // Pull down chip select
	//spi_fast_shift( FLUSH_TX );     // Write cmd to flush tx fifo
	//nrf_CSN_hi;                    // Pull up chip select
	//_delay_ms(1);
	nrf_CSN_lo;                    // Pull down chip select
	spi_fast_shift( W_TX_PAYLOAD ); // Write cmd to write payload
	uint8_t value1[4] = {10,10,10,10};
	spi_transfer_sync(value1, value1, 4);   // Write payload
	nrf_CSN_hi;                    // Pull up chip select
	nrf_CE_hi;  
	_delay_us(20);                   // Start transmission	
	nrf_CE_lo;
	// Back to Listening
	PTX = 0;
	RX_POWERUP;
	RESET_STT;
	nrf_CSN_hi;
	_delay_us(130);
}

void nrf_get_raw(uint8_t * data)
{
	//uint8_t cur_data;
	nrf_CSN_lo;
	spi_fast_shift(R_RX_PAYLOAD);
	spi_transfer_sync(data, data, nrf_PAYLOAD);   // read payload
	nrf_CSN_hi;
	RESET_STT;	
}

/*
extern void nrf_get_data(uint8_t * data)
// Reads n-PAYLOAD bytes into data array
{
	nrf_CSN_lo;                               // Pull down chip select
	spi_fast_shift( R_RX_PAYLOAD );            // Send cmd to read rx payload
	spi_transfer_sync(data,data,nrf_PAYLOAD); // Read payload
	nrf_CSN_hi;                               // Pull up chip select
	RESET_STT;   // Reset status register
}
*/

void gabi_send()
{	
	nrf_CE_lo;
	
	nrf_CSN_lo;
	spi_fast_shift(FLUSH_TX);
	nrf_CSN_hi;
	nrf_CSN_lo;
	spi_fast_shift(FLUSH_RX);
	nrf_CSN_hi;
	
	POWERDN;
	_delay_us(130);	
	TX_POWERUP;
	_delay_us(130);
		
	nrf_CSN_lo;                    // Pull down chip select
	spi_fast_shift( W_TX_PAYLOAD ); // Write cmd to write payload
	spi_fast_shift(0x14);
	spi_fast_shift(0x14);
	spi_fast_shift(0x14);
	spi_fast_shift(0x14);
	nrf_CSN_hi;                    // Pull up chip select
	
	nrf_CE_hi;
	_delay_us(10);                   // Start transmission
	nrf_CE_lo;
	
	RX_POWERUP;
	RESET_STT;
	nrf_CE_hi;
	_delay_us(130);
	PORTC|=(1<<PC0);
	_delay_ms(100);
	PORTC&=~(1<<PC0);
	nrf_CSN_hi;
}
