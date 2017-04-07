#define F_CPU 8000000

#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include "can.h"
#include "header.h"

#define DROIT 'd' 
#define GAUCHE 'g'
#define ROUGE 0x01
#define VERT 0x02


//Sensor gauche @ A0
//Sensor droite @ A1

uint16_t valeurCan16bitDroit;
uint16_t valeurCan16bitGauche;
char cote;


ISR(INT0_vect);

int main()
{   
    DDRC = 0xff;    //portC en ecriture pour la led
    DDRA = 0x00;    //portA en lecture
    can convertisseur;
    initialisationINT0(0,1);   //rising edge
    
    /* CODE POUR ARRETE DE BOUGER */ 
    
    for(;;){
        valeurCan16bitDroit = convertisseur.lecture(PINA1);   //lecture pin1 portA
        valeurCan16bitDroit >>= 2;
        valeurCan16bitGauche = convertisseur.lecture(PINA0);    //lecture pin0 portA
        valeurCan16bitGauche >>= 2;
        
        if(valeurCan16bitDroit > 230) {
            cote = DROIT;
            _delay_ms(200);
        }
    
        else if(valeurCan16bitGauche > 230 ) {
            cote = GAUCHE;
            _delay_ms(200);
        }
    }
}

ISR(INT0_vect){
    switch(cote){
        case DROIT:
            //tourner a droite
            PORTC = ROUGE;
            EIMSK &= ~(1 << INT0);
            break;
        case GAUCHE:
            //tourner a gauche
            PORTC = VERT;
            EIMSK &= ~(1 << INT0);
            break;
    }
    //desactive external interrupt 0.
    /* ETAT SUIVANT */
            
}



