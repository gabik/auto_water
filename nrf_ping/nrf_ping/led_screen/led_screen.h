/*
 * led_screen.h
 *
 * Created: 01/08/2015 02:44:51
 *  Author: Gabi Kazav
 
 
 --A--
 |   |
 B   D
 |   |
 --C--
 |   |
 E   G
 |   |
 --F--  H
 */ 


#ifndef LED_SCREEN_H_
#define LED_SCREEN_H_

#include <avr/io.h>

#define SEGMENTS_PORT PORTD
#define SEGMENTS_DDR  DDRD
#define ASEG PD0
#define BSEG PD1
#define CSEG PD2
#define DSEG PD3
#define ESEG PD4
#define FSEG PD5
#define GSEG PD6
#define HSEG PD7

#define LED_SELECT_PORT PORTC
#define LED_SELECT_DDR  DDRC
#define LED0 PC5
#define LED1 PC4
#define LED2 PC3
#define LED3 PC2

#define	DG0 ~((1<<ASEG)|(1<<BSEG)|(0<<CSEG)|(1<<DSEG)|(1<<ESEG)|(1<<FSEG)|(1<<GSEG)|(0<<HSEG))
#define DG1 ~((0<<ASEG)|(0<<BSEG)|(0<<CSEG)|(1<<DSEG)|(0<<ESEG)|(0<<FSEG)|(1<<GSEG)|(0<<HSEG))
#define DG2 ~((1<<ASEG)|(0<<BSEG)|(1<<CSEG)|(1<<DSEG)|(1<<ESEG)|(1<<FSEG)|(0<<GSEG)|(0<<HSEG))
#define DG3 ~((1<<ASEG)|(0<<BSEG)|(1<<CSEG)|(1<<DSEG)|(0<<ESEG)|(1<<FSEG)|(1<<GSEG)|(0<<HSEG))
#define DG4 ~((0<<ASEG)|(1<<BSEG)|(1<<CSEG)|(1<<DSEG)|(0<<ESEG)|(0<<FSEG)|(1<<GSEG)|(0<<HSEG))
#define DG5 ~((1<<ASEG)|(1<<BSEG)|(1<<CSEG)|(0<<DSEG)|(0<<ESEG)|(1<<FSEG)|(1<<GSEG)|(0<<HSEG))
#define DG6 ~((1<<ASEG)|(1<<BSEG)|(1<<CSEG)|(0<<DSEG)|(1<<ESEG)|(1<<FSEG)|(1<<GSEG)|(0<<HSEG))
#define DG7 ~((1<<ASEG)|(0<<BSEG)|(0<<CSEG)|(1<<DSEG)|(0<<ESEG)|(0<<FSEG)|(1<<GSEG)|(0<<HSEG))
#define DG8 ~((1<<ASEG)|(1<<BSEG)|(1<<CSEG)|(1<<DSEG)|(1<<ESEG)|(1<<FSEG)|(1<<GSEG)|(0<<HSEG))
#define DG9 ~((1<<ASEG)|(1<<BSEG)|(1<<CSEG)|(1<<DSEG)|(0<<ESEG)|(1<<FSEG)|(1<<GSEG)|(0<<HSEG))

void led_screen_init();
void write_to_led(uint16_t num, uint8_t digit);
void devide_4_digits(uint16_t num, uint8_t * d);
void poweroff_led();

#endif /* LED_SCREEN_H_ */