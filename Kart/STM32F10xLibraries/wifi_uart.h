#ifdef __cplusplus
 extern "C" {
#endif

#define LED_ON() GPIO_ResetBits(GPIOB,GPIO_Pin_5);
#define LED_OFF() GPIO_SetBits(GPIOB,GPIO_Pin_5);

#define U2_WRITE() GPIO_ResetBits(GPIOA,GPIO_Pin_1);
#define U2_READ() GPIO_SetBits(GPIOA,GPIO_Pin_1);

void USART1_Config(void);

void USART2_Config(void);

void USART3_Config(void);

void USART1_NVIC_Configuration(void);

void USART2_NVIC_Configuration(void);

void USART3_NVIC_Configuration(void);

void USART_SendBytes(USART_TypeDef *inst, uint8_t* data, uint16_t size);

uint8_t USART_GetData(USART_TypeDef* USARTx, uint8_t* array, uint8_t size);

uint8_t CheckforData(USART_TypeDef *inst);

void wifi_uart_flush(USART_TypeDef *inst);
