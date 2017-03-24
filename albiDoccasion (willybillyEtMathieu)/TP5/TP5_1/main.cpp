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
#include <avr/interrupt.h>

volatile uint8_t etat = 0;
// placer le bon type de signal d'interruption
// a prendre en charge en argument

ISR (INT0_vect) {
    uint8_t etatCourant = PIND & 0x04;
    _delay_ms(30);
    if (PIND == etatCourant){
        etat++;
    }
    else{
        
        
    }
    // changements d'etats tels que ceux de la
    // semaine precedente
    // Voir la note plus bas pour comprendre cette instruction et son role
    EIFR |= (1 << INTF0) ;
}

void initialisation ( void ) {
    // cli est une routine qui bloque toutes les interruptions.
    // Il serait bien mauvais d'etre interrompu alors que
    // le microcontroleur n'est pas pret...
    cli();
    
    // configurer et choisir les ports pour les entrees
    // et les sorties. DDRx... Initialisez bien vos variables
    DDRA = 0xff;
    DDRD = 0x00;
    // cette procédure ajuste le registre EIMSK
    // de ATmega324PA pour permettre les interruptions externes
    EIMSK |= _BV(INT0);
    // il faut sensibiliser les interruptions externes aux
    // changements de niveau du bouton-poussoir
    // en ajustant le registre EICRA
    EICRA |= (1<<ISC00);
    // sei permet de recevoir a nouveau des interruptions.
    sei();
}


int main()
{
  initialisation();

  while(true) {
      
      switch (etat) {
          
          case 0:                
              PORTA = 0x01;
              break;
              
          case 1:
              PORTA = 0x01;         //Interruption pour changer d'état (falling edge asynchrone du bouton poussoir) --> Désappuyer
              _delay_ms(5);
              PORTA = 0x02;
              _delay_ms(15);
              break;
            
          case 2:  
            PORTA = 0x02; //Autre interruption pour changer d'état (rising edge)
            break;
            
          case 3:
            //Interruption pour changer d'état (falling edge)
            PORTA = 0x01;
            break;
            
          case 4:
            PORTA = 0x00;
            break;
            
          case 5:
            //Interruption falling edge
            PORTA = 0x02;
            break;
          
          case 6:
            etat = 0;
            break;
    }

 }
return 0;
} 
