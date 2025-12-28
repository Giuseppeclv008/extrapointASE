/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           joystick.h
** Last modified Date:  2025-12-28
** Descriptions:        Atomic joystick init functions
** Correlated files:    lib_joystick.c, funct_joystick.c
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#include "LPC17xx.h"
#include "joystick.h"

#define JOY_SEL     (1 << 25) // P1.25
#define JOY_DOWN    (1 << 26) // P1.26
#define JOY_LEFT    (1 << 27) // P1.27
#define JOY_RIGHT   (1 << 28) // P1.28
#define JOY_UP      (1 << 29) // P1.29
/*----------------------------------------------------------------------------
  Function that initializes joysticks and switch them off
 *----------------------------------------------------------------------------*/

void joystick_init(void) {
	/* joystick up functionality */
	LPC_GPIO1->FIODIR   &= ~(JOY_SEL);
	LPC_GPIO1->FIODIR   &= ~(JOY_DOWN);
	LPC_GPIO1->FIODIR   &= ~(1<<29);
	LPC_GPIO1->FIODIR   &= ~(1<<29);
	LPC_GPIO1->FIODIR   &= ~(1<<29);
  LPC_PINCON->PINSEL3 &= ~(3<<26);	//PIN mode GPIO (00b value per P1.29)
	LPC_GPIO1->FIODIR   &= ~(1<<29);	//P1.25 Input (joysticks on PORT1 defined as Input) 
}
