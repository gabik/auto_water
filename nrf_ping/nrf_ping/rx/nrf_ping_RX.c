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
#include "../led_screen/led_screen.h"
#include "../misc/misc.h"
#include <avr/io.h>
#include <util/delay.h>

#define MY_ID 1 // Reciever #1
	
uint16_t count;

void setup_rx()
{
// 	LED_DDR |= (1<<LED_PIN);
// 	LED_PORT &= ~(LED_PIN);
// 	nrf_init();
// 	_delay_ms(10);
// 	nrf_config();
	init_button();
	led_screen_init();
	count = 0;
}

void loop_rx()
{
	//uint8_t data[DATA_PAYLOAD]; // Declare the data buffer
	//while (!nrf_get(MY_ID, data)) {write_to_led(count, 0);} // Wait till we got data
	while (!check_button_click()) {write_to_led(count, 0);} // Wait till we got data
	//if (data[0]) count = data[0];
	count++;
}

int main_rx(void) { setup_rx(); while(1) loop_rx(); }