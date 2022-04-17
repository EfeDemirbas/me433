#ifndef SETUP__H__
#define SETUP__H__

#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro

void initSETUP();
void ReadUART1(char * message, int maxLength);
void WriteUART1(const char * string);

#endif // SETUP__H__