/*
 * main.c
 *
 * Control which main to run
 *
 * Created: 31/07/2015 17:01:53
 *  Author: Gabi Kazav
 *
 * -- PORTB ---
 * NRF SPI = AVR SPI
 * NRF CE  = PB0
 * NRF CSN = PB1 
 *
 * -- PORTC ---
 * LED SCREEN LED0 = PC5
 * LED SCREEN LED1 = PC4
 * LED SCREEN LED2 = PC3
 * LED SCREEN LED3 = PC2
 * BTN INPUT       = PC1 ?
 * LED STATUS      = PC0 ?
 *
 * -- PORTD ---
 * LED SEGMENT A = PD0
 * LED SEGMENT B = PD1
 * LED SEGMENT C = PD2
 * LED SEGMENT D = PD3
 * LED SEGMENT E = PD4
 * LED SEGMENT F = PD5
 * LED SEGMENT G = PD6
 * LED SEGMENT H = PD7
 */ 

#include "tx/nrf_ping_TX.h"
#include "rx/nrf_ping_RX.h"

#define F_CPU 1000000L

int main(void) { return main_rx(); }