#include "mechanics.h"
#include <stdio.h>
#include "LPC17xx.h"                    /* LPC17xx definitions                */
#include "led/led.h"
#include "button_EXINT/button.h"
#include "GUI/GUI.h"

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
volatile int playing_field[HEIGHT][WIDTH] ;
volatile int score;
volatile int HighScore = 0;
volatile int game_started ;
volatile int game_over ;
volatile int paused;
volatile int lines_cleared = 0;

volatile ActiveTetromino currentPiece;

const uint16_t TETROMINO_COLORS[7] = { 
    Cyan,    // I
    Yellow,  // O
    Magenta, // T
    Blue,    // J   
    Orange,  // L 
    Green,   // S
    Red      // Z
};
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
void DrawCurrentPiece(uint16_t color){
  int r, c;
  for (r = 0; r < 4; r++) {
      for (c = 0; c < 4; c++) {
          // Prendo in considerazione le cordinate del pezzo dove nella matrice corrisponde un blocco (1)
          if (currentPiece.shape[r][c] != 0) {
              int fieldX = currentPiece.x + c;
              int fieldY = currentPiece.y + r;
              
              // mi assicuro di non uscire dai limiti del playing_field
              if (fieldY >= 0 && fieldY < HEIGHT && fieldX >= 0 && fieldX < WIDTH) {
                  GUI_DrawBlock(fieldX, fieldY, color);
              }
          }
      }   
  }
}
void initializeGame() {
    initializePlayingField();
		srand(LPC_RIT->RICOUNTER); // inizializzo il seme del generatore di numeri casuali, modifica il seed ad ogni reset
    score = 0;
    game_started = 0;
    game_over = 0;
    paused = 1;
    init_piece();
}

void initializePlayingField() {
    int r, c;
    for (r = 0; r < HEIGHT; r++) {
        for (c = 0; c < WIDTH; c++) {
            playing_field[r][c] = 0;
        }
    }
}

void init_piece() {
    currentPiece.x = 0;
    currentPiece.y = 0;

    // Resettiamo la matrice cella per cella
		int r,c;
    for ( r = 0; r < 4; r++) {
        for ( c = 0; c < 4; c++) {
            currentPiece.shape[r][c] = 0;
        }
    }
}
int isOverlapping() {
  int r, c;
    for (r = 0; r < 4; r++) {
        for (c = 0; c < 4; c++) {
            if (currentPiece.shape[r][c] != 0) {
            
                int fieldX = currentPiece.x + c;
                int fieldY = currentPiece.y + r;

                if(fieldX < 0 || fieldX >= WIDTH || fieldY < 0 || fieldY >= HEIGHT)
                  return 1;
                // Controlla se c'è un pezzo già presente
                if(fieldY >= 0){

                  if (playing_field[fieldY][fieldX] != 0) {
                    return 1; // si sovrappone ad un pezzo
                }

                }
                
            }
        }
    }
    return 0; // Non si sovrappone a nessun pezzo 
}


//wrapper per SpawnPiece
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
    DrawCurrentPiece(TETROMINO_COLORS[currentPiece.type]);

    if(isOverlapping()){ //eseguo il controllo per il gamover subito
      game_over = 1;
      game_started = 0;
      paused = 1;
      return;
    }
}

void SpawnPiece(int pieceIndex, int initialX, int initialY) {
  // Imposta coordinate iniziali 
  currentPiece.x = initialX; 
  currentPiece.y = initialY; 
  
  //Salviamo il tipo e resettiamo la rotazione
  int rotationIndex = 0;
  currentPiece.type = pieceIndex;
  currentPiece.rotation = rotationIndex; 

  // Copiamo la forma iniziale (Rotazione 0) dalla memoria 
  int r, c;
  for (r = 0; r < 4; r++) {
      for (c = 0; c < 4; c++) {
          currentPiece.shape[r][c] = TETROMINOS[pieceIndex][rotationIndex][r][c];
      }
  }
}
int checkCollisionLeft(){
  int r, c;
  for (r = 0; r < 4; r++) {
      for (c = 0; c < 4; c++) { 
          if (currentPiece.shape[r][c] != 0) {
              int fieldX = currentPiece.x + c - 1;
              int fieldY = currentPiece.y + r;

              if (fieldX < 0 || playing_field[fieldY][fieldX] != 0) {
                  return 0; // Collisione a sinistra
              }
          }
      }
  }
  return 1; // Nessuna collisione a sinistra
}
int checkCollisionRight(){ 
  int r, c;
  if (currentPiece.x < WIDTH-3) {
    for (r = 0; r < 4; r++) {
      for (c = 0; c < 4; c++) {
          if (currentPiece.shape[r][c] != 0) {
              int fieldX = currentPiece.x + c + 1;
              int fieldY = currentPiece.y + r;

              if (fieldX >= WIDTH || playing_field[fieldY][fieldX] != 0) {
                  return 0; // Collisione a destra
              }
          }
      }
   }
  } else if (currentPiece.x >= WIDTH-4){
    return 0; // supero il limite destro
  }

  return 1; // Nessuna collisione a destra
}

int futurePosition(){
  // funzione che calcola la posizione futura del pezzo in caduta
  // e gestisce il blocco del pezzo e la cancellazione delle linee
  // quando il pezzo raggiunge il fondo o un altro pezzo
  if (canMoveDown()) {
      return 1; // può muoversi giù
  } else {
      handlePieceLock();
      SpawnNewPiece();
      return 0; // non può muoversi giù
  }
}

int canMoveDown() {
  int r, c;
  for (r = 0; r < 4; r++) {
      for (c = 0; c < 4; c++) {
          if (currentPiece.shape[r][c] != 0) {
              int fieldX = currentPiece.x + c;
              int fieldY = currentPiece.y + r + 1; // Controlliamo la riga sotto

              // Controlla i limiti del playing_field
              if (fieldY >= HEIGHT) {
                  return 0; // Non può muoversi giù, ha raggiunto il fondo
              }

              // Controlla se c'è un pezzo già presente
              if (playing_field[fieldY][fieldX] != 0) {
                  return 0; // Non può muoversi giù, c'è un pezzo sotto
              }
          }
      }
  }
  return 1; // Può muoversi giù
}
int isPositionValidAfterRotation(int x, int y, int shape[4][4]) {
  int r, c;
  for (r = 0; r < 4; r++) {
      for (c = 0; c < 4; c++) {
          if (shape[r][c] != 0) {
              int fieldX = x + c;
              int fieldY = y + r;

              // Controlla i limiti del playing_field
              if (fieldX < 0 || fieldX >= WIDTH || fieldY < 0 || fieldY >= HEIGHT) {
                  return 0; // Posizione non valida, esce dai limiti
              }

              // Controlla se c'è un pezzo già presente
              if (playing_field[fieldY][fieldX] != 0) {
                  return 0; // Posizione non valida, c'è un pezzo
              }
          }
      }
  }
  return 1; // Posizione valida
}
void copyRotation(int tempShape[4][4], int rotationIndex) {
  currentPiece.rotation = rotationIndex;
  int r, c;
  for(r = 0; r < 4; r++) {
    for(c = 0; c < 4; c++) {
      currentPiece.shape[r][c] = tempShape[r][c];
    }
  }
  return;
}
void rotatePiece() {
  /* La rotazione del pezzo utilizza una nuova matrice per la posizione del pezzo che potrebbe comportare uno sforamento 
     del playing field, per questo motivo è necessaria l'aggiunta di una "spinta"  verso sinistra o verso destra
     del pezzo rotato, per questo motivo è necessario provare se la rotazione è valida o no */
  int tempRotation = (currentPiece.rotation + 1) % 4;
  int r, c;
  // Copia la forma rotata nella matrice temporanea
  int tempShape[4][4];
  for (r = 0; r < 4; r++) {
    for (c = 0; c < 4; c++) {
      tempShape[r][c] = TETROMINOS[currentPiece.type][tempRotation][r][c];
    }
  }

  
  DrawCurrentPiece(BACKGROUND_COLOR);// cancello il pezzo dalla posizione attuale

  if(isPositionValidAfterRotation(currentPiece.x, currentPiece.y, tempShape)) {
    // Aggiorno il current piece
    copyRotation(tempShape, tempRotation);

  }
  else if (isPositionValidAfterRotation(currentPiece.x + 1, currentPiece.y, tempShape))
  { 
    currentPiece.x += 1; // sposto il pezzo a destra
    // Aggiorno il current piece
    copyRotation(tempShape, tempRotation);

  }
  else if (isPositionValidAfterRotation(currentPiece.x - 1, currentPiece.y, tempShape))
  {
    currentPiece.x -= 1; // sposto il pezzo a sinistra
    // Aggiorno il current piece
    copyRotation(tempShape, tempRotation);

  }
  else if (isPositionValidAfterRotation(currentPiece.x, currentPiece.y - 1, tempShape))
  {
    currentPiece.y -= 1; // sposto il pezzo in alto 
    // Aggiorno il current piece
    copyRotation(tempShape, tempRotation);

  }
  // nel caso in cui ho la rotazione del pezzo I potrei incorrere in uno sforamento di due quadrati 
  else if(currentPiece.type == 0 && isPositionValidAfterRotation(currentPiece.x + 2, currentPiece.y, tempShape)){
    currentPiece.x += 2;
    // Aggiorno il current piece
    copyRotation(tempShape, tempRotation);

  }
  else if(currentPiece.type == 0 && isPositionValidAfterRotation(currentPiece.x - 2, currentPiece.y, tempShape)){
    currentPiece.x -= 2;
    // Aggiorno il current piece
    copyRotation(tempShape, tempRotation);

  }

  DrawCurrentPiece(TETROMINO_COLORS[currentPiece.type]);// disegno il pezzo nella nuova posizione
  return;

}

void movePieceLeft() {
  if(checkCollisionLeft()){
    DrawCurrentPiece(BACKGROUND_COLOR);// cancello il pezzo dalla posizione attuale
    currentPiece.x--;
    DrawCurrentPiece(TETROMINO_COLORS[currentPiece.type]); // disegno il pezzo nella nuova posizione
  }
}

void movePieceRight() {
  if(checkCollisionRight()) {
    DrawCurrentPiece(BACKGROUND_COLOR); // cancello il pezzo dalla posizione attuale
    currentPiece.x++;
    DrawCurrentPiece(TETROMINO_COLORS[currentPiece.type]); // disegno il pezzo nella nuova posizione
  }
}
void movePieceDown() {
  if(futurePosition()){
    DrawCurrentPiece(BACKGROUND_COLOR); // cancello il pezzo dalla posizione attuale
    currentPiece.y++;
    int previous_score = score;
    score += 1; // aumenta il punteggio ad ogni discesa del pezzo
    GUI_UpdateScore(previous_score);
    DrawCurrentPiece(TETROMINO_COLORS[currentPiece.type]); // disegno il pezzo nella nuova posizione
  }
  return;
}

void handlePieceLock(void) {
    int previous_score = score;
    // 1. Solidifica il pezzo nella matrice del playing_field
    lockPiece();
    
    // 2. Controlla le linee e ottieni il numero
    int previous_lines_cleared = lines_cleared;
    int linesRemoved = deleteFullLines();

    // 3. LOGICA PUNTEGGIO SPECIALE
    if (linesRemoved > 0) {
      GUI_UpdateClearedLines(previous_lines_cleared);
      GUI_RefreshScreen();

        // Caso "TETRIS": 4 Linee cancellate con il pezzo I
        if (linesRemoved == 4) {
            int previous_score = score;
            
            // A. Assegna un punteggio bonus enorme
            score += 600; // Bonus extra per il TETRIS
            
        } else {
            // Punteggio normale per 1, 2 o 3 linee
            // Esempio classico Nintendo: 40, 100, 300 punti
            switch(linesRemoved) {
                
                case 1: score += 100; break;
                case 2: score += 200; break;
                case 3: score += 300; break;

            }

        }
    }
    GUI_UpdateScore(previous_score);
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
    int previous_score = score;
    score += 10; // aumenta il punteggio quando un pezzo viene bloccato
}

int deleteFullLines(void) {
  int y, x;
  int linesCleared = 0;
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
  lines_cleared = lines_cleared + linesCleared; // Aggiorna la variabile globale
  return linesCleared; // Restituisce 0, 1, 2, 3 o 4
}