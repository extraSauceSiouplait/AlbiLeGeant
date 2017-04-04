#define F_CPU 8000000

#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include "can.h"
#include "header.h"

#define DROIT = 'd' 
#define GAUCHE = 'g'


//Sensor gauche @ A0
//Sensor droite @ A1

uint16_t valeurCan16bitDroit;
uint16_t valeurCan16bitGauche;
char cote;


int main()
{   
    DDRA = 0x00;    //portA en lecture
    can convertisseur;
    initialisationINT0(1,0);   //falling edge
    
    /* CODE POUR ARRETE DE BOUGER */ 
    
    for(;;){
        
        valeurCan16bitDroit = convertisseur.lecture(PINA1);   //lecture pin1 portA
        valeurCan16bitDroit >>= 2;
        valeurCan16bitGauche = convertisseur.lecture(PINA0);    //lecture pin0 portA
        valeurCan16bitGauche >>= 2;
        
        if(valeurCan16bitDroit > 230) 
            cote = DROIT;
    
        else if(valeurCan16bitGauche > 230) 
            cote = GAUCHE;
        
       
    }
}

ISR(INT0_vect){
    if(verifierRebondMecanique()){
        switch(cote){
            case DROIT:
                //tourner a droite
                PORTC = 0x01;
            case GAUCHE:
                //tourner a gauche
                PORTC = 0x02;
        }
            //bouge pas
    }
}



