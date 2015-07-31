/*
 * tx.c
 *
 * Created: 31/07/2015 16:12:01
 *  Author: Gabi KAzav
 */ 

#include <avr/io.h>
#include "tx.h"
#include "../nrf/nrf_operations.h"

uint8_t check_button_click(uint8_t port, uint8_t pin)
// Check if PIN is GND (pulled up Input)
{
	for (uint8_t i=0 ; i<MIN_COUNT_PRESS ; i++)
	{
		if (port & (1<<pin)) return 0;
	}
	return 1;
}

uint8_t send_counter(uint8_t * clck_count)
{
	clck_count++;
	uint8_t data[DATA_PAYLOAD] = {*clck_count, 0};
	nrf_send(MY_ID, data);
	return 1;
}