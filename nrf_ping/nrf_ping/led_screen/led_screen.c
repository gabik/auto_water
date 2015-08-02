/*
 * led_screen.c
 *
 * Created: 01/08/2015 02:44:43
 *  Author: gabi
 */ 

#include "led_screen.h"
#include <avr/io.h>
#include <avr/delay.h>

uint8_t digits[10] = {DG0, DG1, DG2, DG3, DG4, DG5, DG6, DG7, DG8, DG9};
uint8_t led_pn[4]  = {LED0, LED1, LED2, LED3};
	
void led_screen_init()
{
	SEGMENTS_DDR = 0xFF;
	LED_SELECT_DDR |= (1<<LED0) | (1<<LED1) | (1<<LED2) | (1<<LED3);
	poweroff_led();
}

void poweroff_led()
{
	_delay_ms(1);
	SEGMENTS_PORT = 0xFF; // + to cathode to power off
	LED_SELECT_PORT &= ~((1<<LED0) | (1<<LED1) | (1<<LED2) | (1<<LED3)); // 0 to LED ports (0 to Anode)
}

void write_to_led(uint16_t num, uint8_t digit)
{
	uint8_t d[4];
	devide_4_digits(num, d);

	poweroff_led();
	SEGMENTS_PORT = digits[d[0]];
	LED_SELECT_PORT |= (1<<LED0);
	poweroff_led();	
	SEGMENTS_PORT = digits[d[1]];
	LED_SELECT_PORT |= (1<<LED1);
	poweroff_led();	
	SEGMENTS_PORT = digits[d[2]];
	LED_SELECT_PORT |= (1<<LED2);
	poweroff_led();
	SEGMENTS_PORT = digits[d[3]];
	LED_SELECT_PORT |= (1<<LED3);
	poweroff_led();	
	
// 	for (uint8_t i=0; i<4 ; i++)
// 	{
// 		//poweroff_led();
// 		SEGMENTS_PORT = digits[d[i]];
// 		LED_SELECT_PORT |= led_pn[i];
// 	}
	//poweroff_led();
}

void devide_4_digits(uint16_t num, uint8_t * d)
{
	uint8_t d1=0, d2=0, d3=0, d4=0;
	while (num >= 1000) { num-=1000; d1++; }
	while (num >= 100)	{ num -=100; d2++; }
	while (num >= 10)	{ num -= 10; d3++; }
	while (num > 0)		{ num -= 1 ; d4++; }
	d[3]=d4; d[2]=d3; d[1]=d2; d[0]=d1;
}