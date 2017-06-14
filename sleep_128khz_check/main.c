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
 *  avrdude -B 250 -c usbtiny -P usb -p m328p -U lfuse:w:0xD3:m -U hfuse:w:0xDF:m -U efuse:w:0x07:m -U flash:w:/Users/gabik/workspace/sleep_128khz_check/Debug/sleep_128khz_check.hex:a
 *  http://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega328p&LOW=D3&HIGH=DF&EXTENDED=07&LOCKBIT=FF
 *
 * Connections:
 * - LED to PD6 [OC0A]
 * - POT to PC5 (MUX=ADC5=5)
 *
 *  Datasheet power consumption graph is 34-12 [P.410]
 * ########################################
 */

// #define F_CPU 128000 - defined by eclipse

// LED Definitions
#define LED_PORT PORTD
#define LED_PIN PD6
#define LED_DDR DDRD
#define LED_PWM_PCNT 50

// Watchdog definitions
/* we have 128Khz / 1024K = 0.125hz => 8 Sec
 * Lets rise every 10 Mins to check:
 * 10Min * 60Sec = 600Sec / 8Sec = 75 Cycles.
 */
// TODO: CHANGE BACK TO 75 [now it is 8 sec]
#define WD_COUNTER_MAX 1

// ADC definitions
#define ADC_READ_COUNTS 10
#define POT_MUX 5

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/wdt.h>

// Global watchdog counter (for wake-up counts)
volatile uint8_t watchdog_counter;

// Function declarations [may split to files]
void reset_pins();
void setup_pins();
void setup_adc();
void handle_water();
void sleep_me();
void setup_init();
float read_adc(int ch);
void blink_led();
void setup_pwm();
void test_blink(int pin);
void setup_watchdog(uint8_t state);

void test_blink(int pin)
{
	reset_pins();
	DDRC |= (1<<pin);
	PORTC |= (1<<pin);
	_delay_ms(300);
	reset_pins();
}

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

void setup_watchdog(uint8_t state)
{
	// Watchdog:
	// Clear WD reset flag
	MCUSR = 0;

	// We need to set 1 on WDCE in able to change
	// prescaler and enable interrupts, holds 4 cycles.
	// wrong: WDTCSR = (1<<WDCE);
	WDTCSR = (1 << WDCE) | (1 << WDE);

	// Bit 7    6    5      4    3   2 1 0
	//	   WDIF WDIE WDP[3] WDCE WDE WDP[2:0]   [P.80]
	// register WDTCR bits WDP[3:0] to 1001 => 128Khz/1024 = 1/0.125 [8 sec]
	if (state == 0) {
		WDTCSR = 0;
	} else {
		WDTCSR = (1<<WDP3) | (1<<WDP0) | (1<<WDIE);
	}
	wdt_reset();
	test_blink(2);
}

float read_adc(int ch)
{
  float average = 0.0;

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

void setup_pwm()
{
	// TC0 fast PWM - TOP = 0xFF. non-inverted
	TCCR0A = (1<<WGM00) | (1<<WGM01) | (1<<COM0A1);
	// prescaler = 256 => 500 Hz
	TCCR0B = (1<<CS02);
	OCR0A = 0xFF * LED_PWM_PCNT / 100;
}

void blink_led()
{
	// FULL cycle of 5 sec
	LED_DDR = (1<<LED_PIN);
	LED_PORT = (1<<LED_PIN);
	_delay_ms(5000);
	LED_PORT &= ~(1<<LED_PIN);

	// PWM cycle of 5 sec
	setup_pwm();
	_delay_ms(5000);
}

void sleep_me()
{
	// 14.11 on Datasheet [page 65]

	// Timer0 PWM off
	TCCR0A = 0;
	TCCR0B = 0;
	OCR0A = 0;

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

	test_blink(2);
	setup_watchdog(1);
    // Goto sleep
    sleep_cpu();
}

void setup_init()
{
	// Low-power 128Khz clock / 256 = 500hz - I am not sure it works, I think it is for the 8Mhz oscillator.
	// [P.60]
	CLKPR=(1<<CLKPCE);
	CLKPR=8;   // CLKPS[3:0] = 1000

	// ADC [comperator] disabled, not needed.
	ACSR = 0;

	watchdog_counter = 0;
	setup_watchdog(1);
}

// watchdog interrupt
ISR(WDT_vect) {
	setup_watchdog(0);
}

// ADC completed interrupt
ISR (ADC_vect){};

int main(void)
{
	cli();
	setup_init();
	test_blink(4);
	//handle_water();
    while(1)
    {
    	cli();
    	// Disable SLEEP
    	SMCR &= ~(1<<SE);

    	test_blink(3);

    	// Checking if need to handle water, if not, go to sleep.
    	if (watchdog_counter >= WD_COUNTER_MAX) handle_water();
    	else sleep_me();

    	watchdog_counter++;
    }
}
