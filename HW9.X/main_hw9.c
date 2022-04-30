#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro

#include <stdio.h>
#include <math.h>
#include "setup.h"
#include "ws2812b.h"


int main() {

    init_SETUP();
    ws2812b_setup();
    
    float H=0, S=1, B=0.1, H_buf;
    
    wsColor c[5];
    

    
    while(1){
        

        for(int i = 0; i<5 ; i++){
            
            H_buf = H + 72*i;
        
            while (H_buf >= 360.0){
                H_buf -= 360.0;
            }
            c[i] = HSBtoRGB(H_buf,S,B);

        }
        H = H+1;
        
        ws2812b_setColor(c,5);
        
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT() < 48000000/400){}
        
        LATAINV = 0b0010000;
        
       
        
        
    }

}

/*typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} wsColor; 

void ws2812b_setup();
void ws2812b_setColor(wsColor*,int);
wsColor HSBtoRGB(float hue, float sat, float brightness);*/