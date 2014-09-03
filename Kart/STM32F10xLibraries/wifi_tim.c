#include "cmsis_boot/stm32f10x.h"
#include "stm_lib/inc/stm32f10x_rcc.h"
#include "stm_lib/inc/stm32f10x_tim.h"
#include "stm_lib/inc/misc.h"
#include "stm_lib/inc/wifi_uart.h"
#include "stm_lib/inc/wifi_tim.h"

uint8_t no_of_secs = 0;

uint8_t tim2_flag = 0;

uint8_t tim3_flag = 0;

void wifi_tim_tim2_init(void) {

	TIM_DeInit(TIM2);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM2, DISABLE);

	NVIC_InitTypeDef nvicStructure;
	nvicStructure.NVIC_IRQChannel = TIM2_IRQn;
	nvicStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	nvicStructure.NVIC_IRQChannelSubPriority = 0x0F;
	nvicStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicStructure);

	TIM_TimeBaseInitTypeDef timerInitStructure;
	timerInitStructure.TIM_Prescaler = 8000 - 1;
	timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStructure.TIM_Period = 1000 - 1;
	timerInitStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIM2, &timerInitStructure);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	TIM_Cmd(TIM2, ENABLE);

	TIM_DeInit(TIM3);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM3, DISABLE);

	nvicStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_Init(&nvicStructure);

	timerInitStructure.TIM_Period = 2000 - 1;
	TIM_TimeBaseInit(TIM3, &timerInitStructure);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	TIM_Cmd(TIM3, ENABLE);

}

void TIM2_IRQHandler(void) {
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) {
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

		no_of_secs++;
		if (no_of_secs == 60) {
			tim2_flag = 1;
			no_of_secs = 0;
		}

	}

}

uint8_t* wifi_tim_get_time(void) {
	return &tim2_flag;
}

uint8_t* wifi_tim_get_sec(void) {
	return &no_of_secs;
}

void TIM3_IRQHandler(void) {
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET) {
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		tim3_flag = 1;

	}

}

uint8_t* wifi_tim3_get_time(void) {
	return &tim3_flag;
}
