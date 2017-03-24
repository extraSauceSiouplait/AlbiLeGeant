/*
 * Titre:          TP6 - probleme1.cpp
 * Auteurs :       Camille Bergeron-Miron et Samuel Meilleur 
 */

#define F_CPU 8000000

#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include "memoire_24.h"

#define ROUGE 0x01
#define VERT 0x02

// THE POWER OF THE SCARF FLOWS THROUGH ME !!!!!!!!!


int main()
{
    DDRA = 0xff;
    bool estPareil = true;
    
    const int strLength = 39;
    
    Memoire24CXXX memoire;
    char str[strLength] = "*E*C*O*L*E**P*O*L*Y*T*E*C*H*N*I*Q*U*E*";
    
   // uint8_t banc = memoire.choisir_banc(0);
    for (int i = 0; i < strLength; i++){
        memoire.ecriture(i, str[i]);
        _delay_ms(5);
    }
    
    uint8_t* donnee;
    
    for (int i = 0; i < strLength; i++){
        memoire.lecture(i, donnee);
        _delay_ms(5);
        if(*donnee != str[i])
            estPareil = false;
    } 
    
    switch(estPareil){
        case true: PORTA = VERT; break;
        case false: PORTA = ROUGE; break;
    }
        
    
    
    
    
}
    

