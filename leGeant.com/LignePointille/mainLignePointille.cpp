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
		     capteur.lecture();          // Lecture de l'information par le capteur.
			 capteur.lineTracking();     // Le robot doit suivre des instructions pour avancer.

			 // Condition sur les capteurs, si tous les capteurs sont eteints ca veut dire que le robot a atteint les lignes
			 // pointillées.
			 if (sensors_[1] && (sensors_[2] && sensors_[3]&& sensors_[4]&& sensors_[5])) {
			 
			              // Le robot doit emettre un son par la fonction jouersonnerie.

				                     jouerSonorite(operande);
			 
			 }

			                                                                               	
	





