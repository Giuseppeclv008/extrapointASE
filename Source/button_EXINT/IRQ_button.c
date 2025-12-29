#include "button.h"
#include "LPC17xx.h"


#include "../led/led.h"
#include "../timer/timer.h"
#include "mechanics/mechanics.h"

volatile int paused = 0;
extern volatile int down;
void EINT0_IRQHandler (void)	  	/* INT0														 */
{

	LPC_SC->EXTINT &= (1 << 0);     /* clear pending interrupt         */
}


void EINT1_IRQHandler (void)	  	/* KEY1														 */
{	
	down = 1;							/* inizio sequenza di debouncing	 */
	enable_RIT();										/* enable RIT to count 50ms				 */
	
	LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt         */
}

void EINT2_IRQHandler (void)	  	/* KEY2														 */
{
	LPC_SC->EXTINT &= (1 << 0);     /* clear pending interrupt         */
}


