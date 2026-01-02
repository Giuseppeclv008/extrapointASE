/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           timer.h
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        Prototypes of functions included in the lib_timer, funct_timer, IRQ_timer .c files
** Correlated files:    lib_timer.c, funct_timer.c, IRQ_timer.c
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#ifndef __TIMER_H
#define __TIMER_H

/* init_timer.c */
extern uint32_t init_timer();
extern void enable_timer();
extern void disable_timer();
extern void reset_timer();
/* IRQ_timer.c */
extern void TIMER0_IRQHandler (void);
#endif /* end __TIMER_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
