#define F_CPU 1000000L

#include <avr/io.h>
#include <util/delay.h>
#include "nrf24l01/nrf24l01.h"

//main here
int main(void) {
	uint8_t bufferout[NRF24L01_PAYLOAD];
	uint8_t c = 0;
	nrf24l01_init();

_delay_ms(3000);
	//main loop
	for(;;) {
				for(uint8_t i=0; i<sizeof(bufferout); i++) bufferout[i] = c+i;
				nrf24l01_write(bufferout);
				_delay_ms(1000);
				c++;
			}
}




