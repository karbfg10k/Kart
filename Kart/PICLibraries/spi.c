#include<p24Fxxxx.h>
#include"stdint.h"
#include"spi.h"



#if AF_OP_FREQUENCY==20

#define mGetSPI1PriScaler() 1
#define mGetSPI1SecScaler() 2

#endif


void init_and_config(SPI_Setting* setting){
    PMD1bits.SPI1MD=0;   // Power the SPI module.
    TRISBbits.TRISB10=1; // Configure SDI as Digital Input.
    TRISBbits.TRISB11=0; // Configure SCK as Digital Output(Master Mode operation only).
    TRISBbits.TRISB13=0; // Configure SDO as Digital Output
    TRISBbits.TRISB15=0; // Configure SS as Digital Output.

    SPI1STATbits.SPIEN = 0;
    SPI1CON1bits.CKE = setting->clockedge;
    SPI1CON1bits.CKP = setting->clockpolarity;
    SPI1CON1bits.SMP = setting->inputsample;

    SPI1CON1bits.PPRE = mGetSPI1PriScaler();  //Load the Primary Prescaler.
    SPI1CON1bits.SPRE = mGetSPI1SecScaler();  //Load the Secondary prescaler.

    SPI1CON1bits.SSEN = 0;  // Configure the SPI to not use SS1 Pin.
    SPI1STATbits.SPIROV=0;
    SPI1CON1bits.MSTEN = 1; // Configure the SPI to work in Master Mode.
    SPI1STATbits.SPIEN = 1;
}



void write_spi(uint8_t data){
    uint8_t dummy;
    dummy = SPI1BUF;  //Discard the last unread data.
    SPI1BUF=data;
    while(SPI1STATbits.SPIRBF == 0); //SPI operation wait time. CodeLock Point.
}

uint8_t read_spi(void){
    uint8_t dummy;
    dummy=SPI1BUF;
    SPI1BUF=0xFF;
    while(SPI1STATbits.SPIRBF == 0);//SPI operation of wait time.CodeLock Point.
    dummy = SPI1BUF;
    return dummy;
}

uint8_t rwdata_spi(uint8_t data){
    uint8_t dummy;
    dummy=SPI1BUF; //Safe read to ignore the last data.
    SPI1BUF=data;
    while(SPI1STATbits.SPIRBF==0);//SPI operation wait time. CodeLock Point.
    dummy=SPI1BUF;
    return dummy;
}

void uninit_spi(void){
    SPI1CON2bits.SPIBEN = 0; //Disable SPI Module Operation.
    PMD1bits.SPI1MD=1; // Disable Power to SPI.
}
