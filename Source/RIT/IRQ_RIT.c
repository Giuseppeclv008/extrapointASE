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
#include "../music/music.h"
#include "../adc/adc.h"


#define RIT_SEMIMINIMA 8
#define RIT_MINIMA 16
#define RIT_INTERA 32

#define UPTICKS 1


volatile uint8_t down1 = 0;
volatile uint8_t down2 = 0;
extern volatile uint8_t paused;
extern volatile uint8_t game_started;
extern volatile uint8_t game_over;
extern volatile uint64_t current_period;
extern volatile int slowDownTicks;
volatile uint8_t flag_hard_drop = 0;

extern volatile uint8_t play_sfx_flag;
extern volatile NOTE* current_sfx_ptr;
extern volatile int sfx_note_count;

NOTE song[] = {
    // --- PARTE A (Melodia Principale) ---
    
    // Battuta 1: E (lunga), B, C
    {e4, time_croma * 2},
    {b3, time_croma},
    {c4, time_croma},
    
    // Battuta 2: D (lunga), C, B
    {d4, time_croma * 2},
    {c4, time_croma},
    {b3, time_croma},
    
    // Battuta 3: A (lunga), A, C
    {a3, time_croma * 2},
    {a3, time_croma},
    {c4, time_croma},
    
    // Battuta 4: E (lunga), D, C
    {e4, time_croma * 2},
    {d4, time_croma},
    {c4, time_croma},
    
    // Battuta 5: B (molto lunga), C
    {b3, time_croma * 3},
    {c4, time_croma},
    
    // Battuta 6: D (lunga), E (lunga)
    {d4, time_croma * 2},
    {e4, time_croma * 2},
    
    // Battuta 7: C (lunga), A (lunga)
    {c4, time_croma * 2},
    {a3, time_croma * 2},
    
    // Battuta 8: A (lunga), Pausa
    {a3, time_croma * 2},
    {pause, time_croma * 2},

    // --- PARTE B (Ponte Alto) ---

    // Battuta 9: D (molto lunga), F
    {d4, time_croma * 3},
    {f4, time_croma},
    
    // Battuta 10: A (lunga), G, F
    {a4, time_croma * 2},
    {g4, time_croma},
    {f4, time_croma},
    
    // Battuta 11: E (molto lunga), C
    {e4, time_croma * 3},
    {c4, time_croma},
    
    // Battuta 12: E (lunga), D, C
    {e4, time_croma * 2},
    {d4, time_croma},
    {c4, time_croma},
    
    // Battuta 13: B (lunga), B, C
    {b3, time_croma * 2},
    {b3, time_croma},
    {c4, time_croma},
    
    // Battuta 14: D (lunga), E (lunga)
    {d4, time_croma * 2},
    {e4, time_croma * 2},
    
    // Battuta 15: C (lunga), A (lunga)
    {c4, time_croma * 2},
    {a3, time_croma * 2},
    
    // Battuta 16: A (lunga), Pausa finale
    {a3, time_croma * 2},
    {pause, time_croma * 4}
};

extern  NOTE sfx_clear_lines[];
extern  NOTE sfx_slow_down[];

void RIT_IRQHandler (void)
{			
	// il joystick non interrompere mai il RIT
	static uint8_t old_joy = 0;
	uint8_t current_joy = joystick_read();
	// entriamo nel blocco se il joystick cambia stato rispetto all'ultima lettura

	ADC_start_conversion();
	if(game_started && !paused && !game_over) {
			if (current_joy != old_joy) {
				switch(current_joy){
					case JOY_UP:
						rotatePiece();
						break;
					case JOY_DOWN:
						if(slowDownTicks == 0){
							uint32_t fast_period = current_period / 2;

							LPC_TIM0->MR0 = fast_period; // velocità aumentata di 2 volte

							if(LPC_TIM0->TC >= fast_period){
								LPC_TIM0->TC = fast_period-1; //forzo al conteggio
							}
						}
						
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
					if(	LPC_TIM0->MR0 != current_period && slowDownTicks == 0){
						LPC_TIM0->MR0 = current_period;  // velocità normale 1 square al secondo
					
					}
						
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
				disable_timer(0);
				reset_timer(0);
				init_timer(0, NORMAL_PERIOD);
				LED_Off(1); // spengo il led di pausa se era acceso
				break;
			}
			else{
				paused = !paused;				// attiva o/disattivo la pausa, imposto il contrario del valore attuale ogni volta che premo il tasto Key1
				if (paused){
					GUI_pauseScreen();
					disable_timer(0);
					LED_On(1);      // accendo il led 1 per indicare che il gioco è in pausa 
				}else{
					GUI_resumeScreen();
					enable_timer(0);
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
					flag_hard_drop = 1;
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
	if(slowDownTicks != 0 && slowDownTicks > 0 && !paused){
		slowDownTicks--;
		if(slowDownTicks == 0) GUI_clearSlowDown();
	}
	if(slowDownTicks == 0){
		// sono passati 15 secondi 
		LPC_TIM0->MR0 = current_period;

	}
	/*  ***********************************************  */
	/* 				SONG PART & SFX	 				     */
	/*  ***********************************************  */


	
	// Variabili statiche per la gestione dello stato SFX all'interno del RIT
	static int currentSfxNoteIndex = 0;
	static int sfxTicks = 0;
	static int currentNote = 0;
	static int ticks = 0;
	if (play_sfx_flag){
		if(!isNotePlaying())
		{
			++sfxTicks;
			if(sfxTicks == UPTICKS)
			{
				sfxTicks = 0;
				playNote(current_sfx_ptr[currentSfxNoteIndex++]);
			}
		}
		if(currentSfxNoteIndex >= sfx_note_count)
		{
			play_sfx_flag = 0;
			currentSfxNoteIndex = 0;
			sfxTicks = 0;
		}
	}else if (game_started && !paused && !game_over){
		if(!isNotePlaying())
		{
			++ticks;
			if(ticks == UPTICKS)
			{
				
				int song_length = sizeof(song)/sizeof(song[0]);
				NOTE original_note;
				uint32_t current_speed_reg; 
				uint32_t scaled_duration;
				NOTE new_note;

				ticks = 0;


				if(currentNote >= song_length) currentNote = 0;


				original_note = song[currentNote];
				currentNote++;

				current_speed_reg = LPC_TIM0->MR0;

				scaled_duration = (uint32_t) ((uint64_t) original_note.duration * current_speed_reg) ;

				if(scaled_duration < 1) scaled_duration = 1;

				new_note.freq = original_note.freq;
				new_note.duration = scaled_duration;

				playNote(new_note);
			}
		}
	}

	LPC_RIT->RICTRL |= 1;	
	return;
}

			
		/*  ***********************************************  */
	/* 					 SONG PART						 */
	/*  ***********************************************  */
	/* static int currentNote = 0;
	static int ticks = 0;
	if(!isNotePlaying())
	{
		++ticks;
		if(ticks == UPTICKS)
		{
		ticks = 0;
		playNote(song[currentNote++]);
		}
	}
	if(currentNote == (sizeof(song)/sizeof(song[0])) ) 
	{
		currentNote = 0; // resetto la musica a partire dal primo elemento nell'arrey delle note 
	}*/