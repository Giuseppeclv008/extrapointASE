
                  
#include <stdio.h>
#include "LPC17xx.h"                    /* LPC17xx definitions                */
#include "led/led.h"
#include "button_EXINT/button.h"
#include "timer/timer.h"
#include "mechanics/mechanics.h"
#include "GLCD/GLCD.h"
#include "GUI/GUI.h"

// external variables from mechanics.c
extern volatile uint32_t score;
extern volatile uint32_t HighScore;
extern volatile uint8_t game_started;
extern volatile uint8_t game_over;
extern volatile uint8_t paused;
/* Led external variables from funct_led */
extern unsigned char led_value;					/* defined in lib_led								*/

/* extern variable from IRQ_timer.c*/
extern volatile uint8_t timer_tick;

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
	init_RIT(0x004C4B40);								/* RIT Initialization 50 msec       */
	enable_RIT();
	
	init_timer();										/* TIMER0 Initialization   */ 
	LCD_Initialization();
	// Disegna l'interfaccia statica una volta sola
   GUI_DrawInterface();
   GUI_pauseScreen();

   LED_On(1);  // accendo il led 1 per indicare che il gioco è in pausa
   initializeGame();
   
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
			enable_timer();
			}
		
		if(game_started && !paused && !game_over){
			if(timer_tick==1)
			{
			timer_tick=0;
				movePieceDown();
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
			__ASM("wfi");
		}
	}

}

		

