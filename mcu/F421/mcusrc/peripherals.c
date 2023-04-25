/*
 * peripherals.c
 *
 *  Created on: Sep. 26, 2020
 *      Author: Alka
 */


// PERIPHERAL SETUP


#define KR_KEY_Reload           ((uint16_t)0xAAAA)
#define KR_KEY_Enable           ((uint16_t)0xCCCC)


#include "peripherals.h"



#include "targets.h"
#include "serial_telemetry.h"
#include "common.h"
#include "functions.h"


void initCorePeripherals(void){
  system_clock_config();
	MX_GPIO_Init();
  MX_DMA_Init();
	TIM1_Init();
  TIM6_Init();
  TIM14_Init();
  AT_COMP_Init();
  TIM17_Init();
  TIM16_Init();
  	 
  UN_TIM_Init();
  #ifdef USE_SERIAL_TELEMETRY
    telem_UART_Init();
  #endif
}


void initAfterJump(void){

}


void system_clock_config(void)
{
  flash_psr_set(FLASH_WAIT_CYCLE_3);
  crm_reset();
  crm_clock_source_enable(CRM_CLOCK_SOURCE_HICK, TRUE);
  while(crm_flag_get(CRM_HICK_STABLE_FLAG) != SET)
  {
  }
  crm_pll_config(CRM_PLL_SOURCE_HICK, CRM_PLL_MULT_30);
  crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE);
  while(crm_flag_get(CRM_PLL_STABLE_FLAG) != SET)
  {
  }
  crm_ahb_div_set(CRM_AHB_DIV_1);
  crm_apb2_div_set(CRM_APB2_DIV_1);
  crm_apb1_div_set(CRM_APB1_DIV_1);
	crm_auto_step_mode_enable(TRUE);
  crm_sysclk_switch(CRM_SCLK_PLL);
  while(crm_sysclk_switch_status_get() != CRM_SCLK_PLL)
  {
  }
  crm_auto_step_mode_enable(FALSE);
  system_core_clock_update();
}


void AT_COMP_Init(void)
{
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_CMP_PERIPH_CLOCK, TRUE);
    /* configure PA1 as comparator input */
     gpio_mode_QUICK(GPIOA, GPIO_MODE_ANALOG, GPIO_PULL_NONE, GPIO_PINS_1);
   //  rcu_periph_clock_enable(RCU_CFGCMP);

     gpio_mode_QUICK(GPIOA, GPIO_MODE_ANALOG, GPIO_PULL_NONE, GPIO_PINS_5);
    /* configure comparator channel0 */
		
		NVIC_SetPriority(ADC1_CMP_IRQn, 0);
    NVIC_EnableIRQ(ADC1_CMP_IRQn);
		
	//	COMP_Cmd(COMP1_Selection, ENABLE);
		cmp_enable(CMP1_SELECTION, TRUE);
}


void MX_IWDG_Init(void)
{
#if !defined USE_DEBUG
	WDT->cmd = WDT_CMD_UNLOCK;
	WDT->cmd = WDT_CMD_ENABLE;
  WDT->div = WDT_CLK_DIV_32;
	WDT->rld = 4000;
	WDT->cmd = WDT_CMD_RELOAD;
#endif	
}

void TIM1_Init(void){
	crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK, TRUE);
	TMR1->pr = 3000;
	TMR1->div = 0;
	
	TMR1->cm1 = 0x6868;   // Channel 1 and 2 in PWM output mode
	TMR1->cm2 = 0x68;     // channel 3 in PWM output mode

	/*tmr_output_config_type tmr_oc_init_structure;
	tmr_output_default_para_init(&tmr_oc_init_structure);
	
	tmr_output_struct->oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
  tmr_output_struct->oc_idle_state = FALSE;
  tmr_output_struct->occ_idle_state = FALSE;
  tmr_output_struct->oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct->occ_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct->oc_output_state = TRUE;
  tmr_output_struct->occ_output_state = FALSE;
	
	tmr_oc_init_structure.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
	tmr_oc_init_structure.oc_idle_state = FALSE;
	tmr_oc_init_structure.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
	tmr_oc_init_structure.oc_output_state = TRUE;
	tmr_output_channel_config(TMR1, TMR_SELECT_CHANNEL_1, &tmr_oc_init_structure);*/
	
	#ifdef USE_INVERTED_HIGH
		TMR1->cctrl_bit.c1p=TMR_OUTPUT_ACTIVE_LOW;
		TMR1->cctrl_bit.c2p=TMR_OUTPUT_ACTIVE_LOW;
		TMR1->cctrl_bit.c3p=TMR_OUTPUT_ACTIVE_LOW;
		TMR1->ctrl2_bit.c1ios=TRUE;
		TMR1->ctrl2_bit.c2ios=TRUE;
		TMR1->ctrl2_bit.c3ios=TRUE;
  #else
		TMR1->cctrl_bit.c1p=TMR_OUTPUT_ACTIVE_HIGH;
		TMR1->cctrl_bit.c2p=TMR_OUTPUT_ACTIVE_HIGH;
		TMR1->cctrl_bit.c3p=TMR_OUTPUT_ACTIVE_HIGH;
		TMR1->ctrl2_bit.c1ios=FALSE;
		TMR1->ctrl2_bit.c2ios=FALSE;
		TMR1->ctrl2_bit.c3ios=FALSE;
  #endif
  #ifdef USE_INVERTED_LOW
		TMR1->cctrl_bit.c1cp=TMR_OUTPUT_ACTIVE_LOW;
		TMR1->cctrl_bit.c2cp=TMR_OUTPUT_ACTIVE_LOW;
		TMR1->cctrl_bit.c3cp=TMR_OUTPUT_ACTIVE_LOW;
		TMR1->ctrl2_bit.c1cios=TRUE;
		TMR1->ctrl2_bit.c2cios=TRUE;
		TMR1->ctrl2_bit.c3cios=TRUE;
  #else
		TMR1->cctrl_bit.c1cp=TMR_OUTPUT_ACTIVE_HIGH;
		TMR1->cctrl_bit.c2cp=TMR_OUTPUT_ACTIVE_HIGH;
		TMR1->cctrl_bit.c3cp=TMR_OUTPUT_ACTIVE_HIGH;
		TMR1->ctrl2_bit.c1cios=FALSE;
		TMR1->ctrl2_bit.c2cios=FALSE;
		TMR1->ctrl2_bit.c3cios=FALSE;
  #endif
	
	
	
	tmr_output_channel_buffer_enable(TMR1,TMR_SELECT_CHANNEL_1, TRUE);
	tmr_output_channel_buffer_enable(TMR1,TMR_SELECT_CHANNEL_2, TRUE);
	tmr_output_channel_buffer_enable(TMR1,TMR_SELECT_CHANNEL_3, TRUE);

	tmr_period_buffer_enable(TMR1, TRUE);
	
	TMR1->brk_bit.dtc = DEAD_TIME;

	crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
	
	    /*configure PA8/PA9/PA10(TIMER0/CH0/CH1/CH2) as alternate function*/
    gpio_mode_QUICK(PHASE_A_GPIO_PORT_LOW, GPIO_MODE_MUX, GPIO_PULL_NONE, PHASE_A_GPIO_LOW);


    gpio_mode_QUICK(PHASE_B_GPIO_PORT_LOW, GPIO_MODE_MUX, GPIO_PULL_NONE, PHASE_B_GPIO_LOW);


    gpio_mode_QUICK(PHASE_C_GPIO_PORT_LOW, GPIO_MODE_MUX, GPIO_PULL_NONE, PHASE_C_GPIO_LOW);

gpio_pin_mux_config(PHASE_A_GPIO_PORT_LOW, PHASE_A_PIN_SOURCE_LOW, GPIO_MUX_2);
gpio_pin_mux_config(PHASE_B_GPIO_PORT_LOW, PHASE_B_PIN_SOURCE_LOW, GPIO_MUX_2);
gpio_pin_mux_config(PHASE_C_GPIO_PORT_LOW, PHASE_C_PIN_SOURCE_LOW, GPIO_MUX_2);


    /*configure PB13/PB14/PB15(TIMER0/CH0N/CH1N/CH2N) as alternate function*/
    gpio_mode_QUICK(PHASE_A_GPIO_PORT_HIGH, GPIO_MODE_MUX, GPIO_PULL_NONE, PHASE_A_GPIO_HIGH);

    gpio_mode_QUICK(PHASE_B_GPIO_PORT_HIGH, GPIO_MODE_MUX, GPIO_PULL_NONE, PHASE_B_GPIO_HIGH);

    gpio_mode_QUICK(PHASE_C_GPIO_PORT_HIGH, GPIO_MODE_MUX, GPIO_PULL_NONE, PHASE_C_GPIO_HIGH);

gpio_pin_mux_config(PHASE_A_GPIO_PORT_HIGH, PHASE_A_PIN_SOURCE_HIGH, GPIO_MUX_2);
gpio_pin_mux_config(PHASE_B_GPIO_PORT_HIGH, PHASE_B_PIN_SOURCE_HIGH, GPIO_MUX_2);
gpio_pin_mux_config(PHASE_C_GPIO_PORT_HIGH, PHASE_C_PIN_SOURCE_HIGH, GPIO_MUX_2);

#if defined(USE_INNER_STEP)
  tmr_interrupt_enable(TMR1, TMR_OVF_INT, TRUE);
	//TMR_C1_FLAG
  /* tmr1 overflow interrupt nvic init */
  //nvic_priority_group_config(NVIC_PRIORITY_GROUP_0);
  nvic_irq_enable(TMR1_BRK_OVF_TRG_HALL_IRQn, 0, 0);
#endif

}



void TIM6_Init(void)
{
	crm_periph_clock_enable(CRM_TMR6_PERIPH_CLOCK, TRUE);
	TMR6->pr = 0xFFFF;
	TMR6->div = 59;

}


void TIM14_Init(void)
{
	crm_periph_clock_enable(CRM_TMR14_PERIPH_CLOCK, TRUE);

	TMR14->pr = 100;
	TMR14->div = 119;
	
	
	NVIC_SetPriority(TMR14_GLOBAL_IRQn, 2);
  NVIC_EnableIRQ(TMR14_GLOBAL_IRQn);
	
}


void TIM16_Init(void)
{
	crm_periph_clock_enable(CRM_TMR16_PERIPH_CLOCK, TRUE);
	TMR16->pr = 500;
	TMR16->div = 59;
	TMR16->ctrl1_bit.prben = TRUE;
	
	NVIC_SetPriority(TMR16_GLOBAL_IRQn, 0);
  NVIC_EnableIRQ(TMR16_GLOBAL_IRQn);
}



void TIM17_Init(void)
{
	crm_periph_clock_enable(CRM_TMR17_PERIPH_CLOCK, TRUE);
	TMR17->pr = 0xFFFF;
	//TMR17->div = 59;
	TMR17->div = CPU_FREQUENCY_MHZ-1;
TMR17->ctrl1_bit.prben = TRUE;

}


void MX_DMA_Init(void)
{
crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
	
  NVIC_SetPriority(DMA1_Channel5_4_IRQn, 1);
  NVIC_EnableIRQ(DMA1_Channel5_4_IRQn);

}

void MX_GPIO_Init(void)
{

}


void UN_TIM_Init(void)
{
#ifdef USE_TIMER_3_CHANNEL_1
	
	crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK, TRUE);
	gpio_mode_QUICK(INPUT_PIN_PORT, GPIO_MODE_MUX, GPIO_PULL_NONE, INPUT_PIN);
	gpio_pin_mux_config(INPUT_PIN_PORT, INPUT_PIN_SOURCE, GPIO_MUX_1);
#endif
#ifdef USE_TIMER_15_CHANNEL_1
	
	crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_TMR15_PERIPH_CLOCK, TRUE);
  gpio_mode_QUICK(INPUT_PIN_PORT, GPIO_MODE_MUX, GPIO_PULL_NONE, INPUT_PIN);
#endif
 
crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
	
	
//  DMA_DefaultInitParaConfig(&DMA_InitStructure);

//  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&IC_TIMER_REGISTER->CC1;
//  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&dma_buffer;
//  DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALSRC;
//  DMA_InitStructure.DMA_BufferSize = 32;
//  DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;
//  DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;
//  DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_HALFWORD;
//  DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_WORD;
//  DMA_InitStructure.DMA_Mode = DMA_MODE_NORMAL;
//  DMA_InitStructure.DMA_Priority = DMA_PRIORITY_LOW;
//  DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;
//  DMA_Init(INPUT_DMA_CHANNEL, &DMA_InitStructure);

  INPUT_DMA_CHANNEL->ctrl = 0X98a; //  PERIPHERAL HALF WORD, MEMROY WORD , MEMORY INC ENABLE , TC AND ERROR INTS


   NVIC_SetPriority(IC_DMA_IRQ_NAME, 1);
   NVIC_EnableIRQ(IC_DMA_IRQ_NAME);

	

	
	IC_TIMER_REGISTER->pr = 0xFFFF;
	IC_TIMER_REGISTER->div = 16;
	IC_TIMER_REGISTER->ctrl1_bit.prben = TRUE;
	IC_TIMER_REGISTER->ctrl1_bit.tmren = TRUE;
		
}

#ifdef USE_RGB_LED              // has 3 color led
void LED_GPIO_init(){
	  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	  /* GPIO Ports Clock Enable */
	  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

	  LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_8);
      LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_5);
	  LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_3);

	  GPIO_InitStruct.Pin = LL_GPIO_PIN_8;
	  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
	  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	  GPIO_InitStruct.Pin = LL_GPIO_PIN_5;
	  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
	  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	  GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
	  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
	  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

#endif



