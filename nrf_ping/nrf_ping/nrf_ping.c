/*
 * nrf_ping.c
 *
 * Transmitter module (get click and send number)
 *
 *
 * Created: 31/07/2015 10:53:54
 *  Author: gabi
 */ 

#define F_CPU 1000000L // For Delay.h

#include "nrf_operations.h"
#include <avr/io.h>
#include <util/delay.h>

#define MY_ID 1 // Reciever #1

void setup()
{
	nrf_init();
	_delay_ms(10);
	nrf_config();
}

void loop() 
{
	uint8_t data[DATA_PAYLOAD]; // Declare the data buffer
	while (!nrf_get(MY_ID, data)) {} // Wait till we got data
	
}

int main(void) { setup(); while(1) loop(); }