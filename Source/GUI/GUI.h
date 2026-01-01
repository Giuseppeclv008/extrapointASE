#ifndef __GUI_H
#define __GUI_H


#include "GLCD/GLCD.h"
#include "GLCD/AsciiLib.h"
#include "stdint.h"


// --- DIMENSIONI PLAYING FIELD SUL DISPLAY ---
#define BLOCK_SIZE 15 // NUMERO DI PIXEL PER BLOCCO 
#define FIELD_COLS 10 // LARGHEZZA IN COLONNE DEL PLAYING FIELD
#define FIELD_ROWS 20 // ALTEZZA IN RIGHE DEL PLAYING FIELD


// --- COORDINATE PLAYING FIELD SUL DISPLAY ---
#define FIELD_X 5 // COORDINATA X INIZIALE DEL PLAYING FIELD
#define FIELD_Y 10 // COORDINATA Y INIZIALE DEL PLAYING FIELD

// COORDINATE SEZIONE PUNTEGGIO 
#define SCORE_X 165
#define SCORE_Y 40
#define HIGH_SCORE_Y 120

// COLORI COMPONNENTI STATICHE GUI
#define BACKGROUND_COLOR Black
#define FIELD_BORDER_COLOR White
#define FIELD_BACKGROUND_COLOR Black
#define TEXT_COLOR White
#define SCORE_COLOR Yellow
#define NUMBER_COLOR White

// PROTOTIPI DELLE FUNZIONI 
void GUI_DrawInterface(void);
void GUI_RefreshInterface(void);
void GUI_UpdateScore(void);
void GUI_UpdateHighScore(void);
void GUI_pauseScreen(void);
void GUI_resumeScreen(void);
void GUI_gameOverScreen(void);

#endif
