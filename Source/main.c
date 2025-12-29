/*----------------------------------------------------------------------------
 * Name:    sample.c
 * Purpose: 
 *		to control led11 and led 10 through EINT buttons (similarly to project 03_)
 *		to control leds9 to led4 by the timer handler (1 second - circular cycling)
 * Note(s): this version supports the LANDTIGER Emulator
 * Author: 	Paolo BERNARDI - PoliTO - last modified 15/12/2020
 *----------------------------------------------------------------------------
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2017 Politecnico di Torino. All rights reserved.
 *----------------------------------------------------------------------------*/

                  
#include <stdio.h>
#include "LPC17xx.h"                    /* LPC17xx definitions                */
#include "led/led.h"
#include "button_EXINT/button.h"
#include "timer/timer.h"
#include "mechanics/mechanics.h"

// external variables from mechanics.c
extern volatile int score;
extern volatile int HighScore;
extern volatile int game_started;
extern volatile int game_over;
extern volatile int paused;
/* Led external variables from funct_led */
extern unsigned char led_value;					/* defined in lib_led								*/

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif
/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
int main (void) {
  	
	SystemInit();  												/* System Initialization (i.e., PLL)  */
  BUTTON_init();												/* BUTTON Initialization              */
	init_RIT(0x004C4B40);									/* RIT Initialization 50 msec       */
  LED_init();                           /* LED Initialization                 */
  BUTTON_init();												/* BUTTON Initialization              */

	while (game_started){
		//attendo che venga premuto il tasto per iniziare il gioco
		if
	}
	
	
	init_timer(2,0.007*25*1000000, 0.023*25*1000000 );							/* TIMER2 Initialization MR0 MR1          */
	init_timer(3,0.07*25000000, 0.23*25*1000000 );						/* TIMER3 Inizialization MR0 MR1 */
	/* quello che passiamo init_timer � K */	/* K = T*Fr = [s]*[Hz] = [s]*[1/s]	  */
																						/* T = K / Fr = 0x017D7840 / 25MHz    */
																						/* T = K / Fr = 25000000 / 25MHz      */
																						/* T = 1s	(one second)   							*/
			
																						/* T = 1ms - Fr = 25MHz								*/
																						/* K = T * F = 1*10^-3 * 25*10^6			*/																

	enable_timer(2);
	enable_timer(3);
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= 0xFFFFFFFFD;						
		
  while (1) {                           /* Loop forever                       */	
		__ASM("wfi");
  }

}
