#include "mbed.h"

Serial pc(P0_4,P0_0);
SPI spi(P0_8, P0_9, P0_16);    //mosi, miso, sclk
DigitalOut cs1(P0_15);
DigitalIn drdy1(P0_1);
DigitalOut sw1(P0_14);
DigitalOut sw2(P0_6);

//ADC
const uint8_t rst=0b0110;
const uint8_t wreg=0b0100;
const uint8_t start=0b1000;
uint16_t adc1_read(uint8_t ch);

//vals
uint16_t val[2];
uint8_t sel;

int main(){
    //init
    pc.baud(115200);
    cs1=1;
    spi.format(8,1);
    sw1=0;
    sw2=0;

    //adc init
    cs1=0;
    spi.write(rst);
    cs1=1;
    wait(0.01);

    cs1=0;
    spi.write((wreg<<4)+(0x01<<2)+0);
    spi.write((0b110<<5)+(0b10<<3)+(0b1<<2));       //2ksps, turbo mode, cc mode
    cs1=1;
    wait(0.01);
    
    while (true){
        sel=pc.getc();

        switch(sel){
            case 0x30:  //0=ADC
                val[0]=adc1_read(3);
                val[1]=adc1_read(4);
                pc.printf("%05d%05d",val[0],val[1]);
                break;
            case 0x31:  //rbw=1 wide band
                sw1=0;
                sw2=0;
                break;
            case 0x32:  //rbw=2 narrow band
                sw1=1;
                sw2=1;
                break;
        }
    }
}

//adc1 read func.
uint16_t adc1_read(uint8_t ch){
    uint8_t mux;
    uint8_t buf[2];
    int16_t val;
    if(ch==1){
        mux=0b1000;
    }else if(ch==2){
        mux=0b1001;
    }else if(ch==3){
        mux=0b1010;
    }else if(ch==4){
        mux=0b1011;
    }
    cs1=0;
    spi.write((wreg<<4)+0b0000);
    spi.write((mux<<4)+1);
    cs1=1;
    cs1=0;
    spi.write(start);
    cs1=1;
    while(true){
        if(drdy1==0) break;
    }
    cs1=0;
    buf[1]=spi.write(0x00);
    buf[0]=spi.write(0x00);
    cs1=1;
    val=(buf[1]<<8)+buf[0];
    if(val<0){
        val=0;
    }
    return (uint16_t)val;
}