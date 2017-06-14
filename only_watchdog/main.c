#define __AVR_ATmega328P__

#define LED_PORT PORTD
#define LED_PIN PD6
#define LED_DDR DDRD

#define WD_COUNTER_MAX 1

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/sleep.h>

volatile uint8_t watchdog_counter;

void led_blink()
{
	LED_DDR = (1<<LED_PIN);
	LED_PORT = (1<<LED_PIN);
	_delay_ms(1000);
	LED_DDR = 0;
	LED_PORT = 0;
	watchdog_counter = 0;
}

void test_blink(int pin)
{
//	reset_pins();
//	DDRC |= (1<<pin);
	PORTC |= (1<<pin);
	_delay_ms(100);
	PORTC &= ~(1<<pin);

//	reset_pins();
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

void setup_watchdog()
{
	MCUSR = 0;
	wdt_reset();

	WDTCSR = (1 << WDCE) | (1 << WDE);
//	WDTCSR = 71;
	__asm__ __volatile__ ("sts %0,%1" "\n\t" : : "M" (_SFR_MEM_ADDR(WDTCSR)), "r" (71));

//	WDTCSR = 7 | (1<<WDIE);
//	wdt_enable(WDTO_2S);

//	__asm__ __volatile__ ("wdr");
	test_blink(2);
//	__asm__ __volatile__ ("wdr");
	wdt_reset();
}

void sleep_me()
{
	cli();
	setup_watchdog();
//	__asm__ __volatile__ ("sleep");
	sei();
	sleep_enable();
	sleep_cpu();
}

void setup_init()
{
//	CLKPR=(1<<CLKPCE);
//	CLKPR=8;   // CLKPS[3:0] = 1000

	ACSR = 0;
	ADCSRA = 0;

	watchdog_counter = 0;
	DDRC = 0xff;
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

// watchdog interrupt
ISR(WDT_vect) {
	wdt_disable();
	sleep_disable();
//	test_blink(3);
	PORTC ^= (1<<3);
	watchdog_counter+=1;
}

int main(void)
{
	MCUSR = 0;
	wdt_disable();

	test_blink(4);
	setup_init();
    while(1)
    {
    	if (watchdog_counter >= WD_COUNTER_MAX) led_blink();

    	sleep_me();
    }
}
