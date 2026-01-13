#ifndef MECHANICS_H
#define MECHANICS_H

#include <stdint.h>
#include "GLCD/GLCD.h"
/* --- DEFINIZIONI COSTANTI --- */
#define HEIGHT 20
#define WIDTH 10

// Indici dei pezzi
#define PIECE_I_INDEX 0 // Indice del pezzo I nell'array TETROMINOS
#define PIECE_O_INDEX 1 // Indice del pezzo O nell'array TETROMINOS
#define PIECE_T_INDEX 2 // Indice del pezzo T nell'array TETROMINOS
#define PIECE_J_INDEX 3 // Indice del pezzo J nell'array TETROMINOS
#define PIECE_L_INDEX 4 // Indice del pezzo L nell'array TETROMINOS
#define PIECE_S_INDEX 5 // Indice del pezzo S nell'array TETROMINOS
#define PIECE_Z_INDEX 6 // Indice del pezzo Z nell'array TETROMINOS

// Numero di Powerups
#define NUM_POWERUP_TYPES 2

/* --- STRUTTURE DATI --- */
// x ed y sono le coordinate di ancoraggio del pezzo rispetto all'playing_field, si riferiscono all'angolo 
// sinisto della matrice shape 

typedef struct {
    int x;              // coordinata x nell'arena
    int y;              // coordinata y nell'arena
    int type;           // tipo di pezzo (0-6)
    int rotation;       // rotazione corrente (0-3)
    int shape[4][4];    // matrice locale del pezzo
} ActiveTetromino;



/* --- VARIABILI GLOBALI (EXTERN) --- */
extern volatile uint8_t timer_tick;
extern volatile uint8_t hardDrop_flag;
extern volatile uint16_t playing_field[HEIGHT][WIDTH];
extern volatile uint64_t score;
extern volatile uint64_t HighScore;
extern volatile uint8_t game_started;
extern volatile uint8_t game_over;
extern volatile uint8_t paused;

extern volatile ActiveTetromino currentPiece;
extern const uint8_t TETROMINOS[7][4][4][4];

extern uint16_t POWERUP_COLORS[2];
extern volatile int slowDownTicks;

typedef enum powerups{
    CLEAR_H_LINES = 12,
    SLOW_DOWN = 13
} POWERUP;


/* --- PROTOTIPI DI FUNZIONE --- */
void init_piece(void);
void SpawnNewPiece(void);
void SpawnPiece(int pieceIndex, int initialX, int initialY);
void rotatePiece(void);
void movePieceLeft(void);
void movePieceRight(void);
void movePieceDown(void);
void handlePieceLock(void);
void lockPiece(void);
uint16_t deleteFullLines(void);
uint8_t tryMoveDown(void);
void hardDrop(void);
void initializeGame(void);
void initializePlayingField(void);
void slowDown(void);


#endif /* MECHANICS_H */