#include "GUI/GUI.h"
#include "mechanics/mechanics.h"
#include <stdio.h>
/*********************************************************************************************************
** File:        GUI.c
** Descriptions:    Funzioni per la gestione della GUI del gioco
*********************************************************************************************************/
extern volatile int HighScore;
extern volatile int score;
extern volatile int lines_cleared;
extern const uint16_t TETROMINO_COLORS[7];
void GUI_DrawInterface(void){
    //Disegna il bordo del playing field e la sezione con il punteggio 
    LCD_Clear(BACKGROUND_COLOR);

    // Disegna il bordo del playing field

    // calcolo coordinate dei limiti del playing field 
    uint16_t field_x_start = FIELD_X;
    uint16_t field_y_start = FIELD_Y;
    uint16_t field_x_end = FIELD_X + (FIELD_COLS * BLOCK_SIZE);
    uint16_t field_y_end = FIELD_Y + (FIELD_ROWS * BLOCK_SIZE);

    // disegno le linee che delimitano il playing field
    // LCD_Drawline è definita in GLCD.h
    LCD_DrawLine(field_x_start - 1, field_y_end + 1, field_x_end + 1, field_y_end + 1, FIELD_BORDER_COLOR);     // linea inferiore
    LCD_DrawLine(field_x_start - 1, field_y_start - 1, field_x_start - 1, field_y_end + 1, FIELD_BORDER_COLOR); // linea sinistra
    LCD_DrawLine(field_x_end + 1, field_y_start - 1, field_x_end + 1, field_y_end + 1, FIELD_BORDER_COLOR);     // linea destra

    // Disegna la sezione del punteggio
    
    // Etichetta Punteggio
    GUI_Text(SCORE_X, SCORE_Y, (uint8_t*)"SCORE", SCORE_COLOR, BACKGROUND_COLOR );
    // Valore Punteggio
    GUI_Text(SCORE_X, SCORE_Y + 20, (uint8_t*)"000000", NUMBER_COLOR, BACKGROUND_COLOR) ; //punteggio iniziale 0
    
    char highScoreStr[7];
    sprintf(highScoreStr, "%06d", HighScore);
    // Etichetta High Score
    GUI_Text(SCORE_X, HIGH_SCORE_Y, (uint8_t*)"HI-SCORE", SCORE_COLOR, BACKGROUND_COLOR);
    // Valore High Score
    GUI_Text(SCORE_X, HIGH_SCORE_Y + 20, (uint8_t*)highScoreStr, NUMBER_COLOR, BACKGROUND_COLOR);
    
     // fare in modo di aggiornare l'high score durante il gioco
    
    // Etichetta Linee Cancellate
    GUI_Text(SCORE_X, CLEARED_LINES_Y, (uint8_t*)"LINES", SCORE_COLOR, BACKGROUND_COLOR);
    // Valore Linee Cancellate
    GUI_Text(SCORE_X, CLEARED_LINES_Y + 20, (uint8_t*)"000", NUMBER_COLOR, BACKGROUND_COLOR);

    //opzionalmente aggiungere la sezione per il next piece
}
void GUI_UpdateScore(int previous_score){
    uint32_t sccore_to_erase = previous_score;
    uint8_t score_str_erase[7]; // 6 cifre + terminatore null
    uint32_t score_to_display = score;
    uint8_t score_str[7]; // 6 cifre + terminatore null
    sprintf((char*)score_str, "%06u", score_to_display);
    sprintf((char*)score_str_erase,"%06u", sccore_to_erase);
    GUI_Text(SCORE_X, SCORE_Y + 20,(uint8_t*) score_str_erase, BACKGROUND_COLOR, BACKGROUND_COLOR); // cancello il punteggio precedente
    // Aggiorna il punteggio visualizzato
    GUI_Text(SCORE_X, SCORE_Y + 20, (uint8_t*)score_str, NUMBER_COLOR, BACKGROUND_COLOR);

}

void GUI_UpdateHighScore(int previous_highscore){
    // Aggiorna l'high score visualizzato
    uint32_t highscore_to_display = HighScore;
    uint8_t highscore_str_erase[7]; // 6 cifre + terminatore null
    uint8_t highscore_str[7]; // 6 cifre + terminatore null
    sprintf((char*)highscore_str,"%06u", highscore_to_display);
    sprintf((char*)highscore_str_erase,"%06u", HighScore); // sottraggo 1 per essere sicuro di cancellare il valore precedente
    GUI_Text(SCORE_X, HIGH_SCORE_Y + 20,(uint8_t*) highscore_str_erase, BACKGROUND_COLOR, BACKGROUND_COLOR); // cancello l'high score
   
    GUI_Text(SCORE_X, HIGH_SCORE_Y + 20,(uint8_t*) highscore_str, NUMBER_COLOR, BACKGROUND_COLOR);
}

void GUI_UpdateClearedLines(int previous_lines_cleared){
    // Aggiorna il numero di linee cancellate visualizzato
    uint32_t lines_to_display = lines_cleared;
    uint8_t lines_str_erase[4]; // 3 cifre + terminatore nulle
    uint8_t lines_str[4]; // 3 cifre + terminatore nulle
    sprintf((char*)lines_str_erase,"%03u", previous_lines_cleared);
    sprintf((char*)lines_str,"%03u", lines_to_display);
    GUI_Text(SCORE_X, CLEARED_LINES_Y + 20,(uint8_t*) lines_str_erase, BACKGROUND_COLOR, BACKGROUND_COLOR); // cancello il numero precedente
    GUI_Text(SCORE_X, CLEARED_LINES_Y + 20,(uint8_t*) lines_str, NUMBER_COLOR, BACKGROUND_COLOR);
}

void GUI_RefreshInterface(){
    LCD_Clear(BACKGROUND_COLOR);
    GUI_DrawInterface();
    // Aggiorna il high score
    GUI_UpdateHighScore(HighScore);
    // Aggiorna il punteggio
    GUI_UpdateScore(score);
    // Aggiorna il numero di linee cancellate
    GUI_UpdateClearedLines(lines_cleared);

}
void GUI_RefreshScreen(){
    int r, c;
    for (r = 0; r < HEIGHT; r++) {
        for (c = 0; c < WIDTH; c++) {
            if (playing_field[r][c] != 0) {
                GUI_DrawBlock(c, r, TETROMINO_COLORS[playing_field[r][c]]);
            }
            else{
                GUI_DrawBlock(c, r, BACKGROUND_COLOR);
            }
        }
    }   

}

void GUI_pauseScreen(void){
    // Disegna la schermata di pausa
    GUI_Text(5, 150, (uint8_t*)"PAUSED-PRESS KEY1 TO CONTINUE", TEXT_COLOR, BACKGROUND_COLOR);

}
void GUI_resumeScreen(void){
	GUI_RefreshInterface();
}

void GUI_gameOverScreen(void){
    // Disegna la schermata di game over
    GUI_Text(5, 150, (uint8_t*)"GAME OVER-PRESS KEY1 TO PLAY AGAIN", TEXT_COLOR, BACKGROUND_COLOR);
}

void GUI_clearGameOverScreen(void){
    // Cancella la schermata di game over
    GUI_RefreshInterface();
}

void GUI_DrawBlock(uint16_t x, uint16_t y, uint16_t color){
    // Disegna un blocco del pezzo alla posizione (x,y) con il colore specificato
    int i, j;
    int x_start = FIELD_X + (x * BLOCK_SIZE);
    int y_start = FIELD_Y + (y * BLOCK_SIZE);

    for(i = 0; i < BLOCK_SIZE; i++){
        for(j = 0; j < BLOCK_SIZE; j++){
            if(i == 0 || i == BLOCK_SIZE - 1 || j == 0 || j == BLOCK_SIZE - 1){
                // Disegna il bordo del blocco
                if(color == BACKGROUND_COLOR){
                    // Se il colore è lo sfondo, disegna solo l'interno
                    LCD_SetPoint(x_start + j, y_start + i, color);
                } else {
                LCD_SetPoint(x_start + j, y_start + i, BLOCK_BORDER_COLOR);
                }
            } else {
                // Disegna l'interno del blocco
                LCD_SetPoint(x_start + j, y_start + i, color);
            }
        }
    }
}
void GUI_DrawCurrentPiece(uint16_t color){
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
