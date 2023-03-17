/*
 * functions.c
 *
 *  Created on: Sep. 26, 2020
 *      Author: Alka
 */

#include "functions.h"
#include "targets.h"

#ifdef MCU_AT421
void gpio_mode_set(uint32_t mode, uint32_t pull_up_down, uint32_t pin)
{
 INPUT_PIN_PORT->cfgr = (((((INPUT_PIN_PORT->cfgr))) & (~(((pin * pin) * (0x3UL << (0U)))))) | (((pin * pin) * mode)));
 INPUT_PIN_PORT->pull = ((((((INPUT_PIN_PORT->pull))) & (~(((pin * pin) * (0x3UL << (0U)))))) | (((pin * pin) * pull_up_down))));
}


void gpio_mode_QUICK(gpio_type* gpio_periph, uint32_t mode, uint32_t pull_up_down, uint32_t pin){
gpio_periph->cfgr = (((((gpio_periph->cfgr))) & (~(((pin * pin) * (0x3UL << (0U)))))) | (((pin * pin) * mode)));
}
#endif

#ifdef MCU_AT415
void gpio_mode_set(uint32_t mode, uint32_t pull_up_down, uint32_t pin)
{
	__disable_irq();
	
 	gpio_init_type gpio_init_struct;
	gpio_default_para_init(&gpio_init_struct);
	gpio_init_struct.gpio_mode = mode;
    gpio_init_struct.gpio_pins = pin;
	gpio_init_struct.gpio_pull = pull_up_down;
	gpio_init(GPIOB, &gpio_init_struct);
	
	__enable_irq();
}

void gpio_mode_QUICK(gpio_type* gpio_periph, uint32_t mode, uint32_t pull_up_down, uint32_t pin)
{
	__disable_irq();
	gpio_init_type gpio_init_struct;
	gpio_default_para_init(&gpio_init_struct);
	
if (GPIO_MODE_MUX){
}
	
	gpio_init_struct.gpio_mode = mode;
    gpio_init_struct.gpio_pins = pin;
	gpio_init_struct.gpio_pull = pull_up_down;

  gpio_init(gpio_periph, &gpio_init_struct);
	
	__enable_irq();
	
}
#endif

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
	if (x < in_min){
		x = in_min;
	}
	if (x > in_max){
		x = in_max;
	}
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;

}


int getAbsDif(int number1, int number2){
	int result = number1 - number2;
	if (result < 0) {
	    result = -result;
	}
	return result;
}


void delayMicros(uint32_t micros){
UTILITY_TIMER->cval = 0;

	while (UTILITY_TIMER->cval < micros){

	}
}
void delayMillis(uint32_t millis){
	UTILITY_TIMER->cval = 0; 
	UTILITY_TIMER->div = (CPU_FREQUENCY_MHZ * 100);
	UTILITY_TIMER->swevt |= TMR_OVERFLOW_SWTRIG;
	while (UTILITY_TIMER->cval < (millis*10)){
		WDT->cmd = WDT_CMD_RELOAD;
	}
	UTILITY_TIMER->div = CPU_FREQUENCY_MHZ;
	UTILITY_TIMER->swevt |= TMR_OVERFLOW_SWTRIG;
}