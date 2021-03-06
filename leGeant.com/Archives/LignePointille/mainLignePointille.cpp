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




int main() {
	Capteurs capteur;
	etat = 0; 
	while(1){
		switch (etat) //Machine à état
	    case 1 :  
            
            while(!capteur.estPerdu()) {
		     capteur.lecture();          // Lecture de l'information par le capteur.
			 capteur.lineTracking();     // Le robot doit suivre des instructions pour avancer.
            }
            
            ajustementPwmFrequence(440);
            
            for(uint8_t = 0; i < 4; i++){
            
                while(capteur.EstPerdu())
                    ajustementPwmMoteurs(60,60);
                
                while(!capteur.estPerdu()) {
                capteur.lecture();          // Lecture de l'information par le capteur.
                capteur.lineTracking();     // Le robot doit suivre des instructions pour avancer.
                }
            }
            
            while(capteur.estPerdu();)
                    ajustementPwmMoteurs(60,60);
                
            etat++;
    }
}
			                                                                               	
	





