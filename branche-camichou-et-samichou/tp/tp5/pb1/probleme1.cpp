/*
 * Titre:          TP5 - probleme1.cpp
 * Auteurs :       Camille Bergeron-Miron et Samuel Meilleur 
 *  
 *                              TABLEAU D'ETATS 
 *       ___________________________________________________________________
 *      |  etat present  |   bouton    |     etat suivant     |    sortie   |
 *      |    INITIAL     |      0      |        INITIAL       |  1 (rouge)  |
 *      |    INITIAL     |      1      |         AMBRE        |  1 (rouge)  | 
 *      |     AMBRE      |      0      |         VERT1        | 1-2 (ambre) |
 *      |     AMBRE      |      1      |         AMBRE        | 1-2 (ambre) | 
 *      |     VERT1      |      0      |         VERT1        |  2 (vert)   |
 *      |     VERT1      |      1      |         ROUGE        |  2 (vert)   |
 *      |     ROUGE      |      0      |         ETEINT       |  1 (rouge)  |
 *      |     ROUGE      |      1      |         ROUGE        |  1 (rouge)  | 
 *      |     ETEINT     |      0      |         ETEINT       |      0      |
 *      |     ETEINT     |      1      |         VERT2        |      0      |
 *      |     VERT2      |      0      |        INITIAL       |   2 (vert)  |
 *      |     VERT2      |      1      |         VERT2        |   2 (vert)  |
 *      |________________|_____________|______________________|_____________|
 * 
 */

#define F_CPU 8000000
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>

void initialisation();
ISR(INT0_vect);

 enum ETATS{
        INITIAL = 0, AMBRE = 1, VERT1 = 2, ROUGE = 3, ETEINT = 4, VERT2 = 5  
    };
    
volatile bool EST_ENFONCER;
volatile bool temp;
volatile uint8_t etatPresent = INITIAL;


int main()
{
    initialisation();
  
    for(;;){
        switch(etatPresent){
            case INITIAL:{
                PORTA = 0x01;       //0x01 = rouge
            } break;
            case AMBRE: {
                PORTA= 0x01;
                _delay_ms(3);       //Delai plus court pour le rouge que le vert afin que la LED paraisse plus ambre (presque rouge sinon).
                PORTA = 0x02;       //0x02 = vert
                _delay_ms(10);
            } break;
            case VERT1: {
                PORTA = 0x02;
            } break;
            case ROUGE: {
                    PORTA = 0x01;
            } break;
            case ETEINT: {
                PORTA = 0x00;
            } break;
            case VERT2: {
                PORTA = 0x02;
            } break;
        } 
    }
        
}

void initialisationINT0(bool modeBit0, bool modeBit1){
    cli();          //Interuptions désactivées
    DDRA = 0xff;    //PORT A en mode sortie
    DDRD = 0x00;    //PORT D en mode entree
    EIMSK |= _BV(INT0);
    EICRA |= (modeBit1 << ISC01) | (modeBit0 << ISC00) ;  //1 sur le bit 0 et 0 sur le bit 1 du registre EICRA (interrupt with rising and falling edge)
    
    sei();          //Interruptions réactivées
}   

ISR(INT0_vect){
    EST_ENFONCER = !EST_ENFONCER;  //Si une interuption est détécter, l'état du bouton a nécessairement changée.
    temp = EST_ENFONCER;
    _delay_ms(30);
    
   if (EST_ENFONCER == temp){     //Si l'état du bouton détecté par le microcontrolleur est pareil après 30ms, le contact est bien enclanché.  
        
        switch(etatPresent){
            case INITIAL:{
                etatPresent = AMBRE;        
            } break;
            case AMBRE: {
                etatPresent = VERT1;        
            } break;
            case VERT1: {
                etatPresent = ROUGE;
            } break;
            case ROUGE: {
                etatPresent = ETEINT;
            } break;
            case ETEINT: {
                etatPresent = VERT2;
            } break;
            case VERT2: {
                etatPresent = INITIAL;
            } break;
            
        }
    }
    //*/
   
    EIFR |= (1 << INTF0) ;
}

