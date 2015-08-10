/*
 * tx.c
 *
 * Created: 31/07/2015 16:12:01
 *  Author: Gabi KAzav
 */ 

#include <avr/io.h>
#include "tx.h"
#include "../nrf/nrf_operations.h"

uint8_t send_counter(uint8_t clck_count)
{
	uint8_t data[2] = {clck_count, clck_count};
	nrf_send(1, data);
	return 1;
}