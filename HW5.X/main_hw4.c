#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>
#include<math.h>
#include"spi.h"
#include"setup.h"

#define PI 3.1415

unsigned short DAC_input(char a, unsigned char data);
void DAC_send(unsigned short D);

int main() {

    initSETUP();
    initSPI();
    
    unsigned char sin_val = 0;
    unsigned char tri_val = 0;
    double sin_val_d = 0;
    double tri_val_d = 0;
    double t = 0;
    unsigned short data = 0;
    
    while (1) {
        _CP0_SET_COUNT(0);
        if (t >= 0.50){
            t = 0;
        }
        
        sin_val_d = (1 + sin(4*PI*(t-0.125)))/2;//0.5 second period sin wave
        tri_val_d = (t <= 0.25) ? (4*t) : (4*(0.5-t));//0.5 second period triangle wave
        sin_val = (unsigned char) (sin_val_d*256); 
        tri_val = (unsigned char) (tri_val_d*256); 
        data = DAC_input(1,sin_val);
        DAC_send(data);
        data = DAC_input(0,tri_val);
        DAC_send(data);
        
        t = t+0.01;
        while(_CP0_GET_COUNT() < 480000/2){}
    }
}

unsigned short DAC_input(char a, unsigned char data){
    unsigned short D = 0;//It took me 4 hours to de bug this line, I forgot to set it to 0 so I always got gibberish.
    D |= a << 15;
    D |= 0b111 << 12;
    D |= data << 4;
    return D;
}

void DAC_send(unsigned short D){
    LATAbits.LATA0 = 0;
    spi_io(D >> 8);
    spi_io(D);
    LATAbits.LATA0 = 1;
}



