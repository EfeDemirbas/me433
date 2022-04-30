#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro

#include <stdio.h>
#include <math.h>
#include "setup.h"
#include "i2c_master_noint.h"
#include "ssd1306.h"
#include "font.h"

#define Line_1_Message "Hi Nick, just finished"
#define Line_2_Message "with this homework!! <3"

void make_char(unsigned char x, unsigned char y, signed char ASCII_num);
void OLED_write(char * message, char line);

int main() {

    init_SETUP();
    i2c_master_setup();
    ssd1306_setup();
    
    char buff[32];
    float count = 0;
    unsigned int heartbeat = 0;
    
    while(1){
        
        _CP0_SET_COUNT(0);
        
        
        if ((heartbeat % 25) < 12){
            OLED_write(Line_1_Message,1);
            OLED_write(Line_2_Message,2);
            sprintf(buff," /     /     fps = %4.2f",count);
            OLED_write(buff,3);
            sprintf(buff," (O _ 0)      i = %d",heartbeat);
            OLED_write(buff,4);
        }
        if ((heartbeat % 25) > 12){
            OLED_write(Line_1_Message,1);
            OLED_write(Line_2_Message,2);
            sprintf(buff," \\     \\     fps = %4.2f",count);
            OLED_write(buff,3);
            sprintf(buff,"  (0 . O)     i = %d",heartbeat);
            OLED_write(buff,4);
        }
        ssd1306_update();   
        
        count = _CP0_GET_COUNT();
        count = 24000000/count;
        heartbeat++;
    }

}

void make_char(unsigned char x, unsigned char y, signed char ASCII_num){
    
    ASCII_num = ASCII_num - 32;//Adjusting for the font library to printable characters
    unsigned char color = 0;
    
    for(unsigned char i = 0 ; i <= 4 ; i++){
        for(unsigned char j = 0 ; j <= 7 ; j++){
            color = (ASCII[ASCII_num][i] >> j) & 0b00000001;
            ssd1306_drawPixel(x, y, color);
            y = y+1;
        }
        y = y - 8;
        x = x+1;
    }
    
    
}

void OLED_write(char * message, char line){
    if (line > 4){return;}
    unsigned char x = 0, y = 0;
    y = y + (line-1)*8;
    
    unsigned char i = 0;
    while(1){
        if(message[i] == 0x00){break;}
        make_char(x+5*i,y,message[i]);
        i = i+1;
    }
}
