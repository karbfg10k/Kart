#include <p24Fxxxx.h>
#include <stdint.h>
#include "uart.h"




#if AF_OP_FREQUENCY == 20

#define HF_BAUD_9600 259
#define HF_BAUD_19200 129
#define HF_BAUD_115200 21

#endif

#define mGetBrg(baudrate) HF_BAUD_##baudrate


static UARTFlags u1;
static UARTFlags u2;
static uint8_t uart1_rec_buf[HF_UART1_REC_BUF_SIZE];
static uint8_t uart2_rec_buf[HF_UART2_REC_BUF_SIZE];

uint8_t dummy;

void __attribute__((interrupt, auto_psv)) _U1RXInterrupt(void) {

    if (IFS0bits.U1RXIF == 1) {
        IFS0bits.U1RXIF = 0;
        if (u1.no_of_bytes < HF_UART1_REC_BUF_SIZE) {
            uart1_rec_buf[u1.sent_data_pointer++] = U1RXREG;
            u1.no_of_bytes++;
            if (u1.sent_data_pointer >= HF_UART1_REC_BUF_SIZE)
                u1.sent_data_pointer = 0;

        } else {
            dummy = U1RXREG; //Discard the data.
            u1.overflow = 1;
        }

        u1.recieved_data_flag = 1;
    }
}

void __attribute__((interrupt, auto_psv)) _U2RXInterrupt(void) {

    if (IFS1bits.U2RXIF == 1) {
        IFS1bits.U2RXIF = 0;
        if (u2.no_of_bytes < HF_UART2_REC_BUF_SIZE) {
            uart2_rec_buf[u2.sent_data_pointer++] = U2RXREG;
            u2.no_of_bytes++;
            if (u2.sent_data_pointer >= HF_UART2_REC_BUF_SIZE)
                u2.sent_data_pointer = 0;

        } else {
            dummy = U2RXREG; //Discard the data.
            u2.overflow = 1;
        }
        u2.recieved_data_flag = 1;
    }
}

void init_and_config_uart(UARTProperties *init) {
    if (init->uartno == UART1) {

        u1.no_of_bytes = 0;
        u1.overflow = 0;
        u1.recieved_data_flag = 0;

        TRISBbits.TRISB7 = 0;
        TRISBbits.TRISB2 = 1;
        AD1PCFGbits.PCFG4 = 1;
        PMD1bits.U1MD = 0;
        U1MODEbits.UEN = 0;
        U1MODEbits.WAKE = 0; //Wake from sleep if a falling edge is detected in UART1.
        U1MODEbits.BRGH = 1; //Baud rate clock is in high speed mode.
        U1STAbits.UTXEN = 1;

        U1MODEbits.PDSEL = init->parity;
        U1MODEbits.STSEL = init->stopbits;
        if (init->baudrate == 0)
            U1BRG = mGetBrg(9600);
        if (init->baudrate == 1)
            U1BRG = mGetBrg(19200);
        if (init->baudrate == 2)
            U1BRG = mGetBrg(115200);

        U1MODEbits.UARTEN = 1;
        U1STAbits.UTXEN = 1;
        IEC0bits.U1RXIE = 1;
        IFS0bits.U1RXIF = 0;

    } else if (init->uartno == UART2) {
        u2.no_of_bytes = 0;
        u2.overflow = 0;
        u2.recieved_data_flag = 0;
        TRISBbits.TRISB0 = 0;
        TRISBbits.TRISB1 = 1;
        AD1PCFGbits.PCFG2 = 1;
        AD1PCFGbits.PCFG3 = 1;
        PMD1bits.U2MD = 0;
        U2MODEbits.UEN = 0;
        U2MODEbits.WAKE = 0; //Wake from sleep if a falling edge is detected in UART1.
        U2MODEbits.BRGH = 1; //Baud rate clock is in high speed mode.
        U2STAbits.UTXEN = 1;

        U2MODEbits.PDSEL = init->parity;
        U2MODEbits.STSEL = init->stopbits;
        if (init->baudrate == 0)
            U2BRG = mGetBrg(9600);
        if (init->baudrate == 1)
            U2BRG = mGetBrg(19200);
        if (init->baudrate == 2)
            U2BRG = mGetBrg(115200);

        U2MODEbits.UARTEN = 1;
        U2STAbits.UTXEN = 1;
        IEC1bits.U2RXIE = 1;
        IFS1bits.U2RXIF = 0;
    }
}

void send_data(uint8_t uartno, uint8_t *data, uint16_t size) {
    uint8_t ctr = 0;
    if (uartno == UART1) {
        while (ctr < size) {
            while (U1STAbits.UTXBF);
            U1TXREG = data[ctr];
        }
    } else if (uartno == UART2) {
        while (ctr < size) {
            while (U2STAbits.UTXBF);
            U2TXREG = data[ctr];
        }
    }
    return;
}

void uinit_uart(uint8_t uartno) {
    if (uartno == UART1) {
        U1MODEbits.UARTEN = 0;
        U1STAbits.UTXEN = 0;
        IEC0bits.U1RXIE = 0;
        PMD1bits.U1MD = 1;

    } else if (uartno == UART2) {
        U2MODEbits.UARTEN = 0;
        U2STAbits.UTXEN = 0;
        IEC1bits.U2RXIE = 0;
        PMD1bits.U2MD = 1;
    }

}

uint8_t *check_overflow(uint8_t uartno) {
    if (uartno == UART1)
        return &(u1.overflow);
    else if (uartno == UART2)
        return &(u2.overflow);
}

uint8_t *check_for_recieved_data(uint8_t uartno) {
    if (uartno == UART1) {
        U1STAbits.OERR = 0;
        return &(u1.recieved_data_flag);
    } else if (uartno == UART2) {
        U2STAbits.OERR = 0;
        return &(u2.recieved_data_flag);
    }
}

uint16_t *check_for_no_of_data(uint8_t uartno) {

    if (uartno == UART1) {
        U1STAbits.OERR = 0;
        return &(u1.no_of_bytes);
    } else if (uartno == UART2) {
        U2STAbits.OERR = 0;
        return &(u2.no_of_bytes);
    }

}

uint16_t get_data(uint8_t uartno, uint8_t* array, uint16_t length) {

    uint16_t size = 0;
    uint16_t ctr = 0;
    if (uartno == UART1) {
        U1STAbits.OERR = 0;
        for (ctr = 0; ctr < length; ctr++) {
            if (u1.no_of_bytes > 0) {

                array[ctr] = uart1_rec_buf[u1.recieved_data_pointer];
                u1.recieved_data_pointer++;
                u1.no_of_bytes--;
                size++;
                if (u1.recieved_data_pointer >= HF_UART1_REC_BUF_SIZE)
                    u1.recieved_data_pointer = 0;
            }
            return size;
        }
    } else if (uartno == UART2) {
        U2STAbits.OERR = 0;
        for (ctr = 0; ctr < length; ctr++) {
            if (u2.no_of_bytes != 0) {
                array[ctr] = uart2_rec_buf[u2.recieved_data_pointer];
                u2.recieved_data_pointer++;
                u2.no_of_bytes--;
                size++;
                if (u2.recieved_data_pointer >= HF_UART2_REC_BUF_SIZE)
                    u2.recieved_data_pointer = 0;
            }
        }
        return size;
    }
}




