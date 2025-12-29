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

volatile int down = 0;
extern volatile int paused;
void RIT_IRQHandler (void)
{			
	// il joystick non interromper mai il RIT
	static uint8_t old_joy = 0;
	uint8_t current_joy = joystick_read();
	
	// entriamo nel blocco se il joystick cambia stato rispetto all'ultima lettura
	if (current_joy != old_joy) {
		switch(current_joy){
			case JOY_UP:
				rotatePiece();
				break;
			case JOY_DOWN:
				LPC_TIM0->MR0 = 0.5 * 25000000; 
				LPC_TIM0->TC = 0;  // Reset immediato del contatore per applicare subito la velocità
				LED_On(3);      / 
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
	else if (old_joy == JOY_DOWN && current_joy != JOY_DOWN) {
		// riporto la velocità del pezzo a quella normale
		LPC_TIM0->MR0 = 1 * 25000000; 
		LPC_TIM0->TC = 0;  // Reset immediato del contatore per applicare subito la velocità
	}
	old_joy = current_joy;


	// gestione di KEY 1, debouncing con RIT
	if(down != 0) { // Se down != 0 significa che EINT1 ha attivato la sequenza
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){	/* KEY1 ancora premuto */
			down++;
			switch(down){
				case 1: 
					paused = !paused; // attiva o/disattivo la pausa, imposto il contrario del valore attuale ogni volta che premo il tasto Key1
					if (paused)
						LED_On(1);      // accendo il led 1 per indicare che il gioco è in pausa 
					else
						LED_Off(1);     // spengo il led 1 per indicare che il gioco è ripreso 
						break;
				default:
					break;
			}
		}
		else {	/* KEY1 rilasciato */
			down = 0;
			// non disabilito il RIT per permettere al joystick di funzionare
			
			NVIC_EnableIRQ(EINT1_IRQn);             // riabilita EINT1
			LPC_PINCON->PINSEL4 |= (1 << 22);       /* riconfigura pin come EINT */
		}
	}			
	
	LPC_RIT->RICTRL |= 1;	
}	




