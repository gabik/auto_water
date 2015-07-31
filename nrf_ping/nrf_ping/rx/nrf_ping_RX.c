/*
 * nrf_ping_RX.c
 *
 * Receiver module (get data and blink its count)
 *
 * Created: 31/07/2015 10:53:54
 *  Author: Gabi Kazav
 */ 

#define F_CPU 1000000L // For Delay.h

#include "../nrf/nrf_operations.h"
#include "nrf_ping_RX.h"
#include <avr/io.h>
#include <util/delay.h>

#define MY_ID 1 // Reciever #1

void setup_rx()
{
	LED_DDR |= (1<<LED_PIN);
	LED_PORT &= ~(LED_PIN);
	nrf_init();
	_delay_ms(10);
	nrf_config();
}

void loop_rx() 
{
	uint8_t data[DATA_PAYLOAD]; // Declare the data buffer
	while (!nrf_get(MY_ID, data)) {} // Wait till we got data
	uint8_t count = data[0];
	for (uint8_t i=0 ; i<count; i++)
	{
		LED_PORT |= (1<<LED_PIN);
		_delay_ms(100);
		LED_PORT &= ~(1<<LED_PIN);
		_delay_ms(100);
	}
}

int main_rx(void) { setup_rx(); while(1) loop_rx(); }