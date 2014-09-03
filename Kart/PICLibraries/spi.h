/* 
 * File:   spi.h
 * Author: Kart
 *
 * Created on January 11, 2014, 11:53 PM
 */

#ifndef SPI_H
#define	SPI_H

#ifdef	__cplusplus
extern "C" {
#endif

    typedef struct {
        uint8_t clockedge : 1;
        uint8_t inputsample : 1;
        uint8_t clockpolarity : 1;
    } SPI_Setting;

#define mSPISelect(x) (LATBbits.LATB15 = x)
#define mSPIAssert()  (LATBbits.LATB15=1)
#define mSPIDeassert() (LATBbits.LATB15 = 0)


    void init_and_config(SPI_Setting *setting);

    void write_spi(uint8_t data);

    uint8_t read_spi(void);

    uint8_t rwdata_spi(uint8_t data);

    void uninit_spi(void);



#ifdef	__cplusplus
}
#endif

#endif	/* SPI_H */

