/*
 * nrf_operations.c
 *
 * mplement specific NRF operartions
 *
 * Created: 31/07/2015 14:31:52
 *  Author: Gabi Kazav
 */ 

#include "nrf/nrf.h"
#include "nrf_const.h"
#include "nrf_operations.h"
#include <avr/io.h>

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
	
	nrf_send_raw(raw);
}

void nrf_get(uint8_t my_id, uint8_t * data)
{
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
			}
		}
	}
}