#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "setup.h"
#include "i2c_master_noint.h"
#include "ssd1306.h"
#include "font.h"

#define Line_r2_Message "|           /    /       | "
#define Line_r3_Message "|          /    /        | "
#define Line_r4_Message "|         /    /         | "
#define Line_l2_Message "|       \\    \\          |  "
#define Line_l3_Message "|        \\    \\         |  "
#define Line_l4_Message "|         \\    \\        |  "
#define Line_s2_Message "|         |     |        |  "
#define Line_s3_Message "|         |     |        |  "
#define Line_s4_Message "|         |     |        |  "


void make_char(unsigned char x, unsigned char y, signed char ASCII_num);
void OLED_write(char * message, char line);
void PWM_Set(int pwm_left,int pwm_right);

int main() {

    init_SETUP();
    i2c_master_setup();
    ssd1306_setup();
    
    char buff[32];
    float count = 0;
    unsigned int heartbeat = 0;
    char pico_buff[200];
    int movement = 0;
    float delta_row = 0;
    float row_center = 0;
    int pwm_base = 0;
    int pwm_left = 0;
    int pwm_right = 0;
    
    int turn_coefficient = 30 ; //For a wheel max pwm  = turn_coefficient+base_coefficient+base_pwm
    int base_coefficient = 0;
    int pwm_min = 40;
    
    while(1){
 
        //_CP0_SET_COUNT(0);

        ReadUART2(pico_buff,100);
        sscanf(pico_buff,"%d", &movement);
        ReadUART2(pico_buff,10);
        sscanf(pico_buff,"%f", &delta_row);
        ReadUART2(pico_buff,10);
        sscanf(pico_buff,"%f", &row_center);
        
        
        LATBbits.LATB12 = 0;//Phases must be opposite when driving forward 
        LATBbits.LATB14 = 1;
        
        //delta_row ranges between -30 to 30 which is nearly proportional to the
        //relative pwm between wheels the amount of difference between the wheel speeds 
        //will be controlled with a P-type controller
        //The floor of the difference will be adjusted with another coefficient related to
        //row_center of the farthest row which can give us an idea of how curved the
        //upcoming section is and gain us time by slowing everything down
        
        row_center = 15 - row_center;
        if (row_center < 0){ 
            row_center = -row_center/15; //Now row_center is normalized around 0
        }
        
        delta_row = delta_row/30; //Now normalized around 0
        
        pwm_base = (int)(base_coefficient*(1-row_center)+pwm_min); //Scale the pwm to 90 to leave room for maneuvering
                                         //and return to integer for speed
        
   
        if(-0.7<delta_row <0){
            pwm_left = pwm_base - (int)(turn_coefficient*delta_row);
            pwm_right = pwm_base + (int)(turn_coefficient*delta_row/2); //
        }
        else if(0.7>delta_row > 0){
            pwm_left = pwm_base - (int)(turn_coefficient*delta_row/2); //
            pwm_right = pwm_base + (int)(turn_coefficient*delta_row);
        }
        else{
            pwm_left = pwm_base ;
            pwm_right = pwm_base ;
        }

        if(movement == !0){
            PWM_Set(pwm_left,pwm_right);
        }
        else{
            OC1RS = 0;
            OC4RS = 0;
        }
        
        
        
        
        //OLED printing
        
        sprintf(buff,"||    %d    %4.2f    %4.2f ||", movement, delta_row, row_center );
        OLED_write(buff,1);
        
        if (0.7 > delta_row >= 0.15){
            OLED_write(Line_r2_Message,2);
            OLED_write(Line_r3_Message,3);
            OLED_write(Line_r4_Message,4);
        }
        else if(-0.7 < delta_row < -0.15){
            OLED_write(Line_l2_Message,2);
            OLED_write(Line_l3_Message,3);
            OLED_write(Line_l4_Message,4);
        }
        else{
            OLED_write(Line_s2_Message,2);
            OLED_write(Line_s3_Message,3);
            OLED_write(Line_s4_Message,4);
        }
        ssd1306_update(); 
        

        
 
    }

}



void PWM_Set(int pwm_left,int pwm_right){

    if (pwm_left > 100){pwm_left = 100;}
    if (pwm_right > 100){pwm_right = 100;}
    
    
    OC1RS = (pwm_left*(PR2+1))/100-1;
    
    OC4RS = (pwm_right*(PR2+1))/100-1;

    
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
