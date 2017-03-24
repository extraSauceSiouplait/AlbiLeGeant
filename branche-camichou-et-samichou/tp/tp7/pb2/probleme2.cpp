/*
 * Titre:          TP7 - probleme2.cpp
 * Auteurs :       Camille Bergeron-Miron et Samuel Meilleur 
 *   
 */

//Borne + de la LED @ B0
//Borne - de la LED @ B1

#define F_CPU 8000000

#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include "can.h"

#define ROUGE 0x01
#define VERT 0x02


uint16_t valeurCan16bit;
uint8_t valeurCanFinale;

int main()
{   
    
    
    DDRA = 0x00;
    DDRB = 0xff;
  
    
    can convertisseur;
    
    for(;;){
        
        valeurCan16bit = convertisseur.lecture(0x01);
        valeurCan16bit >>= 2;
        valeurCanFinale = valeurCan16bit;
        
        if(valeurCanFinale < 110){
            PORTB = VERT; 
        }
        else if (valeurCanFinale > 220){
            PORTB = ROUGE;
        }
        else{
            PORTB = VERT;
            _delay_ms(7);
            PORTB = ROUGE;
            _delay_ms(3);
        }
    }
}


