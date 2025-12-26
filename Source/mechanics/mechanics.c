#include "mechanics.h"
#include <stdio.h>
#include "LPC17xx.h"                    /* LPC17xx definitions                */
#include "led/led.h"
#include "button_EXINT/button.h"
#include "timer/timer.h"


#define HEIGHT 20
#define WIDTH 10

int playing_field[HEIGHT][WIDTH] = {0};

// x ed y sono le coordinate di ancoraggio del pezzo rispetto all'arena, si riferiscono all'angolo 
// sinisto della matrice shape 

// considerando un'arena 20x10 la coordinata massima per il current piece può essere (16,0)
// posizionando al limite la matrice 4x4 e non uscire fuori dai bordi 
typedef struct{
	int x; // coordinata x del pezzo nel playing field 
	int y; // coordinata y del pezzo nel playing field 
    int type; //lo uso per vedere se sono permesse delle posizioni iniziali nell'arena
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
    // la posizione limite che previene uno sforamento dell'arena è (0, 6)
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
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            
            // Accesso: [Tipo][Rotazione 0][Riga][Colonna]
            currentPiece.shape[r][c] = TETROMINOS[pieceIndex][rotationIndex][r][c];
            
        }
    }
}