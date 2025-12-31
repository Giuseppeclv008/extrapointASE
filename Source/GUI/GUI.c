#include "GUI/GUI.h"

/*********************************************************************************************************
** File:        GUI.c
** Descriptions:    Funzioni per la gestione della GUI del gioco
*********************************************************************************************************/
extern volatile int HighScore;

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
    GUI_Text(SCORE_X, SCORE_Y - 20, "SCORE", TEXT_COLOR, BACKGROUND_COLOR );
    // Valore Punteggio
    GUI_Text(SCORE_X, SCORE_Y + 10, "000000", NUMBER_COLOR, BACKGROUND_COLOR) ; //punteggio iniziale 0
    
    
    // Etichetta High Score
    GUI_Text(SCORE_X, HIGH_SCORE_Y - 20, "HIGH SCORE", TEXT_COLOR, BACKGROUND_COLOR);
    // Valore High Score
    GUI_Text(SCORE_X, HIGH_SCORE_Y + 10, "000000", NUMBER_COLOR, BACKGROUND_COLOR);
    
     // fare in modo di aggiornare l'high score durante il gioco
    

    //opzionalmente aggiungere la sezione per il next piece
}