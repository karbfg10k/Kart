/* 
 * File:   i2c.h
 * Author: Admin
 *
 * Created on January 16, 2014, 5:06 PM
 */

#ifndef I2C_H
#define	I2C_H

#ifdef	__cplusplus
extern "C" {
#endif


#define  make_clk_high()  (TRISBbits.TRISB7=1)
#define  make_clk_low()   (TRISBbits.TRISB7=0)
#define  make_sda_high()  (TRISBbits.TRISB2=1)
#define  make_sda_low()   (TRISBbits.TRISB2=0)

#define  INIT_TIME 2
#define  CLOCK_TIME 3




void hf_wait(uint16_t oneustick);

void hf_sts21_init(void);

void hf_sts21_starti2c(void);

void hf_sts21_stopi2c(void);

uint8_t hf_sts21_send_i2c(uint8_t data);

uint8_t hf_sts21_get_data_i2c(uint8_t ack);

uint16_t hf_sts21_get_tempreature(void);


#ifdef	__cplusplus
}
#endif

#endif	/* I2C_H */



