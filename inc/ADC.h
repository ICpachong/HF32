/*
 * ADC.h
 *
 *  Created on: May 20, 2020
 *      Author: Alka
 */

#include "main.h"
#include "targets.h"


#ifndef ADC_H_
#define ADC_H_



void ADC_DMA_Callback();
void enableADC_DMA();
void activateADC();
void ADC_Init(void);
void ADC_Init_Detector(void);
extern uint16_t adc_diagnose_data[4];


#endif /* ADC_H_ */
