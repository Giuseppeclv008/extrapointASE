#include "music.h"

#include "../timer/timer.h"
#include "LPC17xx.h"

void playNote(NOTE note)
{
	if(note.freq != pause)
	{	
		init_timer(1, note.freq);
		reset_timer(1);
		enable_timer(1);
	}
	init_timer(2, note.duration);
	reset_timer(2);
	enable_timer(2);
}

BOOL isNotePlaying(void)
{
	return ((LPC_TIM1->TCR != 0) || (LPC_TIM2->TCR != 0));
}
