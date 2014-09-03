/* 
 * File:   uart.h
 *
 * Created on January 14, 2014, 12:00 AM
 */

#ifndef UART_H
#define	UART_H

#ifdef	__cplusplus
extern "C" {
#endif

#define HF_UART1_REC_BUF_SIZE 50
#define HF_UART2_REC_BUF_SIZE 50
#define UART1 1
#define UART2 2

    typedef struct {
        uint16_t no_of_bytes;
        uint8_t overflow;
        uint8_t recieved_data_flag;
        uint16_t recieved_data_pointer;
        uint16_t sent_data_pointer;
    } UARTFlags;

    typedef struct {
        uint8_t uartno;
        uint8_t baudrate : 1;
        uint8_t parity : 2;
        uint8_t stopbits : 1;

    } UARTProperties;

    void init_and_config_uart(UARTProperties *init);

    void send_data(uint8_t uartno, uint8_t *data, uint16_t size);

    void uinit_uart(uint8_t uartno);

    uint8_t *check_overflow(uint8_t uartno);

    uint8_t *check_for_recieved_data(uint8_t uartno);

    uint16_t *check_for_no_of_data(uint8_t uartno);

    uint16_t get_data(uint8_t uartno,uint8_t* array,uint16_t length);
    
    void __attribute__((interrupt, auto_psv)) _U1RXInterrupt(void);

    
    void __attribute__((interrupt, auto_psv)) _U2RXInterrupt(void);


#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */

