#ifndef MECHANICS_H
#define MECHANICS_H

#include <stdint.h>
#include "GLCD/GLCD.h"
/* --- DEFINIZIONI COSTANTI --- */
#define HEIGHT 20
#define WIDTH 10

/* --- STRUTTURE DATI --- */
// x ed y sono le coordinate di ancoraggio del pezzo rispetto all'playing_field, si riferiscono all'angolo 
// sinisto della matrice shape 

// considerando un'playing_field 20x10 la coordinata massima per il current piece può essere (16,0)
// posizionando al limite la matrice 4x4 e non uscire fuori dai bordi 

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
extern volatile uint32_t score;
extern volatile uint32_t HighScore;
extern volatile uint8_t game_started;
extern volatile uint8_t game_over;
extern volatile uint8_t paused;

extern volatile ActiveTetromino currentPiece;
extern const uint8_t TETROMINOS[7][4][4][4];

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
#endif /* MECHANICS_H */