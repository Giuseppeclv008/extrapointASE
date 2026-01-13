/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_adc.c
** Last modified Date:  20184-12-30
** Last Version:        V1.00
** Descriptions:        functions to manage A/D interrupts
** Correlated files:    adc.h
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#include "LPC17xx.h"
#include "adc.h"
#include "../led/led.h"
#include "../timer/timer.h"
#include "mechanics/mechanics.h"

/*----------------------------------------------------------------------------
  A/D IRQ: Executed when A/D Conversion is ready (signal from ADC peripheral)
 *----------------------------------------------------------------------------*/

unsigned short AD_current;   
unsigned short AD_last = 0xFF;     /* Last converted value               */

/* k=1/f'*f/n  k=f/(f'*n) k=25MHz/(f'*45) */

//const int freqs[8]={4240,3779,3367,3175,2834,2525,2249,2120};
/* 
131Hz		k=4240 C3
147Hz		k=3779
165Hz		k=3367
175Hz		k=3175
196Hz		k=2834		
220Hz		k=2525
247Hz		k=2249
262Hz		k=2120 C4
*/
/*
262Hz	k=2120		c4
294Hz	k=1890		
330Hz	k=1684		
349Hz	k=1592		
392Hz	k=1417		
440Hz	k=1263		
494Hz	k=1125		
523Hz	k=1062		c5

*/
extern volatile int slowDownTicks ;
volatile uint32_t current_period = NORMAL_PERIOD;

void ADC_IRQHandler(void) {
  	
	AD_current = ((LPC_ADC->ADGDR>>4) & 0xFFF);/* Read Conversion Result             */
	int diff = (int)AD_current - (int)AD_last;
	uint32_t new_period;
	if(diff < 0) diff = -diff ;
  
	  if(diff > 40){
		  uint32_t max_period = NORMAL_PERIOD;
		  uint32_t min_period = 5000000;
		  // fattore di scala: (25M - 5M) / 4095 = 4885
		  uint32_t decrement = 4884;
		  uint32_t reduction = AD_current * decrement; 
  
		  if(reduction >= (max_period - min_period))
		  {
			  new_period = min_period;
		  }else{
			  new_period = max_period - reduction;
		  }
		  // se il powerup di slowdown è attivo non devo modificare il match register
		  current_period = new_period;
		  if(slowDownTicks == 0){
	
				LPC_TIM0->MR0 = new_period;
				if(LPC_TIM0->TC >= new_period){
					LPC_TIM0->TC = new_period-1; //forzo al conteggio
				}
			}
		  AD_last = AD_current;
		}
	  
}
  

  
  


