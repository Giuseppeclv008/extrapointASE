#include "mechanics.h"
#include <stdio.h>
#include "LPC17xx.h"                    /* LPC17xx definitions                */
#include "led/led.h"
#include "button_EXINT/button.h"
#include "GUI/GUI.h"
#include "adc/adc.h"
#include "timer/timer.h"

#define HEIGHT 20
#define WIDTH 10


// variabili globali
volatile uint16_t playing_field[HEIGHT][WIDTH] ;
volatile uint16_t highest_row = HEIGHT;
volatile uint16_t pending_powerups[10];
volatile uint16_t powerUpFlag = 0;

volatile uint16_t powerupsInTheField = 0; // da rimuovere 
volatile uint64_t HighScore = 0;
volatile uint64_t score = 0;
volatile uint8_t game_started ;
volatile uint8_t game_over ;
volatile uint8_t paused; // the playing state is represented by !paused 
volatile uint16_t lines_cleared = 0;
volatile uint8_t hardDrop_flag;
volatile ActiveTetromino currentPiece;

// variabili globali per il conteggio dello slowdown, 15 secondi = 300 Ticks, perchè RIT scatta ogni 50ms
volatile int slowDownTicks = 0; 
extern volatile uint64_t current_period;

const uint16_t TETROMINO_COLORS[7] = { 
    Cyan,    // I
    Yellow,  // O
    Magenta, // T
    Blue,    // J   
    Orange,  // L 
    Green,   // S
    Red      // Z
};

uint16_t POWERUP_COLORS[2] = { 
  White, // ClearHalfLines
  Grey // SlowDown
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
void inizializePendingPowerups(void){
  int i;
  for(i = 0; i < WIDTH; i++){
    pending_powerups[i] = 0;
  }
}
void initializeGame(void) {
    initializePlayingField();
		inizializePendingPowerups();
    score = 0;
    game_started = 0;
    game_over = 0;
    lines_cleared = 0;
    paused = 1;
    init_piece();
}

void initializePlayingField(void) {
    int r, c;
    for (r = 0; r < HEIGHT; r++) {
        for (c = 0; c < WIDTH; c++) {
            playing_field[r][c] = 0;
        }
    }
}

void init_piece(void) {
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
uint8_t isOverlapping(void) {
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
void SpawnNewPiece(void){
    // wrapper per la generazione di un nuovo pezzo che si occupa 
    // di generare un numero casuale fra 0 e 6 per la scelta del pezzo 
    // ed un numero casuale fra 0 e 4 per la scelta casuale della rotazione 
    int pieceIndex = rand() % 7;
    // la posizione iniziale è di sicuro in alto X = 0 ma puntando le nostre coordinate 
    // all'angolo sinistro in alto della matrice rappresentate un pezzo 
    // la posizione limite che previene uno sforamento dell'playing_field è (0, 6)
    int initialX = rand() % 7;
    SpawnPiece(pieceIndex, initialX, 0);
    GUI_DrawCurrentPiece(TETROMINO_COLORS[currentPiece.type]);

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
uint8_t checkCollisionLeft(void){
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
uint8_t checkCollisionRight(void){ 
  int r, c;
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
  return 1; // Nessuna collisione a destra
}



uint8_t canMoveDown(void) {
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
              if (fieldY >= 0 && playing_field[fieldY][fieldX] != 0) {
                  return 0; // Non può muoversi giù, c'è un pezzo sotto
              }
          }
      }
  }
  return 1; // Può muoversi giù
}
//verifica se il pezzo può muoversi verso il basso o no 
uint8_t tryMoveDown(void){
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
uint8_t isPositionValidAfterRotation(int x, int y, int shape[4][4]) {
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
void rotatePiece(void) {
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
  GUI_DrawCurrentPiece(BACKGROUND_COLOR);// cancello il pezzo dalla posizione attuale
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

  GUI_DrawCurrentPiece(TETROMINO_COLORS[currentPiece.type]);// disegno il pezzo nella nuova posizione
  return;

}

void movePieceLeft(void) {
  if(checkCollisionLeft()){
    GUI_DrawCurrentPiece(BACKGROUND_COLOR);// cancello il pezzo dalla posizione attuale
    currentPiece.x--;
    GUI_DrawCurrentPiece(TETROMINO_COLORS[currentPiece.type]); // disegno il pezzo nella nuova posizione
  }
}

void movePieceRight(void) {
  if(checkCollisionRight()) {
    GUI_DrawCurrentPiece(BACKGROUND_COLOR); // cancello il pezzo dalla posizione attuale
    currentPiece.x++;
    GUI_DrawCurrentPiece(TETROMINO_COLORS[currentPiece.type]); // disegno il pezzo nella nuova posizione
  }
}
void movePieceDown(void) {
  if(tryMoveDown()){
    GUI_DrawCurrentPiece(BACKGROUND_COLOR); // cancello il pezzo dalla posizione attuale
    currentPiece.y++;
    uint64_t previous_score = score;
    score += 1; // aumenta il punteggio ad ogni discesa del pezzo
    GUI_UpdateScore(previous_score);
    GUI_DrawCurrentPiece(TETROMINO_COLORS[currentPiece.type]); // disegno il pezzo nella nuova posizione
  }
  return;
}
void hardDrop(void){
  hardDrop_flag = 1;
  GUI_DrawCurrentPiece(BACKGROUND_COLOR);
  while(tryMoveDown()){
    currentPiece.y++;
  }
  hardDrop_flag = 0;
  return;

}

void lockPiece(void) {
  // Blocca il pezzo corrente nell'playing_field
  int r,c;

  for (r = 0; r < 4; r++) {
      for ( c = 0; c < 4; c++) {
          if (currentPiece.shape[r][c] != 0) {
              int fieldX = currentPiece.x + c;
              int fieldY = currentPiece.y + r;
              if (fieldY >= 0 && fieldY < HEIGHT && fieldX >= 0 && fieldX < WIDTH) {
                  if(currentPiece.shape[r][c] == 1){
                    if(fieldY < highest_row) highest_row = fieldY; // quando viene inserito un quadrato in alto me ne salvo la coordinata 
                                                                   // per poter calcolare a quanto ammontano le linee da cancellare in clearHalfLines
                                                                   // uso il segno < perchè più sono in alto più fiedlY è piccola
                    if(fieldY < highest_row) highest_row = fieldY; // quando viene inserito un quadrato in alto me ne salvo la coordinata 
                                                                   // per poter calcolare a quanto ammontano le linee da cancellare in clearHalfLines
                                                                   // uso il segno < perchè più sono in alto più fiedlY è piccola
                    playing_field[fieldY][fieldX] = currentPiece.type + 1 ; 
                    // aumento di 1 per evitare confusione fra gli spazi vuoti e gli spazi pieni 
                    // fondamentali per il check delle linee piene 
                  }
                  else{
                    playing_field[fieldY][fieldX] = currentPiece.shape[r][c];
                  }
                  
              }
          }
      }
  }
  score += 10; // aumenta il punteggio quando un pezzo viene bloccato
}

void assignScore(uint16_t linesRemoved, uint16_t  previous_lines_cleared){

  if (linesRemoved > 0) {
    uint32_t previous_score = score;
    uint16_t linesToAssignPoints = linesRemoved;

    GUI_UpdateClearedLines(previous_lines_cleared);
    GUI_RefreshScreen();
    while(linesToAssignPoints >= 4){
      // A. Assegna un punteggio bonus
      score += 600; // Bonus extra 
      linesToAssignPoints = linesToAssignPoints - 4;
    }
 
    // Punteggio normale per 1, 2 o 3 linee
    switch(linesToAssignPoints) {
        
        case 1: score += 100; break;
        case 2: score += 200; break;
        case 3: score += 300; break;

    }
      GUI_UpdateScore(previous_score);
  }

}


/* *************** */
/*    POWERUPS    */
/* *************** */



void addPendingPowerup(POWERUP type){
  int i;
  for(i = 0; i < WIDTH; i++){
    if(pending_powerups[i] == 0){
      pending_powerups[i] = type;
      return;
    }
  }
}

void slowDown(void){
  if(current_period < NORMAL_PERIOD){
    LPC_TIM0->MR0 = NORMAL_PERIOD;
    LPC_TIM0->TC = 0;

    slowDownTicks = 300;

    GUI_Text(10, 300, (uint8_t*)"SlowDown ON", White, Red);
  }
  


}


void clearHalfLines(void){
  int i, r, c, x, j;
  uint16_t lines_occupied = HEIGHT - highest_row;
  uint16_t linesRemoved = 0;
  uint16_t lines_to_clear; 
  
  if(lines_occupied == 0) return;

  lines_to_clear = lines_occupied / 2;
  if(lines_to_clear == 0 && lines_occupied > 0) lines_to_clear = 1;
  
  for(i = lines_to_clear; i > 0; i--){

    for( x = 0; x < WIDTH; x++){
      if(playing_field[HEIGHT-1][x] == SLOW_DOWN || playing_field[HEIGHT-1][x] == CLEAR_H_LINES){ //attivazione del powerup quando cancello una riga che lo contiene
        
       addPendingPowerup(playing_field[HEIGHT-1][x]);
       if(powerupsInTheField > 0) powerupsInTheField--;
      }

      }
    
    // Shift down logic adapted from deleteFullLines
      // We target the bottom row (HEIGHT - 1) to shift everything down
      for (r = HEIGHT - 1; r > 0; r--) {
          for (c = 0; c < WIDTH; c++) {
              playing_field[r][c] = playing_field[r-1][c];
          }
      }
      // Clear the new top row
      for (c = 0; c < WIDTH; c++) {
          playing_field[0][c] = 0;
      }
      linesRemoved++;
  }
  uint16_t previous_lines_cleared = lines_cleared;
  lines_cleared += linesRemoved;
  highest_row += lines_to_clear;
  if(highest_row > HEIGHT) highest_row = HEIGHT;

  assignScore(linesRemoved, previous_lines_cleared);
  GUI_RefreshScreen();
}


void spawnPowerUp(void){
    uint16_t powerUpTypes[2] = {CLEAR_H_LINES, SLOW_DOWN};
    uint16_t occupied_lines = HEIGHT - highest_row;
    uint16_t powerUpType = powerUpTypes[rand() % 2]; 

    uint16_t randomY = (rand() % occupied_lines) + highest_row ; // la somma con highest_row mi fornisce l'oofset adatto 
                                                                    // più il valore è alto più sono in basso
    uint16_t randomX = rand() % WIDTH;
    uint32_t attempts ; // imposto un limite di tentativi per l'inserimento di un powerup, evito loop infiniti 
    for( attempts =  100; attempts > 0; attempts--){

      if (playing_field[randomY][randomX] != 0) {
        playing_field[randomY][randomX] = SLOW_DOWN;  // se trovo un blocco diverso da 0 lo sostituisco con un powerup ed esco dal loop  
        GUI_DrawBlock(randomX, randomY, POWERUP_COLORS[SLOW_DOWN-12]);
        break;
      }
      randomY =(rand() % occupied_lines) + highest_row;
      randomX = rand() % WIDTH;
    }
}




void activePowerUp(POWERUP type){
    if(type == CLEAR_H_LINES){

      clearHalfLines();

    }
    else if(type == SLOW_DOWN){
      slowDown();
    }
}
  

void powerUpCheck(void){
  // se rilevo un powerup quando elimino le righe lo attivo qui 
  // inserito qui perchè matrice del playing field stabile 
  int i;
  while(pending_powerups[0] != 0){
    POWERUP typeToExecute = pending_powerups[0];

    //shift della coda dei powerups
    for(i = 0; i < WIDTH-1; i++){
      pending_powerups[i] = pending_powerups[i+1];
    }
    pending_powerups[WIDTH-1] = 0; //pulisco l'ultimo elemento 
    powerUpFlag = 1;
    activePowerUp(typeToExecute);
    powerUpFlag = 0;
  }
  
}


/* END POWERUP SECTION */



uint16_t deleteFullLines(void) {
int y, x, i;
uint8_t linesCleared = 0;
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
        for (x = 0; x < WIDTH; x++) {
          if(playing_field[y][x] == SLOW_DOWN || playing_field[y][x] == CLEAR_H_LINES){ //attivazione del powerup quando cancello una riga che lo contiene
            
            addPendingPowerup(playing_field[y][x]);
            if(powerupsInTheField > 0) powerupsInTheField--;
          }

            }
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
        // ricordiamo che più è grande y più in basso ci troviamo nel playing field 
        y++; 
    }
}
highest_row += linesCleared; // Aggiorna la variabile globale 
lines_cleared = lines_cleared + linesCleared; // Aggiorna la variabile globale
return linesCleared; // Restituisce 0, 1, 2, 3 o 4
}


static uint16_t lines_to_next_powerup = 0;
void handlePieceLock(void) {
 
  
    if(hardDrop_flag == 1) GUI_DrawCurrentPiece(TETROMINO_COLORS[currentPiece.type]);
    // 1. Solidifica il pezzo nella matrice del playing_field
    lockPiece();
    
    // 2. Controlla le linee e ottieni il numero
    uint16_t previous_lines_cleared = lines_cleared;
    uint16_t linesRemoved = deleteFullLines();
    // Quando puliamo delle linee e il numero di linee pulite raggiunge un multiplo di 5 
    // faccio comparire un PowerUp 
    if(linesRemoved > 0){
      lines_to_next_powerup += linesRemoved;
      
      /*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
      /*  TO DO MODIFICARE LA CONDIZIONE DELL'IF QUI SOTTO CORRETTAMENTE COME lines_to_next_powerup >= 5 */
      /*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
      
      if(lines_to_next_powerup >= 1){ // in questo modo gestisco i casi in cui cleared_lines non sia precisamente multiplo di 5 
        spawnPowerUp();
        powerupsInTheField ++;
        lines_to_next_powerup = lines_to_next_powerup - 5;
    }
    assignScore(linesRemoved, previous_lines_cleared);

    }
    
    powerUpCheck(); 
}

