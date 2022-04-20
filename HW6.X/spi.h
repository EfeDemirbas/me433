#ifndef SPI__H__
#define SPI__H__

#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro

void initSPI();
unsigned char spi_io(unsigned char o);
unsigned short DAC_input(char a, unsigned char data);
void DAC_send(unsigned short D);

#endif // SPI__H__