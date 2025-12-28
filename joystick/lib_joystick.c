/**************************************************************************
** File name:       lib_joystick.c
** Descriptions:    Atomic joystick functions
**************************************************************************/
#include "joystick.h"

/* Maschere hardware (interne alla libreria) */
#define P1_25_SEL_MASK    (1 << 25)
#define P1_26_DOWN_MASK   (1 << 26)
#define P1_27_LEFT_MASK   (1 << 27)
#define P1_28_RIGHT_MASK  (1 << 28)
#define P1_29_UP_MASK     (1 << 29)

/* Maschera cumulativa per configurazione veloce */
#define JOY_ALL_MASK      (P1_25_SEL_MASK | P1_26_DOWN_MASK | P1_27_LEFT_MASK | P1_28_RIGHT_MASK | P1_29_UP_MASK)

/*----------------------------------------------------------------------------
  Function: joystick_init
  Purpose:  Configures P1.25-P1.29 as Inputs and GPIO function
 *----------------------------------------------------------------------------*/
void joystick_init(void) {
    /* 1. Configurazione Funzione Pin (PINSEL3) 
       I pin P1.25-P1.29 risiedono nel registro PINSEL3 che configura i pin da P1.16 a P1.31.

       Dobbiamo pulire i bit corrispondenti per impostarli come GPIO (00).
       P1.25 (bit 18:19), P1.26 (20:21), ... P1.29 (26:27) */
       
    // Questa maschera pulisce tutti i bit di funzione per i pin del joystick
	// i valori dei bit sul quale stiamo facendo la maschera sono stati calcolati come segue:
	// P1.25 -> (25-16)*2 = 18 -> bit 18:19 (perché PINSEL3 inizia dal pin 16)
	// P1.26 -> (26-16)*2 = 20 -> bit 20:21
	// P1.27 -> (27-16)*2 = 22 -> bit 22:23
	// P1.28 -> (28-16)*2 = 24 -> bit 24:25
	// P1.29 -> (29-16)*2 = 26 -> bit 26:27
	// Moltiplichiamo per 2 perché ogni pin è rappresentato da 2 bit nel registro PINSEL3
	// in questo modo otteniamo la maschera completa per pulire tutti i pin del joystick
	// 3 rappresenta il valore binario '11', quindi stiamo pulendo entrambi i bit per ogni pin
	// Usando l'operatore AND con la negazione della maschera, impostiamo i bit a 00
	// per selezionare la funzione GPIO 00 per i pin del joystick

    LPC_PINCON->PINSEL3 &= ~( (3<<18) | (3<<20) | (3<<22) | (3<<24) | (3<<26) );

    /* 2. Configurazione Direzione (FIODIR)
       Impostiamo i pin come INPUT (scrivendo 0) */
    LPC_GPIO1->FIODIR &= ~(JOY_ALL_MASK);
}

/*----------------------------------------------------------------------------
  Function: joystick_read
  Purpose:  Returns the code of the pressed button, or 0 if none.
 *----------------------------------------------------------------------------*/
uint8_t joystick_read(void) {
    /* Acquisizione istantanea dello stato della Porta 1 */
    uint32_t pin_state = LPC_GPIO1->FIOPIN;

    /* Verifica logica Active Low (0 = Premuto) */
    
    // Verifica SELECT (P1.25)
    if ( (pin_state & P1_25_SEL_MASK) == 0 ) {
        return JOY_SEL;
    }
    
    // Verifica DOWN (P1.26)
    if ( (pin_state & P1_26_DOWN_MASK) == 0 ) {
        return JOY_DOWN;
    }
    
    // Verifica LEFT (P1.27)
    if ( (pin_state & P1_27_LEFT_MASK) == 0 ) {
        return JOY_LEFT;
    }
    
    // Verifica RIGHT (P1.28)
    if ( (pin_state & P1_28_RIGHT_MASK) == 0 ) {
        return JOY_RIGHT;
    }
    
    // Verifica UP (P1.29)
    if ( (pin_state & P1_29_UP_MASK) == 0 ) {
        return JOY_UP;
    }

    /* Nessun tasto premuto */
    return JOY_NONE;
}