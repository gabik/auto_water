/*
 * nrf_operations.c
 *
 * mplement specific NRF operartions
 *
 * Created: 31/07/2015 14:31:52
 *  Author: Gabi Kazav
 */ 

#define F_CPU 1000000L

#include "nrf.h"
#include "nrf_const.h"
#include "nrf_operations.h"
#include <avr/io.h>
#include <util/delay.h>
#include "../led_screen/led_screen.h"

void nrf_send(uint8_t id, uint8_t * data)
{
	// build XOR
	uint8_t xor = id;
	for (uint8_t i=0 ; i<DATA_PAYLOAD ; i++) { xor ^= data[i]; }
	
	// build raw packet
	uint8_t raw[nrf_PAYLOAD];
	raw[0] = id;
	for (uint8_t i=0 ; i<DATA_PAYLOAD ; i++) { raw[i+1] = data[i]; }
	raw[nrf_PAYLOAD - 1] = xor;
	
	//write_to_led_hex(raw[0], raw[1], 500);
	//write_to_led_hex(raw[2], raw[3], 500);	
	PORTC |= (1<<PC0);
	_delay_ms(100);
	PORTC &= ~(1<<PC0);	
	nrf_send_raw(raw);
}

uint8_t nrf_get(uint8_t my_id, uint8_t * data)
{
	uint8_t valid = 0;
	
	// Reset data to 0
	for (uint8_t i=0 ; i<DATA_PAYLOAD ; i++) { data[i] = 0; }
	
	// Check if we have data 
	if (nrf_data_ready())
	{
		uint8_t raw[nrf_PAYLOAD];
		nrf_get_raw(raw);
		if (raw[0] == my_id)
		{
			uint8_t xor = my_id;
			for (uint8_t i=0 ; i<DATA_PAYLOAD ; i++) { xor ^= raw[i+1]; }
			if (xor == raw[nrf_PAYLOAD - 1])
			{
				for (uint8_t i=0 ; i<DATA_PAYLOAD ; i++) { data[i] = raw[i+1]; }			
				valid = 1;
			}
		}
	}
	return valid;
}