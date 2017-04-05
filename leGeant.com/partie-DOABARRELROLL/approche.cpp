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


 void main(){
     
     while(!capteurs.estPerdu()){
         capteurs.lecture();
         capteurs.lineTracking();
    }
     
    ecrire1('D',2);
	ecrire1('D',3);        
    
    //
    // On fait avancer le robot jusqu'à ce qu'il arrive au bout. Puis on le fait reculer de maniere manuelle
    // jusqu'à ce que son centre de rotation soit au dessus de C (j'ai pas trouvé de meilleur moyen :/)
    //
    
    ajustementPwmMoteurs(20,20);
    _delay_ms(200);
    ecrire0('D',2);
	ecrire0('D',3);            // Ici je fais freiner le robot pour être sur de s'arreter
    ajustementPwmMoteurs(10,10);
    _delay_ms(50);
    ajustementPwmMoteurs(0,0);
    
    for(uint8_t compteurLignes = 0; i < 9; i++) {
       
        while(!capteurs.estPerdu())
            capteurs.tournerGauche();
        
        while(capteurs.estPerdu()
           capteurs.tournerGauche();
    }
     
    etat++; 
     
    
}
