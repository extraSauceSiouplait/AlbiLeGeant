/*  TP3 - INF1995
 *  Réalisé par Mathieu Bélanger et William Harvey
 *  31/01/2017
 */

/*   _______________________________________
 *  |Etat initial|Btn|Etat suivant|DEL libre|             
 *  |------------|---|------------|---------|
 *  |     INIT   | 1 |     E1     |  Rouge  |
 *  |      E1    | 1 |     E1     |  Ambre  |
 *  |      E1    | 0 |     E2     |  Ambre  |
 *  |      E2    | 1 |     E3     |  Vert   |
 *  |      E3    | 0 |     E4     |  Rouge  |
 *  |      E4    | 1 |     E5     |  Off    |
 *  |      E5    | 0 |     INIT   |  Vert   |
 *  |____________|___|____________|_________|
 * 
 * 
*/

#define F_CPU 8000000
#include <util/delay.h>
#include <avr/io.h> 


int main()
{
  DDRA = 0xff;// PORT A est en mode sortie
  DDRD= 0x00; // PORT D est en mode entree
  
  enum state {INIT, E1, E2, E3, E4, E5};
  int etat = INIT;
  
  while(true) {
      
      switch (etat) {
          
          case INIT:
              PORTA = 0x01;
              if(PIND & 0x04) {
                  _delay_ms(10);
                  if(PIND & 0x04)
                      etat = E1;
              }
              break;
              
          case E1:
              while(PIND & 0x04) {
                  PORTA = 0x01;
                  _delay_ms(5);
                  PORTA = 0x02;
                  _delay_ms(15);
            }
            etat = E2;
            break;
            
          case E2:  
            PORTA = 0x02;
              if(PIND & 0x04) {
                  _delay_ms(10);
                  if(PIND & 0x04)
                      etat = E3;
            }
            break;
            
          case E3:
            while(PIND & 0x04) {
                 PORTA = 0x01;
            }
            etat = E4;
            break;
            
          case E4:
            PORTA = 0x00;
            if(!(PIND & 0x04)) {
                  _delay_ms(10);
                  if(PIND & 0x04)
                      etat = E5;
            }
            break;
          case E5:
              while(PIND & 0x04) {
                  PORTA = 0x02;
            }
            etat = INIT;
            break;
    }

 }
return 0;
} 
