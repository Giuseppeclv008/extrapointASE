#include "button.h"
#include "LPC17xx.h"


#include "../led/led.h"
#include "../timer/timer.h"
#include "mechanics/mechanics.h"

int paused = 0;

void EINT0_IRQHandler (void)	  	/* INT0														 */
{
	LED_On(0);
	LPC_SC->EXTINT &= (1 << 0);     /* clear pending interrupt         */
}


void EINT1_IRQHandler (void)	  	/* KEY1														 */
{	
	paused = !paused; // attiva o/disattivo la pausa, imposto il contrario del valore attuale ogni volta che premo il tasto Key1
	if (paused)
		LED_On(1);      // accendo il led 1 per indicare che il gioco è in pausa 
	else
	    LED_Off(1);     // spengo il led 1 per indicare che il gioco è ripreso 

	LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt         */
}

void EINT2_IRQHandler (void)	  	/* KEY2														 */
{
	LED_Off(0);
	LED_Off(1);
  LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */  
	enable_timer(0);  
}


