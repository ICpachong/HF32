/*
 * dshot.c
 *
 *  Created on: Apr. 22, 2020
 *      Author: Alka
 */

#include "functions.h"
#include "dshot.h"
#include "targets.h"
#include "common.h"
#include "main.h"

int dpulse[16] = {0} ;

const char gcr_encode_table[16] = { 0b11001,
		0b11011,
		0b10010,
		0b10011,
		0b11101,
		0b10101,
		0b10110,
		0b10111,
		0b11010,
		0b01001,
		0b01010,
		0b01011,
		0b11110,
		0b01101,
		0b01110,
		0b01111
};

int shift_amount = 0;
uint32_t gcrnumber;
extern int e_com_time;
extern int zero_crosses;
extern char send_telemetry;
extern int smoothedinput;
extern uint8_t max_duty_cycle_change;
int dshot_full_number;
extern char play_tone_flag;
uint8_t command_count = 0;
uint8_t last_command = 0;
uint8_t high_pin_count = 0;
uint32_t gcr[26] =  {0};
uint16_t dshot_frametime;
uint16_t dshot_goodcounts;
uint16_t dshot_badcounts;
char dshot_extended_telemetry = 0;
uint16_t send_extended_dshot = 0;

void computeDshotDMA(){


const int j = 0;
dshot_frametime = dma_buffer[31]- dma_buffer[0];
uint32_t dpulsbits=0;
#if defined MCU_AT421

				if((dshot_frametime < 3500)&&(dshot_frametime > 2800)){
								
				for (int i = 0; i < 16; i++){
					dpulsbits<<=1;
					if(dma_buffer[j + (i<<1) +1] - dma_buffer[j + (i<<1)]>=100){
						dpulsbits|=1;
					}
				}
#endif
#if defined MCU_AT415

				if((dshot_frametime < 5000)&&(dshot_frametime > 3000)){
						
				for (int i = 0; i < 16; i++){
					dpulsbits<<=1;
					if(dma_buffer[j + (i<<1) +1] - dma_buffer[j + (i<<1)]>=120){
						dpulsbits|=1;
					}
				}
#endif
				
				int calcCRC = ((dpulsbits>>12 )^ (dpulsbits>>8) ^ (dpulsbits>>4))&0xf;
				int checkCRC = dpulsbits &0xf;		
				

				if(!armed){
					if (dshot_telemetry == 0){
						 #if 0
						 if((INPUT_PIN_PORT->idt & INPUT_PIN)){  // if the pin is high for 100 checks between signal pulses its inverted
							 high_pin_count++;
							 if(high_pin_count > 100){
								 dshot_telemetry = 1;
							 }
						 }
						 #else
						 if(calcCRC == ~checkCRC+16){
							high_pin_count++;
							 if(high_pin_count > 4){
								 dshot_telemetry = 1;
							 }
						 }else{
							 high_pin_count=0;
						 }
						 #endif
					}
				}
				if(dshot_telemetry){
					checkCRC= ~checkCRC+16;
				}
				int tocheck =	dpulsbits>>5;

				if(calcCRC == checkCRC){
					signaltimeout = 0;
					dshot_goodcounts++;
					if(dpulsbits&(1<<4)){
                    send_telemetry=1;
					}
					if (tocheck > 47){


						newinput = tocheck;
	                    dshotcommand = 0;
	                    command_count = 0;
	                    return;
					}

				if ((tocheck <= 47)&& (tocheck > 0)){
					newinput = 0;
					dshotcommand = tocheck;    //  todo
				}
				if (tocheck == 0){
					newinput = 0;
					dshotcommand = 0;
					command_count = 0;
				}

				if ((dshotcommand > 0) && (running == 0) && armed) {
					if(dshotcommand != last_command){
						last_command = dshotcommand;
						command_count = 0;
					}
					if(dshotcommand < 5){    // beacons
						command_count = 6;      //go on right away
					}
					command_count++;
					if(command_count >= 6){
						command_count = 0;
						switch (dshotcommand){                   // todo

						case 1:
							playInputTune();
						break;
						case 2:
							playInputTune2();
								break;
						case 3:
							playBeaconTune3();
						break;
						case 7:
							dir_reversed = 0;
							forward = 1 - dir_reversed;
							play_tone_flag = 1;
							break;
						case 8:
								dir_reversed = 1;
								forward = 1 - dir_reversed;
								play_tone_flag = 2;
							break;
						case 9:
							bi_direction = 0;
							armed = 0;
							zero_input_count = 0;
							break;
						case 10:
							bi_direction = 1;
							zero_input_count = 0;
							armed = 0;
							break;
						case 12:
							saveEEpromSettings();
							//delayMillis(100);
						//	NVIC_SystemReset();
							break;
						case 13:
							dshot_extended_telemetry = 1;
							send_extended_dshot = 0b111000000000;
							//make_dshot_package();
							break;
						case 14:
							dshot_extended_telemetry = 0;
							send_extended_dshot = 0b111011111111;
							//make_dshot_package();
							break;
						case 20:
							forward = 1 - dir_reversed;
							break;
						case 21:
							forward = dir_reversed;
						break;

						}
						last_dshot_command = dshotcommand;
						dshotcommand = 0;
					}
				}
			}else{
				dshot_badcounts++;
			}

		}
}



#if 0
void make_dshot_package(){
if(send_extended_dshot > 0){
  dshot_full_number = send_extended_dshot;
  send_extended_dshot = 0;
}else{
  if (!running){
	  e_com_time = 65535;
  }
//	calculate shift amount for data in format eee mmm mmm mmm, first 1 found in first seven bits of data determines shift amount
// this allows for a range of up to 65408 microseconds which would be shifted 0b111 (eee) or 7 times.
for (int i = 15; i >= 9 ; i--){
	if(e_com_time >> i == 1){
		shift_amount = i+1 - 9;
		break;
	}else{
		shift_amount = 0;
	}
}
// shift the commutation time to allow for expanded range and put shift amount in first three bits
	dshot_full_number = ((shift_amount << 9) | (e_com_time >> shift_amount));

}
//calculate checksum
	uint16_t  csum = 0;
	uint16_t csum_data = dshot_full_number;
		  for (int i = 0; i < 3; i++) {
		      csum ^=  csum_data;   // xor data by nibbles
		      csum_data >>= 4;
		  }
		  csum = ~csum;       // invert it
		  csum &= 0xf;

		  dshot_full_number = (dshot_full_number << 4)  | csum; // put checksum at the end of 12 bit dshot number

// GCR RLL encode 16 to 20 bit

		  gcrnumber = gcr_encode_table[(dshot_full_number >> 12)] << 15  // first set of four digits
		  | gcr_encode_table[(((1 << 4) - 1) & (dshot_full_number >> 8))] << 10  // 2nd set of 4 digits
		  | gcr_encode_table[(((1 << 4) - 1) & (dshot_full_number >> 4))] << 5  //3rd set of four digits
		  | gcr_encode_table[(((1 << 4) - 1) & (dshot_full_number >> 0))];  //last four digits
//GCR RLL encode 20 to 21bit output

#ifdef MCU_AT421
		  gcr[1+3] = 78;
		  for( int i= 19; i >= 0; i--){              // each digit in gcrnumber
			  gcr[3+20-i+1] = ((((gcrnumber &  1 << i )) >> i) ^ (gcr[3+20-i]>>6)) *78;        // exclusive ored with number before it multiplied by 64 to match output timer.
		  }
          gcr[3] = 0;
#endif
#ifdef MCU_AT415
		  gcr[1+3] = 97;
		  for( int i= 19; i >= 0; i--){              // each digit in gcrnumber
			  gcr[3+20-i+1] = ((((gcrnumber &  1 << i )) >> i) ^ (gcr[3+20-i]>>6)) *97;        // exclusive ored with number before it multiplied by 64 to match output timer.
		  }
          gcr[3] = 0;
#endif

}
#else




void make_dshot_package(){
if(send_extended_dshot > 0){
  dshot_full_number = send_extended_dshot;
  send_extended_dshot = 0;
}else{
  if (!running){
	  e_com_time = 65535;
  }
//	calculate shift amount for data in format eee mmm mmm mmm, first 1 found in first seven bits of data determines shift amount
// this allows for a range of up to 65408 microseconds which would be shifted 0b111 (eee) or 7 times.


#define checkbit(i) if(e_com_time & (1<<i)){shift_amount = i+1 - 9;break;}
	do{
		checkbit(15);
		checkbit(14);
		checkbit(13);
		checkbit(12);
		checkbit(11);
		checkbit(10);
		checkbit(9);
		shift_amount = 0;
	}while(0);
	
	/*shift_amount = 0;
	for(int i=15;i>=9;--i){
		checkbit(i);
	}*/
	
// shift the commutation time to allow for expanded range and put shift amount in first three bits
	dshot_full_number = ((shift_amount << 9) | (e_com_time >> shift_amount));

}
//calculate checksum
	
	int csum = dshot_full_number ^ (dshot_full_number>>4) ^(dshot_full_number>>8) ^(dshot_full_number>>16);
	csum = ~csum;       // invert it
	csum &= 0xf;

// GCR RLL encode 16 to 20 bit
		  gcrnumber = gcr_encode_table[(dshot_full_number >> 8)] << 15  // first set of four digits
		  | gcr_encode_table[(((1 << 4) - 1) & (dshot_full_number >> 4))] << 10  // 2nd set of 4 digits
		  | gcr_encode_table[(((1 << 4) - 1) & (dshot_full_number >> 0))] << 5  //3rd set of four digits
		  | gcr_encode_table[csum];  //last four digits
//GCR RLL encode 20 to 21bit output

#ifdef MCU_AT421
		  const int timerdt=78;
#endif
#ifdef MCU_AT415
		const int timerdt=97;
#endif			
			gcr[1+3] = timerdt;
			int prev=1;
		  for(int i= 19; i >= 0; i--){ // each digit in gcrnumber
				prev=(int)((gcrnumber>> i) &  1) ^ prev;
			  gcr[3+20-i+1] = prev ? timerdt:0;
		  }
      gcr[3] = 0;	

}
#endif