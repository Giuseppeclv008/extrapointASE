#ifndef MECHANICS_H
#define MECHANICS_H

#include <stdint.h>
#include "GLCD/GLCD.h"
/* --- DEFINIZIONI COSTANTI --- */
#define HEIGHT 20
#define WIDTH 10

// Indici dei pezzi
#define PIECE_I_INDEX 0
#define PIECE_O_INDEX 1
#define PIECE_T_INDEX 2
#define PIECE_J_INDEX 3
#define PIECE_L_INDEX 4
#define PIECE_S_INDEX 5
#define PIECE_Z_INDEX 6

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

extern volatile int playing_field[HEIGHT][WIDTH];
extern volatile int score;
extern volatile int HighScore;
extern volatile int game_started;
extern volatile int game_over;
extern volatile int paused;

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
int deleteFullLines(void);
int futurePosition(void);
void initializeGame(void);
void initializePlayingField(void);
#endif /* MECHANICS_H */