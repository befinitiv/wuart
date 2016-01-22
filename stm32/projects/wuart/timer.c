#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencmsis/core_cm3.h>




#define TIM_CLK 8e6
#define PRESCALER_US 8
#define PRESCALER_MS 8e3


void timer_sleep(int prescaler, int auto_reload_value) {
	rcc_periph_clock_enable(RCC_TIM6);
	

	TIM6_PSC = prescaler-1;
	TIM6_CNT = 0;
	TIM6_ARR = auto_reload_value;


	TIM6_SR = 0;
	TIM6_EGR = TIM_EGR_UG;
	TIM6_DIER = TIM_DIER_UIE;
	TIM6_CR1 = TIM_CR1_OPM | TIM_CR1_URS | TIM_CR1_CEN;

	TIM6_SR &= ~TIM_SR_UIF;

	//TODO: instead of busy waiting the uc should go into stop mode and wakeup when the timer throws an interrupt
	// however, the timer interrupt does not wake up the uc... this could help to save some power
	while(!(TIM6_SR & TIM_SR_UIF));

//	while(!(TIM6_SR & TIM_SR_UIF)) {

//#define SCB_SCR_SEVONPEND (1 << 4)
//	SCB_SCR |= SCB_SCR_SEVONPEND;
//
//	nvic_clear_pending_irq(17);
//
//	asm volatile ("wfe");
//
//
//	TIM6_SR |= TIM_SR_UIF;
//	}

	rcc_periph_clock_disable(RCC_TIM6);

}



void timer_sleep_us(int us) {
	timer_sleep(PRESCALER_US, us);
}

void timer_sleep_ms(int ms) {
	timer_sleep(PRESCALER_MS, ms);
}
