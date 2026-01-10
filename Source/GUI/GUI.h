#ifndef __GUI_H
#define __GUI_H


#include "GLCD/GLCD.h"
#include "GLCD/AsciiLib.h"
#include "stdint.h"
#include <stdio.h>


// --- DIMENSIONI PLAYING FIELD SUL DISPLAY ---
#define BLOCK_SIZE 15 // NUMERO DI PIXEL PER BLOCCO 
#define FIELD_COLS 10 // LARGHEZZA IN COLONNE DEL PLAYING FIELD
#define FIELD_ROWS 20 // ALTEZZA IN RIGHE DEL PLAYING FIELD


// --- COORDINATE PLAYING FIELD SUL DISPLAY ---
#define FIELD_X 5 // COORDINATA X INIZIALE DEL PLAYING FIELD
#define FIELD_Y 10 // COORDINATA Y INIZIALE DEL PLAYING FIELD

// COORDINATE SEZIONE STATISTCHE 
#define SCORE_X 165
#define SCORE_Y 40
#define HIGH_SCORE_Y 120
#define CLEARED_LINES_Y 200

// COLORI COMPONENTI STATICHE GUI
#define BACKGROUND_COLOR Black
#define FIELD_BORDER_COLOR White
#define FIELD_BACKGROUND_COLOR Black
#define TEXT_COLOR White
#define SCORE_COLOR Yellow
#define NUMBER_COLOR White
#define BLOCK_BORDER_COLOR White


// PROTOTIPI DELLE FUNZIONI 
void GUI_DrawInterface(void);
void GUI_RefreshInterface(void);
void GUI_UpdateScore(uint64_t previous_score);
void GUI_UpdateClearedLines(int previous_lines_cleared);
void GUI_UpdateHighScore(uint64_t previous_highscore);
void GUI_pauseScreen(void);
void GUI_resumeScreen(void);
void GUI_gameOverScreen(void);
void GUI_RefreshScreen(void);
void GUI_clearGameOverScreen(void);
void GUI_DrawBlock(uint16_t x, uint16_t y, uint16_t color);
void GUI_DrawCurrentPiece(uint16_t color);


#endif
