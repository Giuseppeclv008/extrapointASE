/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "RIT.h"
#include "../led/led.h"
#include "../joystick/joystick.h"
#include "../mechanics/mechanics.h"
#include "../timer/timer.h"
#include "../GUI/GUI.h"
volatile int down1 = 0;
volatile int down2 = 0;
extern volatile int paused;
void RIT_IRQHandler (void)
{			
	// il joystick non interrompere mai il RIT
	static uint8_t old_joy = 0;
	uint8_t current_joy = joystick_read();
	
	// entriamo nel blocco se il joystick cambia stato rispetto all'ultima lettura
	if(game_started && !paused && !game_over) {
		if (current_joy != old_joy) {
			switch(current_joy){
				case JOY_UP:
					rotatePiece();
					break;
				case JOY_DOWN:						
					LPC_TIM0->MR0 = FAST_PERIOD; // velocità aumentata di 2 volte, 2 square al secondo 
					break;
				case JOY_LEFT:
					movePieceLeft();
					break;
				case JOY_RIGHT:
					movePieceRight();
					break;
				case JOY_SEL:
					// Azione per select
					break;
				default:
					break;
			}
		}
		else {
			// riporto la velocità del pezzo a quella normale se il current_joy non è JOY_DOWN
			if(current_joy != JOY_DOWN){
			LPC_TIM0->MR0 = NORMAL_PERIOD;  // velocità normale 1 square al secondo
			}
		}
		old_joy = current_joy;

	}
	
	/* *********************************************** */
	/* gestione di KEY 1, debouncing con RIT		   */ 
	/* *********************************************** */
	// nell'if verifico se il pin P2.11 (KEY1) è ancora basso (premuto)
	// se è ancora basso incremento il contatore down
	// se è stato rilasciato (alto) resetto il contatore down
	// in base al valore di down eseguo l'azione corrispondente
	// disabilito EINT1 per evitare che l'interrupt del tasto interferisca
	// se LPC_PINCON->PINSEL4 è pari a 00 allora sono in modalità GPIO, semplice Input/Output GP
	// se sono in modalità EINT1, sono in "01" e sono collegato al controller degli interrupt esterni 
	// verifico quindi nell'if che il pin sia in modalità GPIO (00)
	
	if(down1>=1){
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){	// KEY1 ancora premuto , attivo basso 
			// sul FIOPIN leggo lo stato del pin P2.11 (KEY1)
		switch(down1){
			case 2: 
			if(game_over) {
				// se il gioco è finito o non è iniziato, resetto il gioco
				initializeGame();
				disable_timer();
				reset_timer();
				LED_Off(1); // spengo il led di pausa se era acceso
				break;
			}
			else{
				paused = !paused;				// attiva o/disattivo la pausa, imposto il contrario del valore attuale ogni volta che premo il tasto Key1
				if (paused){
					GUI_pauseScreen();
					disable_timer();
					LED_On(1);      // accendo il led 1 per indicare che il gioco è in pausa 
				}else{
					GUI_resumeScreen();
					enable_timer();
					LED_Off(1); 
				}					// spengo il led 1 per indicare che il gioco è ripreso 
			}
				break;
			default:
				break;
		}
		down1++;
	}else{	/* KEY1 rilasciato */
			down1 = 0;
			NVIC_EnableIRQ(EINT1_IRQn);             // riabilita EINT1
			// rinconfiguro il pin P2.11 come EINT1
			LPC_PINCON->PINSEL4 |= (1 << 22);       /* riconfigura pin come EINT */
		
		}
	}
	/* *********************************************** */
	/* gestione di KEY 2, debouncing con RIT		   */ 
	/* *********************************************** */
	if(down2>=1){
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){	// KEY2 ancora premuto , attivo basso 
			switch(down2){
			case 2: 
				if(!paused && game_started){
					hardDrop();
				}
				break;
			default:
				break;
		}
		down2++;
	}else{	/* KEY2 rilasciato */
			down2 = 0;
			NVIC_EnableIRQ(EINT2_IRQn);             // riabilita EINT
			// rinconfiguro il pin P2.12 come EINT
			LPC_PINCON->PINSEL4 |= (1 << 24);		/* riconfigura pin come EINT */
		}
	}

	LPC_RIT->RICTRL |= 1;	
	return;
}
	

			
	