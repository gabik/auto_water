/*
 * tx.c
 *
 * Created: 31/07/2015 16:12:01
 *  Author: Gabi KAzav
 */ 

#include <avr/io.h>
#include "tx.h"
#include "../nrf/nrf_operations.h"

uint8_t send_counter(uint8_t * clck_count)
{
	clck_count++;
	uint8_t data[DATA_PAYLOAD] = {*clck_count, 0};
	nrf_send(MY_ID, data);
	return 1;
}