#include <p24Fxxxx.h>
#include <stdint.h>
#include "i2c.h"

#if AF_OP_FREQUENCY==20
#define mGetTimer3Period() 10
#define mGetTimer3Prescaler() 0

#endif

void hf_wait(uint16_t no_of_micro_seconds){

    uint16_t cntr;
    PMD1bits.T3MD = 0; //Power up the Timer 3 moudle.
    PR3 = mGetTimer3Period();
    T3CONbits.TCKPS = mGetTimer3Prescaler();
    IEC0bits.T3IE = 0;
    IFS0bits.T3IF = 0;
    T3CONbits.TON = 1;

    for(cntr=0;cntr<no_of_micro_seconds;cntr++){
        while(IFS0bits.T3IF == 0);
        IFS0bits.T3IF=0;
    }

    T3CONbits.TON = 0;
    PMD1bits.T3MD = 1;

    return;
}

void hf_sts21_init(void) {
    make_sda_high();
    make_clk_high();
    LATBbits.LATB2 = 0;
    LATBbits.LATB7 = 0;
    hf_wait(15000);
}

void hf_sts21_starti2c(void) {

    make_clk_high();
    hf_wait(INIT_TIME);
    make_sda_low();
    hf_wait(INIT_TIME);
}

void hf_sts21_stopi2c(void) {

    make_sda_low();
    make_clk_high();
    hf_wait(INIT_TIME);
    make_sda_high();
}

uint8_t hf_sts21_send_i2c(uint8_t dat) {

    uint8_t ctr = 0, ack;

    for (ctr = 0; ctr < 8; ctr++) {

        make_clk_low();
        if (dat & 0x80)
            make_sda_high();
        else {
            make_sda_low();
        }
        dat = dat * 2;
        hf_wait(CLOCK_TIME);
        make_clk_high();
        hf_wait(CLOCK_TIME);
    }
    make_sda_high();
    make_clk_low();
    hf_wait(CLOCK_TIME);
    ack = PORTBbits.RB2;
    make_clk_high();
    hf_wait(CLOCK_TIME);
    make_clk_low();
    return ack;
}

uint8_t hf_sts21_get_data_i2c(uint8_t ack) {

    uint8_t cntr;
    uint8_t dat = 0;

    make_sda_high();
    for (cntr = 0; cntr < 8; cntr++) {

        make_clk_low();
        hf_wait(CLOCK_TIME);
        dat = (dat * 2)+(PORTBbits.RB2);
        make_clk_high();
        hf_wait(CLOCK_TIME);
        make_clk_low();
    }

    make_clk_low();
    hf_wait(CLOCK_TIME);

    if (ack == 0)
        make_sda_low();
    else
        make_sda_high();

    make_clk_high();
    hf_wait(CLOCK_TIME);
    make_clk_low();

    return dat;

}

uint8_t get_checksum(uint8_t *data,uint8_t size)
{
    uint8_t crc=0,i,j;
    
    uint16_t poly=0x131;
    for(i=0;i<size;i++)
    {
        crc ^=data[i];
        for(j=8;j>0;j--)
        {
            if(crc&0x80){
                crc=(crc<<1);
                crc^=poly;
            }
            else
                crc=(crc<<1);
        }
    }
    return crc;
}


uint16_t hf_sts21_get_tempreature(void) {

    uint8_t t_ack=0xFE,at=0;
    uint16_t i, tmp;
    uint8_t data_t[3];
    uint8_t crc;
    
    hf_sts21_starti2c();
    hf_sts21_send_i2c(0x94);
    hf_sts21_send_i2c(0xE3);

    for (i = 0; i < 8500; i++)
        hf_wait(10);

    while (t_ack != 0 ) {

        hf_sts21_starti2c();
        t_ack=hf_sts21_send_i2c(0x95);
        for (i = 0; i < 8500; i++)
            hf_wait(10);
        at++;

        if(at==3){

            at=0;
            break;
        }
    }

    data_t[0] = hf_sts21_get_data_i2c(0);
    data_t[1] = hf_sts21_get_data_i2c(0);
    data_t[2] = hf_sts21_get_data_i2c(1);
    crc=get_checksum(data_t,2);

    if(crc!=data_t[2])
    {
        return 0;
    }

    for (i = 0; i < 1000; i++)
        hf_wait(1000);

    hf_sts21_stopi2c();
    tmp = (data_t[0]*256) | data_t[1];

    return tmp;
}




