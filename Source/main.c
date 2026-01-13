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
#include "GUI/GUI.h"
#include "adc/adc.h"


// external variables from mechanics.c
extern volatile uint64_t score;
extern volatile uint64_t HighScore;
extern volatile uint8_t game_started;
extern volatile uint8_t game_over;
extern volatile uint8_t paused;
/* Led external variables from funct_led */
extern unsigned char led_value;					/* defined in lib_led								*/

/* extern variable from IRQ_timer.c*/
extern volatile uint8_t timer_tick;
/* extern variable from IRQ_RIT.c*/
extern volatile uint8_t flag_hard_drop;

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif
/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
int main (void) {
  uint8_t first = 1;
	SystemInit();  											/* System Initialization (i.e., PLL)  */
	BUTTON_init();											/* BUTTON Initialization              */
	LED_init();                         /* LED Initialization                 */
	joystick_init();										/* joystick Initialization                 */
	ADC_init();
	
	init_RIT(0x004C4B40);								/* RIT Initialization 50 msec       */
	//configurazione DAC
	LPC_PINCON->PINSEL1 |= (1<<21); 
    LPC_PINCON->PINSEL1 &= ~(1<<20);
    LPC_GPIO0->FIODIR |= (1<<26);

	init_timer(0, NORMAL_PERIOD);										/* TIMER0 Initialization MR0 MR1 ad 1 e 2 secondi     */ 
	init_timer(1, 0);
	init_timer(2, 0);


   LCD_Initialization();
   GUI_DrawInterface();
   GUI_pauseScreen();
	 
	enable_RIT(); // accendo il RIT qui perchè il RITCOUNT è necessario per la modifica del seed x
	LED_On(1);  // accendo il led 1 per indicare che il gioco è in pausa

   initializeGame();

	// abilito gli interupt dopo aver eseguito le operazioni più "costose"
	enable_timer(0);

   

  

   
	while(1){
		if (first){
	
			// main game loop
			while(paused){
				__ASM("wfi");
			}
			srand(LPC_RIT->RICOUNTER); // inizializzo il seme del generatore di numeri casuali, modifica il seed ad ogni reset
				SpawnNewPiece();
				first = 0;
				game_started = 1;
				enable_timer(0);
			}
		
		if(game_started && !paused && !game_over){
			if(timer_tick==1)
			{
			timer_tick=0;
			int previous_score = score;
				movePieceDown();
			}

			if(flag_hard_drop == 1){
				flag_hard_drop = 0;
				hardDrop();
			}
			__ASM("wfi");
		}
		else if(game_over){
			HighScore =(score > HighScore) ? score : HighScore;
			GUI_gameOverScreen();
			first = 1;
			// blocco il gioco finché non si resetta
			while(game_over){
				__ASM("wfi");
			}
			first = 1;
			GUI_DrawInterface();
			GUI_pauseScreen();
		}
		else{
			LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
			LPC_SC->PCON &= ~(0x2);						
			// gioco in pausa o non iniziato
			__ASM("wfi");
		}
	}

}

		

