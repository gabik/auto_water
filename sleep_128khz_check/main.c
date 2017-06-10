/*
 * main.c
 *
 *  Created on: Jun 8, 2017
 *      Author: gabik
 */

/*
 * ########################################
 * TODO: FUSES:
 * - 128Khz clock
 * - BOD disabled
 * - On-chip Debug System disabled
 *  avrdude -B 250 -c usbtiny -P usb -p m328p -U lfuse:w:0xD3:m -U hfuse:w:0xDF:m -U efuse:w:0xFF:m
 *  http://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega328p&LOW=D3&HIGH=DF&EXTENDED=FF&LOCKBIT=FF
 *
 * Connections:
 * - LED to PD0
 * - POT to PC5 (MUX=ADC5=5)
 *
 *  Datasheet power consumption graph is 34-12 [P.410]
 * ########################################
 */

// #define F_CPU 128000 - defined by eclipse

// LED Definitions
#define LED_PORT PORTD
#define LED_PIN PD0
#define LED_DDR DDRD

// Watchdog definitions
/* we have 128Khz / 1024K = 0.125hz => 8 Sec
 * Lets rise every 10 Mins to check:
 * 10Min * 60Sec = 600Sec / 8Sec = 75 Cycles.
 */
#define WD_COUNTER_MAX 75

// ADC definitions
#define ADC_READ_COUNTS 10
#define POT_MUX 5

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdint.h>
#include <util/delay.h>

// Function declarations [may split to files]
void reset_pins();
void setup_pins();
void setup_adc();
void handle_water();
void sleep_me();
void setup_init();
float read_adc(int ch);
void blink_led();

// Global watchdog counter (for wake-up counts)
uint8_t watchdog_counter;

void reset_pins()
{
	DDRB = 0;
	DDRC = 0;
	DDRD = 0;
	PORTB = 0;
	PORTC = 0;
	PORTD = 0;
}

void setup_pins()
{
	// Enable pull-up resistors
	SMCR &= ~(1<<PUD);
	reset_pins();
	LED_DDR |= (1<<LED_PIN);
}

void setup_adc()
{
	// Enable ADC in PRR
	PRR = 0xfe;

	// Set ADC with AVcc ref, left adjust
	ADMUX = (1<<REFS0) | (1<<ADLAR);
	// Enable, Clear flag, Set interrupt mode, prescaler = 2
	ADCSRA = (1<<ADEN) | (1<<ADIF) | (1<<ADIE);
}

float read_adc(int ch)
{
  float average = 0.0;
  ch &= 0b00000011;

  // take N samples in a rowy
  for (int8_t i = 0; i < ADC_READ_COUNTS; i++)
  {
	  // clears the bottom 2 bits and set the channel got as arg
	  ADMUX = (ADMUX & 0xFC)|ch;
	  // start the conversion with interrupt
	  ADCSRA |= (1<<ADSC) | (1<<ADIE);
	  set_sleep_mode (SLEEP_MODE_ADC);    // sleep during sample
	  sei();
	  sleep_mode ();

	  average += ADCH;
  }

  average /= ADC_READ_COUNTS;
  return average;
}

void handle_water()
{
	// If counter match, handle the water part.
	// But first we need to re-setup ADC + PINS
	setup_pins();
	setup_adc();

	// TODO: WORK
	uint8_t pot = read_adc(POT_MUX);
	if (pot >= 100) blink_led();

	// After the work, we need to reset watchdog counter and go to sleep.
	watchdog_counter = 0;
	sleep_me();
}

void blink_led()
{
	for (int8_t  i=0 ; i<10 ; i++)
	{
		LED_PORT ^= (1<<LED_PIN);
		_delay_ms(1000);
	}
}

void sleep_me()
{
	// 14.11 on Datasheet [page 65]

	// ADC and AC
	// ADCSRA &= ~(1<<ADEN); //Disable ADC, saves ~230uA - replaced with ADCSRA = 0
	// ACSR &= ~(1<<ACD); // Disable AD comparator - moved to setup_init.
	ADCSRA = 0;

	/* The Internal Voltage Reference will be enabled when needed by the Brown-Out Detection, the Analog
	Comparator or the Analog-to-Digital Converter. If these modules are disabled as described in the sections
	above, the internal voltage reference will be disabled and it will not be consuming power
	[System Control and Reset. P.72]
	*/

	// Input pins:
	// Refer to the section Digital Input Enable and Sleep Modes for details on which pins are enabled
	// Digital input buffers can be disabled by writing to the Digital Input Disable Registers (DIDR0 for ADC, DIDR1 for AC)
	reset_pins();

	// Ensure the On-chip Debug System Fuses are off.

	// PRR register handles: [P.71]
	// TWI, Timers[0,1,2], SPI, USART, ADC (Should disable ADC first)
	PRR = 0xff;

	// Set sleep mode to POWER DOWN
	SMCR |= (1 << SM1);
	// Enable SLEEP
	SMCR |= (1<<SE);

	// Enable global interrupts.
	sei();

	// Disable pull-up resistors
	SMCR |= (1<<PUD);
	// BOD disable (I should disable it from FUSE) [System Control and Reset. P.72]
	// Should be done right before sleep [P.85]
	// MCUCR |= (1<<BODS) | (1<<BODSE);
    // MCUCR &= ~(1<<BODSE);  // must be done right before sleep

    // Goto sleep
    sleep_cpu();

}

void setup_init()
{
	// Low-power 128Khz clock / 256 = 500hz - I am not sure it works, I think it is for the 8Mhz oscillator.
	// [P.60]
	CLKPR=(1<<CLKPCE);
	CLKPR=8;   // CLKPS[3:0] = 1000

	// ADC disabled, not needed.
	ACSR = 0;

	// Watchdog:
	// We need to set 1 on WDCE in able to change
	// prescaler and enable interrupts, holds 4 cycles.
	WDTCSR = (1<<WDCE);
	// Bit 7    6    5      4    3   2 1 0
	//	   WDIF WDIE WDP[3] WDCE WDE WDP[2:0]   [P.80]
	// register WDTCR bits WDP[3:0] to 1001 => 128Khz/1024 = 1/0.125 [8 sec]
	WDTCSR = 0b01100001;
}

// watchdog interrupt
ISR(WDT_vect) {
	cli();
	watchdog_counter++;

	// Disable SLEEP
	SMCR &= ~(1<<SE);

	/* ReEnable the watchdog interrupt,
	* as this gets reset when entering this ISR
	* and automatically enables the WDE signal
	* that resets the MCU the next time the  timer overflows */
	WDTCSR |= (1<<WDIE);

	// Checking if need to handle water, if not, go to sleep.
	if (watchdog_counter >= WD_COUNTER_MAX) handle_water();
	else sleep_me();
}

// ADC completed interrupt
EMPTY_INTERRUPT (ADC_vect);

int main(void)
{
	cli();
	setup_init();
	handle_water();
    while(1);
}
