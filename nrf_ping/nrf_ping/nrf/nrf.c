/*
 * nrf.c
 *
 * Implementing NRF
 *
 * Created: 31/07/2015 11:39:48
 *  Author: Gabi Kazav
 */ 

#include "nrf.h"
#include "nrf_const.h"
#include "spi.h"
#include <avr/io.h>

// Defines for setting the NRF registers for transmitting or receiving mode
#define TX_POWERUP nrf_config_register((1<<PWR_UP) | (0<<PRIM_RX))
#define RX_POWERUP nrf_config_register((1<<PWR_UP) | (1<<PRIM_RX))
#define POWERDN    nrf_config_register(0)
#define RESET_STT  nrf_write_register(STATUS, ((1<<RX_DR) | (1<<TX_DS) | (1<<MAX_RT)), 1)

// Flag which denotes transmitting mode
volatile uint8_t PTX;

void nrf_init()
// Init NRF - call at start
{
	// Define CSN and CE as Output and set them to default
	NRF_DDR |= ((1<<CSN)|(1<<CE));
	nrf_CE_lo;
	nrf_CSN_hi;
	
	// Initialize spi module
	spi_init();
}


void nrf_config()
// Sets the important registers in the NRF module and powers the module
// TODO: setup SETUP_RETR=0, 
{
	uint8_t RX_PIPE[5] = 0xf0f0f0f0f0;
	uint8_t TX_PIPE[5] = 0xf1f1f1f1f1;
	nrf_write_register(RF_CH, nrf_CH, 1);				// Set RF Channel
	nrf_write_register(RX_PW_P0, nrf_PAYLOAD, 1);		// Set Bytes payload
	nrf_write_register(RF_SETUP, (0b11<<RF_PWR), 1);	// Set 0dBm
	nrf_write_register(EN_AA, 0x00, 1);					// Disable Auto_Ack
	nrf_write_register(EN_RXADDR, (1<<ERX_P0), 1);		// Enable only data pipe 0 for RX
	nrf_write_register(SETUP_AW, (0b11<<AW), 1);		// Enable 5 Byte addresses
	nrf_write_register(RX_ADDR_P0, RX_PIPE, 5);			// Set RX pipe
	nrf_write_register(TX_ADDR, TX_PIPE, 5)				// Set TX pipe
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
	nrf_write_register(CONFIG, nrf_CONFIG | value, 1);
}

void nrf_read_register(uint8_t reg, uint8_t * value, uint8_t len)
// Reads an array of bytes from the given start position in the NRF registers.
{
	nrf_CSN_lo;
	spi_fast_shift(R_REGISTER | (REGISTER_MASK & reg));
	spi_transfer_sync(value, value, len);
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

	nrf_CE_lo;						// Stop listening
	PTX = 1;                        // Set to transmitter mode
	TX_POWERUP;                     // Go to TX mode
	
	nrf_CSN_lo;                    // Pull down chip select
	spi_fast_shift( FLUSH_TX );     // Write cmd to flush tx fifo
	nrf_CSN_hi;                    // Pull up chip select
	
	nrf_CSN_lo;                    // Pull down chip select
	spi_fast_shift( W_TX_PAYLOAD ); // Write cmd to write payload
	spi_transmit_sync(value,nrf_PAYLOAD);   // Write payload
	nrf_CSN_hi;                    // Pull up chip select
	
	nrf_CE_hi;                     // Start transmission
	
	// Back to Listening
	RX_POWERUP;
}

void nrf_get_raw(uint8_t * data)
{
	uint8_t cur_data;
	nrf_CSN_lo;
	spi_fast_shift(R_RX_PAYLOAD);
	for (uint8_t i=0 ; i<nrf_PAYLOAD ; i++) 
	{ 
		cur_data = spi_fast_shift(NOP);
		data[i] = cur_data;
	}
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