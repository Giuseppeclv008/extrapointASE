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
#include "GLCD/GLCD.h"

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
  	
	SystemInit();  											/* System Initialization (i.e., PLL)  */
	BUTTON_init();											/* BUTTON Initialization              */
	init_RIT(0x004C4B40);									/* RIT Initialization 50 msec       */
  	LED_init();                           					/* LED Initialization                 */
  	BUTTON_init();											/* BUTTON Initialization              */
	init_timer(0 , 1 );										/* TIMER0 Initialization MR0 MR1 ad 1 e 2 secondi     */ 
	enable_timer(0);
	initializeGame();	

	GUI_Init();

	GUI_SetBkColor(Black);
	LCD_Clear(Black);
    GUI_Text(..., "PRESS KEY1 TO START");

	while(1){
		if(game_started && !paused && !game_over){ 
			// main game loop
			// inserisco refresh del display qui se pesante
			// oppure semplicemente dormo
			__ASM("wfi");
		}
		else if(game_over){
			HighScore =(score > HighScore) ? score : HighScore;
			GUI_Text(..., "GAME OVER - PRESS KEY1 TO PLAY AGAIN");

			// blocco il gioco finché non si resetta
			while(game_over){
				__ASM("wfi");
			}
		}
		else{
			// gioco in pausa o non iniziato
			__ASM("wfi");
		}
	}

}

		

