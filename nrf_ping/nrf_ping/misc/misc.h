/*
 * misc.h
 *
 * Created: 01/08/2015 16:30:09
 *  Author: Gabi Kazav
 */ 


#ifndef MISC_H_
#define MISC_H_

#include <avr/io.h>

#define MIN_COUNT_PRESS 10

uint8_t check_button_click();
void init_button();

#endif /* MISC_H_ */