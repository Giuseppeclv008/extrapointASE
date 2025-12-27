#include "mechanics.h"
#include <stdio.h>
#include "LPC17xx.h"                    /* LPC17xx definitions                */
#include "led/led.h"
#include "button_EXINT/button.h"
#include "timer/timer.h"


#define HEIGHT 20
#define WIDTH 10
#define PIECE_I_INDEX 0 // Indice del pezzo I nell'array TETROMINOS
#define PIECE_O_INDEX 1 // Indice del pezzo O nell'array TETROMINOS
#define PIECE_T_INDEX 2 // Indice del pezzo T nell'array TETROMINOS
#define PIECE_J_INDEX 3 // Indice del pezzo J nell'array TETROMINOS
#define PIECE_L_INDEX 4 // Indice del pezzo L nell'array TETROMINOS
#define PIECE_S_INDEX 5 // Indice del pezzo S nell'array TETROMINOS
#define PIECE_Z_INDEX 6 // Indice del pezzo Z nell'array TETROMINOS


// variabili globali
volatile int playing_field[HEIGHT][WIDTH] = {0};
volatile int score = 0;
volatile int HighScore = 0;
volatile int game_started = 0;
volatile int game_over = 0;
volatile int paused = 0;

// x ed y sono le coordinate di ancoraggio del pezzo rispetto all'playing_field, si riferiscono all'angolo 
// sinisto della matrice shape 

// considerando un'playing_field 20x10 la coordinata massima per il current piece può essere (16,0)
// posizionando al limite la matrice 4x4 e non uscire fuori dai bordi 
typedef struct{
	int x; // coordinata x del pezzo nel playing field 
	int y; // coordinata y del pezzo nel playing field 
    int type; //lo uso per vedere se sono permesse delle posizioni iniziali nell'playing_field
    int rotation; //per determinare la posizione e muovermi nella matrice per le posizioni 
	int shape[4][4];
} ActiveTetromino;

ActiveTetromino currentPiece;
// Usiamo uint8_t perché ci basta 0 o 1, non serve un intero a 32 bit.

//matrice di matrici 4x4, ognuna delle 7 righe è dedicata ad un pezzo diverso 
// per orgni riga ho 4 colonne rappresentanti tutte le possibili rotazioni del pezzo 
// ogni pezzo è rappresenatato da una matrice 4x4
const uint8_t TETROMINOS[7][4][4][4] = {
    // --- PEZZO I (Linea) ---
    {
        { {0,0,0,0}, 
          {1,1,1,1},
          {0,0,0,0}, 
          {0,0,0,0} }, // Rotazione 0
         
        { {0,0,1,0},
          {0,0,1,0}, 
          {0,0,1,0},
          {0,0,1,0} }, // Rotazione 1

        { {0,0,0,0},
          {1,1,1,1}, 
          {0,0,0,0}, 
          {0,0,0,0} }, // Rotazione 2
         
        { {0,0,1,0}, 
          {0,0,1,0}, 
          {0,0,1,0}, 
          {0,0,1,0} }  // Rotazione 3
    },
     // --- PEZZO O (Quadrato) ---
    // Il quadrato è uguale in tutte le rotazioni
    {
        { {0,1,1,0},
          {0,1,1,0}, 
          {0,0,0,0}, 
          {0,0,0,0} },

        { {0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0} },
        { {0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0} },
        { {0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0} }
    },
    // --- PEZZO T ---
    {
        { {0,1,0,0},
          {1,1,1,0}, 
          {0,0,0,0}, 
          {0,0,0,0} },

        { {0,1,0,0}, 
          {0,1,1,0}, 
          {0,1,0,0}, 
          {0,0,0,0} },

        { {0,0,0,0}, 
          {1,1,1,0}, 
          {0,1,0,0}, 
          {0,0,0,0} },

        { {0,1,0,0}, 
          {1,1,0,0}, 
          {0,1,0,0}, 
          {0,0,0,0} }
    },
    // --- PEZZO J ---
    {
        { {1,0,0,0}, 
          {1,1,1,0}, 
          {0,0,0,0},
          {0,0,0,0} },

        { {0,1,1,0}, 
          {0,1,0,0}, 
          {0,1,0,0}, 
          {0,0,0,0} },

        { {0,0,0,0},
          {1,1,1,0}, 
          {0,0,1,0}, 
          {0,0,0,0} },

        { {0,1,0,0}, 
          {0,1,0,0}, 
          {1,1,0,0}, 
          {0,0,0,0} }
    },
    // --- PEZZO L ---
    {
        { {0,0,1,0}, 
          {1,1,1,0}, 
          {0,0,0,0}, 
          {0,0,0,0} },

        { {0,1,0,0}, 
          {0,1,0,0}, 
          {0,1,1,0}, 
          {0,0,0,0} },

        { {0,0,0,0}, 
          {1,1,1,0}, 
          {1,0,0,0}, 
          {0,0,0,0} },

        { {1,1,0,0},  
          {0,1,0,0}, 
          {0,1,0,0}, 
          {0,0,0,0} }
    },
    // --- PEZZO S ---
    {
        { {0,1,1,0}, 
          {1,1,0,0}, 
          {0,0,0,0}, 
          {0,0,0,0} },

        { {0,1,0,0}, 
          {0,1,1,0}, 
          {0,0,1,0}, 
          {0,0,0,0} },

        { {0,0,0,0}, 
          {0,1,1,0}, 
          {1,1,0,0}, 
          {0,0,0,0} },

        { {1,0,0,0}, 
          {1,1,0,0}, 
          {0,1,0,0}, 
          {0,0,0,0} }
    },
    
    // --- PEZZO Z ---
    {
        { {1,1,0,0}, 
          {0,1,1,0}, 
          {0,0,0,0}, 
          {0,0,0,0} },

        { {0,0,1,0}, 
          {0,1,1,0}, 
          {0,1,0,0}, 
          {0,0,0,0} },

        { {0,0,0,0}, 
          {1,1,0,0}, 
          {0,1,1,0}, 
          {0,0,0,0} },

        { {0,1,0,0}, 
          {1,1,0,0}, 
          {1,0,0,0}, 
          {0,0,0,0} }
    }
};

void init_piece(){
	currentPiece.x = 0;
	currentPiece.y = 0;
    int shape[4][4] = {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};
	currentPiece.shape = shape;
}

void SpawnNewPiece(){
    // wrapper per la generazione di un nuovo pezzo che si occupa 
    // di generare un numero casuale fra 0 e 6 per la scelta del pezzo 
    // ed un numero casuale fra 0 e 4 per la scelta casuale della rotazione 
    int pieceIndex = rand() % 7;
    // la posizione iniziale è di sicuro in alto X = 0 ma puntando le nostre coordinate 
    // all'angolo sinistro in alto della matrice rappresentate un pezzo 
    // la posizione limite che previene uno sforamento dell'playing_field è (0, 6)
    int initialX = rand() % 7;
    SpawnPiece(pieceIndex, initialX, 0);
}

void SpawnPiece(int pieceIndex, int initialX, int initialY) {
    // 1. Imposta coordinate iniziali 
    currentPiece.x = initialX; 
    currentPiece.y = initialY; 
    
    // 2. Salviamo il tipo e resettiamo la rotazione
    int rotationIndex = 0;
    currentPiece.type = pieceIndex;
    currentPiece.rotation = rotationIndex; 

    // 3. Copiamo la forma iniziale (Rotazione 0) dalla memoria costante
    int r,c;
    for (r = 0; r < 4; r++) {
        for (c = 0; c < 4; c++) {
            
            // Accesso: [Tipo][Rotazione 0][Riga][Colonna]
            currentPiece.shape[r][c] = TETROMINOS[pieceIndex][rotationIndex][r][c];
            
        }
    }
}

void rotatePiece() {
    // Aggiorna l'indice di rotazione
    currentPiece.rotation = (currentPiece.rotation + 1) % 4;

    // Aggiorna la matrice shape del pezzo corrente
	  int r ;
	  int c ;
    for ( r = 0; r < 4; r++) {
        for( c = 0; c < 4; c++) {
            currentPiece.shape[r][c] = TETROMINOS[currentPiece.type][currentPiece.rotation][r][c];
        }
    }
}

void movePieceLeft() {
  if (currentPiece.x > 0) currentPiece.x--;
}

void movePieceRight() {
  if (currentPiece.x < WIDTH - 4) currentPiece.x++;
}
void movePieceDown() {
    currentPiece.y++;
    score += 1; // aumenta il punteggio ad ogni discesa del pezzo
}



void handlePieceLock(void) {
    // 1. Solidifica il pezzo nella matrice del playing_field
    lockPiece();

    // 2. Controlla le linee e ottieni il numero
    int linesRemoved = deleteFullLines();

    // 3. LOGICA DELL'AZIONE SPECIALE
    if (linesRemoved > 0) {
        
        // Caso "TETRIS": 4 Linee cancellate con il pezzo I
        if (linesRemoved == 4 && currentPiece.type == PIECE_I_INDEX) {
            
            // A. Assegna un punteggio bonus enorme
            score += 600; // Bonus extra per il TETRIS
            
            // B. Feedback Visivo sulla LandTiger 
            //Todo: Implementa la funzione UpdateScoreDisplay(score);
            
            // C. Feedback Sonoro  
            // PlaySound(SOUND_TETRIS_EFFECT);
            
        } else {
            // Punteggio normale per 1, 2 o 3 linee
            // Esempio classico Nintendo: 40, 100, 300 punti
            switch(linesRemoved) {
                case 1: score += 100; break;
                case 2: score += 200; break;
                case 3: score += 300; break;
            }
        }
        
        // Ridisegna l'interfaccia col nuovo punteggio
        // TODO: UpdateScoreDisplay(score);
        
        // Ridisegna l'playing_field pulita
        // TODO : Redrawplaying_field();
    }
}
void lockPiece() {
    // Blocca il pezzo corrente nell'playing_field
    int r,c;
    for (r = 0; r < 4; r++) {
        for ( c = 0; c < 4; c++) {
            if (currentPiece.shape[r][c] != 0) {
                int fieldX = currentPiece.x + c;
                int fieldY = currentPiece.y + r;
                if (fieldY >= 0 && fieldY < HEIGHT && fieldX >= 0 && fieldX < WIDTH) {
                    playing_field[fieldY][fieldX] = currentPiece.shape[r][c];
                }
            }
        }
    }
    score += 10; // aumenta il punteggio quando un pezzo viene bloccato

}

int deleteFullLines(void) {
  int linesCleared = 0;
  int y, x;

  // Scansioniamo dal basso (riga 19) verso l'alto
  for (y = HEIGHT - 1; y >= 0; y--) {
      int isFull = 1;
      
      for (x = 0; x < WIDTH; x++) {
          if (playing_field[y][x] == 0) {
              isFull = 0;
              break;
          }
      }

      if (isFull) {
          linesCleared++; 
          
          // Fai scendere tutto ciò che c'è sopra
          // (Copia la riga y-1 in y, y-2 in y-1, ecc...)
          int c, r;
          for (r = y; r > 0; r--) {
              for (c = 0; c < WIDTH; c++) {
                  playing_field[r][c] = playing_field[r-1][c];
              }
          }
          // Pulisci la riga 0 (quella nuova che entra dall'alto)
          
          for (c = 0; c < WIDTH; c++) {
              playing_field[0][c] = 0;
          }
          
          // IMPORTANTE: Poiché tutto è sceso, dobbiamo ricontrollare 
          // la riga attuale 'y' al prossimo giro, quindi incrementiamo y
          // (che verrà decrementato dal for loop subito dopo)
          y++; 
      }
  }
  
  return linesCleared; // Restituisce 0, 1, 2, 3 o 4
}