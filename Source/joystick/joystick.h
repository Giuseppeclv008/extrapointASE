/**************************************************************************
** File name:       joystick.h
** Descriptions:    Prototypes & Definitions
**************************************************************************/
#ifndef __JOYSTICK_H 
#define __JOYSTICK_H

#include "LPC17xx.h"

/* Definizione dei valori di ritorno per la lettura */
#define JOY_NONE    0
#define JOY_SEL     1
#define JOY_DOWN    2
#define JOY_LEFT    3
#define JOY_RIGHT   4
#define JOY_UP      5

/* Prototipi delle funzioni */
void joystick_init(void);
uint8_t joystick_read(void);

#endif