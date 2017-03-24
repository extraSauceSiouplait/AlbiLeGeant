/*
 * Titre:          TP7 - probleme1.cpp
 * Auteurs :       Camille Bergeron-Miron et Samuel Meilleur 
 *  
 *                              TABLEAU D'ETATS 
 *       ____________________________________________________________________________________
 *      |  etat present  |   bouton    |     etat suivant       |           sortie           |
 *      |    INITIAL     |      0      |        INITIAL         |              0             |
 *      |    INITIAL     |      1      |       COMPTEUR         |              0             | 
 *      |   COMPTEUR     |      0      |       CLIGNOTE         |              0             |
 *      |   COMPTEUR     |      1      |       COMPTEUR         |              0             | 
 *      |                |             |                        |                            |
 *      |                |             |INITIAL(apres compteur/2|          vert(0.5s)        |   
 *      |   CLIGNOTE     |      X      | clignotement + 1s)     |rouge(compteur/2 fois @ 2Hz)|
 *      |                |             |                        |           vert(1s)         |
 *      |________________|_____________|________________________|____________________________|
 * 
 */

//Borne + de la LED @ B0
//Borne - de la LED @ B1

#define F_CPU 8000000

#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>

#define ROUGE 0x01
#define VERT 0x02

void initialisation();

ISR(INT0_vect);
ISR(TIMER1_COMPA_vect);

enum Etats {
  INITIAL, COMPTEUR, CLIGNOTE  
};

volatile bool estEnfoncer = false;
volatile bool temp = false;
volatile uint8_t etatPresent = INITIAL;
volatile uint8_t compteur = 0;
 


int main()
{   
    initialisation();
    DDRB = 0xff;    //mode écriture pour allumer la LED.
    DDRD = 0x00;    //mode lecture pour detecter interruption.
    
    for(;;){
        switch(etatPresent){
            case INITIAL:{} break;  //fait rien.
            
            case COMPTEUR:{
                 compteur++;
                 _delay_ms(100);    //compteur incrémente @10Hz
               
                if(compteur >= 120)
                    etatPresent = CLIGNOTE; //clignote si comtpeur = 120.
            } break;
            
            case CLIGNOTE:{
                PORTB = VERT;
                _delay_ms(500);     //clignote vert pour 0.5 secondes.
                PORTB = 0;
                _delay_ms(2000);    //attend 2 secondes.
                
                for(int i = 0; i < compteur; i += 2){
                    PORTB = ROUGE;     
                    _delay_ms(500/2);   //clignote rouge @2Hz (compteur/2) fois
                    PORTB = 0;
                    _delay_ms(500/2);
                }
                
                PORTB = VERT;       //vert pour 1 seconde.
                _delay_ms(1000);
                PORTB = 0;
                etatPresent = INITIAL;
            } break;
            
        }
        
    }
        
}
    

void initialisation(){
    cli();           //Interuptions désactivées
    EIMSK |= _BV(INT0);  //external interrupt INT0 enable.
    EICRA |= (1 << ISC00) | (0 << ISC01);  //interrupt on rising edge and falling edge.
    
    sei();          //Interruptions réactivées
}   

ISR(INT0_vect){
    estEnfoncer = !estEnfoncer; //l'interruption implique que le bouton à changé d'état.

    temp = estEnfoncer;          
    _delay_ms(30);               //compare estEnfoncer à son état il y a 30ms
    if(temp == estEnfoncer) {    //pour assurer un état stable (antirebond).
        
        switch(etatPresent){
            case INITIAL:{
                if (estEnfoncer)
                    etatPresent = COMPTEUR;
            } break;
            
            case COMPTEUR:{
                 if(!estEnfoncer)
                    etatPresent = CLIGNOTE;
            } break;
            
            case CLIGNOTE:{} break;   //clignote tant qu'on ne retourne pas a l'etat initial.
        }
    
    }
}


