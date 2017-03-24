/*  TP3 - Partie 2
 *  Réalisé par Mathieu Bélanger Et William Harvey
 *  31/01/2017
 */

/*   _________________________________________________________
 *  |Etat initial|Btn|Compteur|Etat suivant|DEL libre|Compteur|             
 *  |------------|---|--------|------------|---------|--------|
 *  |     INIT   | 1 |  != 5  |    INIT    |  Off    |   ++   |
 *  |     INIT   | 0 |   5    |    ROUGE   |  Off    |   X    |
 *  |     ROUGE  | X |   X    |    INIT    |  Rouge  |   0    |
 *  |____________|___|________|____________|_________|________|
*/

#define F_CPU 8000000
#include <util/delay.h>
#include <avr/io.h> 


int main()
{
  DDRA = 0xff;// PORT A est en mode sortie
  DDRD= 0x00; // PORT D est en mode entree
  
  enum state {INIT, ROUGE};
  int etat = INIT;
  int compteur = 0;
  _Bool estEnclenche = 0;
  
while(1) {
	switch (etat) { 
		case INIT : 
			while(compteur != 5) {
                
				PORTA = 0x00;
				if(PIND & 0x04 && !estEnclenche) {
                    _delay_ms(10);
                    if(PIND & 0x04) {
                        compteur++;
                        estEnclenche = 1;
                    }
				}
				
				while(estEnclenche) {
                    if(!(PIND & 0x04))
                        estEnclenche = 0;
                }
			}
			etat = ROUGE;
            
		case ROUGE :
            
			PORTA = 0x01;
			_delay_ms(1000);
            compteur = 0;
            etat = INIT;

	}
}		

return 0;
}
