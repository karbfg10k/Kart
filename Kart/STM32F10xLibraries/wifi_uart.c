#include "cmsis_boot/stm32f10x.h"
#include "stm_lib/inc/stm32f10x_gpio.h"
#include "stm_lib/inc/stm32f10x_rcc.h"
#include "stm_lib/inc/stm32f10x_usart.h"
#include "stm_lib/inc/misc.h"
#include "stm_lib/inc/wifi_uart.h"

volatile uint8_t recievebuf_usart1[50];
volatile uint8_t recievebuf_usart2[50];
volatile uint8_t recievebuf_usart3[50];
static uint8_t usart1_nofbytes = 0;
static uint8_t usart1_rx_index = 0;
static uint8_t usart1_read_index = 0;
static uint8_t usart2_nofbytes = 0;
static uint8_t usart2_rx_index = 0;
static uint8_t usart2_read_index = 0;
static uint8_t usart3_nofbytes = 0;
static uint8_t usart3_rx_index = 0;
static uint8_t usart3_read_index = 0;

void USART1_Config(void) {


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,
			ENABLE);

	USART_ClockInitTypeDef USART_ClockInitStructure;
	GPIO_InitTypeDef UARTTX;
	UARTTX.GPIO_Pin = GPIO_Pin_9;
	UARTTX.GPIO_Mode = GPIO_Mode_AF_PP;
	UARTTX.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitTypeDef UARTRX;
	UARTRX.GPIO_Pin = GPIO_Pin_10;
	UARTRX.GPIO_Mode = GPIO_Mode_IPU;
	UARTRX.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &UARTTX);
	GPIO_Init(GPIOA, &UARTRX);

	USART_ClockStructInit(&USART_ClockInitStructure);
	USART_ClockInit(USART1, &USART_ClockInitStructure);

	USART_InitTypeDef UART1;
	UART1.USART_BaudRate = 9600;
	UART1.USART_WordLength = USART_WordLength_8b;
	UART1.USART_StopBits = USART_StopBits_1;
	UART1.USART_Parity = USART_Parity_No;
	UART1.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	UART1.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART1, &UART1);
	USART_Cmd(USART1, ENABLE);

	USART1_NVIC_Configuration();
}

void USART2_Config(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	USART_ClockInitTypeDef USART_ClockInitStructure;
	GPIO_InitTypeDef UARTTX;
	UARTTX.GPIO_Pin = GPIO_Pin_2;
	UARTTX.GPIO_Mode = GPIO_Mode_AF_PP;
	UARTTX.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitTypeDef UARTRX;
	UARTRX.GPIO_Pin = GPIO_Pin_3;
	UARTRX.GPIO_Mode = GPIO_Mode_IPU;
	UARTRX.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &UARTTX);
	GPIO_Init(GPIOA, &UARTRX);

	USART_ClockStructInit(&USART_ClockInitStructure);
	USART_ClockInit(USART2, &USART_ClockInitStructure);

	USART_InitTypeDef UART2;
	UART2.USART_BaudRate = 1200;
	UART2.USART_WordLength = USART_WordLength_9b;
	UART2.USART_StopBits = USART_StopBits_1;
	UART2.USART_Parity = USART_Parity_Odd;
	UART2.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	UART2.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART2, &UART2);
	USART_Cmd(USART2, ENABLE);

	USART2_NVIC_Configuration();
}

void USART3_Config(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	USART_ClockInitTypeDef USART_ClockInitStructure;
	GPIO_InitTypeDef UARTTX;
	UARTTX.GPIO_Pin = GPIO_Pin_10;
	UARTTX.GPIO_Mode = GPIO_Mode_AF_PP;
	UARTTX.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitTypeDef UARTRX;
	UARTRX.GPIO_Pin = GPIO_Pin_11;
	UARTRX.GPIO_Mode = GPIO_Mode_IPU;
	UARTRX.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &UARTTX);
	GPIO_Init(GPIOB, &UARTRX);

	USART_ClockStructInit(&USART_ClockInitStructure);
	USART_ClockInit(USART3, &USART_ClockInitStructure);

	USART_InitTypeDef UART3;
	UART3.USART_BaudRate = 9600;
	UART3.USART_WordLength = USART_WordLength_8b;
	UART3.USART_StopBits = USART_StopBits_1;
	UART3.USART_Parity = USART_Parity_No;
	UART3.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	UART3.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART3, &UART3);
	USART_Cmd(USART3, ENABLE);

	USART3_NVIC_Configuration();
}

void USART_SendBytes(USART_TypeDef *inst, uint8_t* data, uint16_t size) {

	uint16_t i;
	for (i = 0; i < size; i++) {
		USART_SendData(inst, data[i]);
		while (USART_GetFlagStatus(inst, USART_FLAG_TC) == RESET)
			;
	}
}

void USART1_NVIC_Configuration(void) {
	//configure NVIC
	NVIC_InitTypeDef NVIC_InitStructure;
	//select NVIC channel to configure
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	//set priority to lowest
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	//set subpriority to lowest
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	//enable IRQ channel
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//update NVIC registers
	NVIC_Init(&NVIC_InitStructure);
	//enable Receive Data register not empty interrupt
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

void USART2_NVIC_Configuration(void) {
	//configure NVIC
	NVIC_InitTypeDef NVIC_InitStructure;
	//select NVIC channel to configure
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	//set priority to lowest
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	//set subpriority to lowest
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	//enable IRQ channel
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//update NVIC registers
	NVIC_Init(&NVIC_InitStructure);
	//enable Receive Data register not empty interrupt
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

void USART3_NVIC_Configuration(void) {
	//configure NVIC
	NVIC_InitTypeDef NVIC_InitStructure;
	//select NVIC channel to configure
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	//set priority to lowest
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	//set subpriority to lowest
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	//enable IRQ channel
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//update NVIC registers
	NVIC_Init(&NVIC_InitStructure);
	//enable Receive Data register not empty interrupt
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
}

void USART1_IRQHandler(void) {

	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) { // Received characters modify string

		recievebuf_usart1[usart1_rx_index++] = USART_ReceiveData(USART1);
		usart1_nofbytes++;
		if (usart1_rx_index >= (sizeof(recievebuf_usart1) - 1))
			usart1_rx_index = 0;
	}
}

void USART2_IRQHandler(void) {

	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) { // Received characters modify string

		recievebuf_usart2[usart2_rx_index++] = USART_ReceiveData(USART2);
		usart2_nofbytes++;
		if (usart2_rx_index >= (sizeof(recievebuf_usart2) - 1))
			usart2_rx_index = 0;
	}
}

void USART3_IRQHandler(void) {

	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) { // Received characters modify string

		recievebuf_usart3[usart3_rx_index++] = USART_ReceiveData(USART3);
		usart3_nofbytes++;
		if (usart3_rx_index >= (sizeof(recievebuf_usart3) - 1))
			usart3_rx_index = 0;
	}
}
uint8_t USART_GetData(USART_TypeDef* USARTx, uint8_t* array, uint8_t size) {
	uint8_t ctr, read = 0;
	if (USARTx == USART1) {
		for (ctr = 0; ctr < size; ctr++) {
			if (usart1_nofbytes > 0) {
				array[ctr] = recievebuf_usart1[usart1_read_index++];
				usart1_nofbytes--;
				read++;
				if (usart1_read_index >= (sizeof(recievebuf_usart1) - 1))
					usart1_read_index = 0;
			} else
				break;

		}

	}
	if (USARTx == USART2) {
		for (ctr = 0; ctr < size; ctr++) {
			if (usart2_nofbytes > 0) {
				array[ctr] = recievebuf_usart2[usart2_read_index++];
				usart2_nofbytes--;
				read++;
				if (usart2_read_index >= (sizeof(recievebuf_usart2) - 1))
					usart2_read_index = 0;
			} else
				break;

		}
	}
	if (USARTx == USART3) {
		for (ctr = 0; ctr < size; ctr++) {
			if (usart3_nofbytes > 0) {
				array[ctr] = recievebuf_usart3[usart3_read_index++];
				usart3_nofbytes--;
				read++;
				if (usart3_read_index >= (sizeof(recievebuf_usart3) - 1))
					usart3_read_index = 0;
			} else
				break;

		}
	}

	return read;
}

//temp test function
uint8_t CheckforData(USART_TypeDef *inst) {
	if (inst == USART1)
		return usart1_nofbytes;
	 else if (inst == USART2)
		return usart2_nofbytes;
	 else if (inst == USART3)
		return usart3_nofbytes;

	return 0;
}

void wifi_uart_flush(USART_TypeDef *inst){
	if(inst==USART2){
		usart2_nofbytes = 0;
		usart2_rx_index = 0;
		usart2_read_index = 0;

	}
}
