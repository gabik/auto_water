/*
 * nrf.h
 *
 * Define the funcions
 *
 * Created: 31/07/2015 11:15:52
 *  Author: Gabi Kazav
 */ 


#ifndef NRF_H_
#define NRF_H_

#include <avr/io.h>

// NRF settings
#define nrf_CH         2 // Channel (2.4Ghz + x1Mhz)
#define nrf_PAYLOAD    4 // in Bytes (id, 2xdata, cksum)
#define nrf_CONFIG     ((1<<MASK_MAX_RT) | (1<<MASK_RX_DR) | (MASK_TX_DS)) // Disabling IRQ

// Pin definitions for chip select and chip enabled of the MiRF module
#define NRF_PORT	PORTB
#define NRF_DDR		DDRB
#define CE			PB0
#define CSN			PB1

// Definitions for selecting and enabling MiRF module
#define nrf_CSN_hi     NRF_PORT |=  (1<<CSN);
#define nrf_CSN_lo     NRF_PORT &= ~(1<<CSN);
#define nrf_CE_hi      NRF_PORT |=  (1<<CE );
#define nrf_CE_lo      NRF_PORT &= ~(1<<CE );

// Public functions
extern void		nrf_init();
extern void		nrf_config(uint8_t is_tx);
extern void		nrf_flush();
extern void		nrf_send_raw(uint8_t * value);
extern void		nrf_set_RADDR(uint8_t * adr);
extern void		nrf_set_TADDR(uint8_t * adr);
extern uint8_t	nrf_data_ready();
extern void		nrf_get_raw(uint8_t * data);
extern void		nrf_config_register(uint8_t value);
extern void		nrf_read_register(uint8_t reg, uint8_t * value, uint8_t len);
extern void		nrf_write_register(uint8_t reg, uint8_t * value, uint8_t len);
extern void		nrf_write_register_1(uint8_t reg, uint8_t value);

#endif /* NRF_H_ */