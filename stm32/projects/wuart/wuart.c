/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2011 Stephen Caudle <scaudle@doceme.com>
 * Modified by Fernando Cortes <fermando.corcam@gmail.com>
 * modified by Guillermo Rivera <memogrg@gmail.com>
 * modified by Frantisek Burian <BuFran@seznam.cz>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>

#include <string.h>
#include <stdio.h>

#include "hwdefines.h"
#include "gpio.h"
#include "timer.h"
#include "adc.h"



void rf_on(void) {
			//gpio_set(PORT_LED, PIN_LED_R);
			gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8);
}

void rf_off(void) {
			//gpio_clear(PORT_LED, PIN_LED_R);
			gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO8);	
}


const int BITTIME_US = 10;

void send_char(uint8_t c) {
	int i;

	//start bit
	rf_on();
	timer_sleep_us(BITTIME_US);

	for(i=0; i<8; ++i) {
		if(c & (1 << 7)) {
			rf_on();
		}
		else {
			rf_off();
		}
		c <<= 1;
		timer_sleep_us(BITTIME_US);
	}
	
	//stop bit
	rf_off();
	timer_sleep_us(BITTIME_US*100);
}

void send_string(char *str) {
	while(*str)
		send_char(*str++);
}

int main(void)
{

	gpio_setup();

	gpio_clear(PORT_LED, PIN_LED_R);
	
	RCC_CR |= RCC_CR_HSEON;
	RCC_CFGR = RCC_CFGR_PLLNODIV | RCC_CFGR_MCO_PLL | RCC_CFGR_PLLMUL_MUL4 | RCC_CFGR_PLLSRC | RCC_CFGR_PLLSRC0;
	RCC_CFGR2 = RCC_CFGR2_PREDIV_NODIV;
	RCC_CR |= RCC_CR_PLLON;

	
	gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO8);
	gpio_set_af(GPIOA, GPIO_AF0, GPIO8);	
			
	gpio_set(PORT_LED, PIN_LED_G);


	rf_off();
		
		
	uint8_t channel_conf[] = { ADC_CHANNEL_TEMP, ADC_CHANNEL_VREF};
	adc_init(2, channel_conf);
	
	for(;;) {
		float vdda, temp;
		adc_get_temp(&vdda, &temp);
	
		char text[128];
		sprintf(text, "%f %f\n", temp, vdda);
		send_string(text);
		timer_sleep_ms(500);
	}
}

