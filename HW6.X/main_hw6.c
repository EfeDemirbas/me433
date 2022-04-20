#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>
#include<math.h>
#include"spi.h"
#include"setup.h"
#include"i2c_master_noint.h"


void i2c_MCP23008_write(unsigned char write_add, unsigned char reg, unsigned char val);
unsigned char i2c_MCP23008_read(unsigned char write_add, unsigned char reg);


int main() {
  
    initSETUP();
    i2c_master_setup();
    
    _CP0_SET_COUNT(0);
    
    unsigned char button = 0;
    
    i2c_MCP23008_write(0b01000000 , 0x00 , 0b00000001); //Initial setup of IODIR
    
    while (1) {
        if(_CP0_GET_COUNT() > 4800000){
            LATAbits.LATA4 = !(LATAbits.LATA4);
            _CP0_SET_COUNT(0);
        }
        
        button = i2c_MCP23008_read(0b01000000 , 0x09);
        
        button = (button & 0b00000001);
        
        
        if(button == 0){
            
            i2c_MCP23008_write(0b01000000 , 0x0A , 0b10000000);//Turn on the LED
        
        }
        else {
            i2c_MCP23008_write(0b01000000 , 0x0A , 0b00000000);//Turn on the LED
        } 
        

        
    }
}

void i2c_MCP23008_write(unsigned char write_add, unsigned char reg, unsigned char val){
    i2c_master_start();
    i2c_master_send(write_add);
    i2c_master_send(reg);
    i2c_master_send(val);
    i2c_master_stop();
}

unsigned char i2c_MCP23008_read(unsigned char write_add, unsigned char reg){
    unsigned char read = 0;
    i2c_master_start();
    i2c_master_send(write_add);
    i2c_master_send(reg);
    i2c_master_restart();
    i2c_master_send(write_add | 0x01);
    read = i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    return read;
}




