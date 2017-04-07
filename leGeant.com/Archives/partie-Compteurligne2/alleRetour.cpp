#define F_CPU 8000000
#include <avr/io.h> 
#include <util/delay_basic.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "librairieFonctions.h"
#include "memoire_24.h"
#include "capteurs.h"
#define VERT 'v'
#define ROUGE 'r'


 void main()
 
 {
     
     uint8_t triggerSwitchEtat = 0;
     capteur.intersectionGauche();
              
     while(!capteur.estPerdu()) {
         capteur.lecture();
         capteur.lineTracking();
    }
    
    if(couleurChoisie == VERT) {
        capteur.tourner180Gauche();
        triggerSwitchEtat = 3;
    }
    else {
        capteur.tourner180Droite();
        triggerSwitchEtat = 1;
    }
    
    while(!capteur.estIntersection) {
        capteur.lecture();
        capteur.lineTracking();
    }
    
    capteur.intersectionGauche();
    
    for (uint8_t i = 0; i < triggerSwitchEtat; i++){
     
        while(!capteur.estIntersection()) {
        capteur.lecture();
        capteur.lineTracking();
        }
        while(capteur.estIntersection)
            ajustementPWMMoteurs(60,60);
        
    }
    
    etat++;
}
