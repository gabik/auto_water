This project contain Gabi Kazav projects:
1) Auto water - small standalone watering system
2) NRF ping (rx/tx_sleep_test) - usng power down sleep
3) Gabi's working NRF24l01 lib

Some useful commands:
AVRDude:
Download: http://download.savannah.gnu.org/releases/avrdude/ [source]
	  https://sourceforge.net/projects/winavr/ [binary - windows]
The release repo has PDF files as well
-c usbtiny
-p atmega328p [m32] / attiny84 [t84/t85]
Checking connection and chip: avrdude -c usbtiny -p t84
-U <memtype>:r|w|v:<filename>[:format]:
   memtype: flash, eeprom, hfuse (high fuse), lfuse (low fuse), or efuse (extended fuse)
             read write verify
                   hex file
                             ignore
-U flash:w:firmware.hex

FUSES:
use fuse calc: http://www.engbedded.com/fusecalc/
for example, for auto water project I need 128kHz clock, no BOD, fast rising boot time [+4]
-U lfuse:w:0x54:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m

Tutorial: http://www.ladyada.net/learn/avr/avrdude.html
AVRDude into Atmel Studio: http://www.crash-bang.com/using-usbtiny-with-atmelstudio/
