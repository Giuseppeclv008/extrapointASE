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


/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

volatile int down=0;

void RIT_IRQHandler (void)
{					
	// Setto il flag per la lettura del joystick
	uint8_t joy_value = joystick_read();

	switch (joy_value) {
		case JOY_LEFT:
			movePieceLeft();
			break;
		case JOY_RIGHT:
			movePieceRight();
			break;
		case JOY_DOWN:
			down=1;
			movePieceDown();
			break;
		case JOY_UP:
			rotatePiece();
			break;
		default:
			break;
	}
  return;


}

/******************************************************************************
**                            End Of File
******************************************************************************/
