/*
 * IO.c
 *
 *  Created on: Sep. 26, 2020
 *      Author: Alka
 */

#include "targets.h"
#include "IO.h"
#include "dshot.h"
#include "serial_telemetry.h"
#include "functions.h"
#include "common.h"

char ic_timer_prescaler = 16;
char output_timer_prescaler;
int buffersize = 32;
int smallestnumber = 0;
uint32_t dma_buffer[64] = {0};
char out_put = 0;
char buffer_divider = 44;
int dshot_runout_timer = 62500;
uint32_t average_signal_pulse;

void changeToOutput(){
	INPUT_DMA_CHANNEL->ctrl |= DMA_DIR_MEMORY_TO_PERIPHERAL;
	   tmr_reset(IC_TIMER_REGISTER);
	  IC_TIMER_REGISTER->cm1 = 0x60;            // oc mode pwm
	IC_TIMER_REGISTER->cctrl = 0x3;         // 
	IC_TIMER_REGISTER->div = output_timer_prescaler;
	IC_TIMER_REGISTER->pr = DSHOT_PRE;
	
	  out_put = 1;
	IC_TIMER_REGISTER->swevt_bit.ovfswtr = TRUE;
}

void changeToInput(){
#ifdef MCU_AT415
gpio_mode_QUICK(GPIOB, GPIO_MODE_INPUT, GPIO_PULL_NONE, INPUT_PIN);
#endif
	INPUT_DMA_CHANNEL->ctrl |= DMA_DIR_PERIPHERAL_TO_MEMORY;

	tmr_reset(IC_TIMER_REGISTER);
	IC_TIMER_REGISTER->cm1 = 0x1;
	IC_TIMER_REGISTER->cctrl = 0xB;
	IC_TIMER_REGISTER->div = ic_timer_prescaler;
	IC_TIMER_REGISTER->pr = 0xFFFF;
	
	  IC_TIMER_REGISTER->swevt_bit.ovfswtr = TRUE;
	  out_put = 0;

}
void receiveDshotDma(){

	changeToInput();
	IC_TIMER_REGISTER->cval = 0;
	INPUT_DMA_CHANNEL->paddr = (uint32_t)&IC_TIMER_REGISTER->c1dt;
	INPUT_DMA_CHANNEL->maddr = (uint32_t)&dma_buffer;
	INPUT_DMA_CHANNEL->dtcnt = buffersize;
	IC_TIMER_REGISTER->iden |= TMR_C1_DMA_REQUEST;
	IC_TIMER_REGISTER->ctrl1_bit.tmren = TRUE;
	INPUT_DMA_CHANNEL->ctrl = 0x0000098b;

}

void sendDshotDma(){

	changeToOutput();
	
	INPUT_DMA_CHANNEL->paddr = (uint32_t)&IC_TIMER_REGISTER->c1dt;
	INPUT_DMA_CHANNEL->maddr = (uint32_t)&gcr;
	INPUT_DMA_CHANNEL->dtcnt = 26;
	INPUT_DMA_CHANNEL->ctrl |= DMA_FDT_INT;
	INPUT_DMA_CHANNEL->ctrl |= DMA_DTERR_INT;
	INPUT_DMA_CHANNEL->ctrl_bit.chen = TRUE;
	IC_TIMER_REGISTER->iden |= TMR_C1_DMA_REQUEST;
	IC_TIMER_REGISTER->brk_bit.oen = TRUE;
	IC_TIMER_REGISTER->ctrl1_bit.tmren = TRUE;
#ifdef MCU_AT415
gpio_mode_QUICK(INPUT_PIN_PORT, GPIO_MODE_MUX, GPIO_PULL_NONE, INPUT_PIN);
#endif
}


void detectInput(){
	smallestnumber = 20000;
	average_signal_pulse = 0;
	dshot = 0;
	servoPwm = 0;
	int lastnumber = dma_buffer[0];



	int sigcount=0;
	for ( int j = 1 ; j < 31; j++){
		if(dma_buffer[j] - lastnumber > 0 ){
			if((dma_buffer[j] - lastnumber) < smallestnumber){
					smallestnumber = dma_buffer[j] - lastnumber;
			}
			average_signal_pulse += (dma_buffer[j] - lastnumber);
			++sigcount;
		}
		lastnumber = dma_buffer[j];
	}
	if(sigcount<=0)sigcount=1;
	average_signal_pulse = average_signal_pulse/sigcount ;
#ifdef MCU_AT415
	if ((smallestnumber > 1)&&(smallestnumber <= 5)&& (average_signal_pulse < 70)) {
#endif			
#ifdef MCU_AT421		
	if ((smallestnumber > 1)&&(smallestnumber <= 4)&& (average_signal_pulse < 60)) {
#endif		
		ic_timer_prescaler= 0;
		output_timer_prescaler=1;
		dshot = 1;
		buffer_divider = 44;
		dshot_runout_timer = 65000;
		armed_count_threshold = 10000;
		buffersize = 32;
	}
#ifdef MCU_AT415
    if ((smallestnumber > 5 )&&(smallestnumber <= 10)&& (average_signal_pulse < 100)){
#endif	
#ifdef MCU_AT421
	if ((smallestnumber > 4 )&&(smallestnumber <= 8)&& (average_signal_pulse < 100)){
#endif		
		dshot = 1;
		ic_timer_prescaler=1;
		output_timer_prescaler=3;
		IC_TIMER_REGISTER->cval = 0xffff;
		buffer_divider = 44;
		dshot_runout_timer = 65000;
		armed_count_threshold = 10000;
		buffersize = 32;
	}
//	if ((smallestnumber > 100 )&&(smallestnumber < 400)){
//		multishot = 1;
//		armed_count_threshold = 1000;
//		buffersize = 4;
//	}
//	if ((smallestnumber > 2000 )&&(smallestnumber < 3000)){
//		oneshot42 = 1;
//	}
		if (smallestnumber > 30 && smallestnumber < 20000){
			if((average_signal_pulse-smallestnumber)/(float)smallestnumber<0.1f){
				servoPwm = 1;
				ic_timer_prescaler=119;
				armed_count_threshold = 35;
				buffersize = 2;
			}
		}

	if(dshot||servoPwm){
		inputSet = 1;
	}else{
		inputSet = 0;
	}
	/*if (smallestnumber == 0 || smallestnumber == 20000){
		inputSet = 0;
	}else{
		inputSet = 1;
	}*/
	UTILITY_TIMER->c1dt = smallestnumber;
}
























