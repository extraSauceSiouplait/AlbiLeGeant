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

volatile char    couleurChoisie = 0; //NUL (pas encore choisi)
volatile uint8_t etat;
volatile bool    commencerParking = false;

ISR(INT0_vect){
    _delay_ms(30);
    if(!(PIND & (1 << 2 ))){ //Anti-rebond pour le bouton poussoir
        switch(couleurChoisie){
            case VERT:
                couleurChoisie = ROUGE; //Si la couleur précédente vert, la couleur futur devient rouge
                ecrire1('C', 0);
                ecrire0('C', 1);
                break;
            case ROUGE:
                couleurChoisie = VERT; //Si la couleur précédente rouge, la couleur futur devient vert
                ecrire1('C', 1);
                ecrire0('C', 0);
                break;
            default: //Quand aucune couleur n'a encore été choisie.
                couleurChoisie = VERT;
                ecrire1('C', 1);
                ecrire0('C', 0);
                break;
        }
    }
}


ISR(INT1_vect){
	//Pas besoin d'anti-rebond pour cette interruption, car elle est d'usage unique
    if(couleurChoisie == VERT || couleurChoisie == ROUGE){  //Ne fait rien si la couleur n'a pas encore été choisie.
        if (etat == 0){
			etat++;
		}


        EIMSK &= ~ (1 << INT0) & ~ (1 << INT1);   //interruptions désactivées pour INT0 et INT1, le choix de couleur ne peut plus être changé.
    }
}

ISR(TIMER1_COMPA_vect) {
    if(etat == 5) {
        commencerParking = true;
    }
}

int main() {
	Capteurs capteur;
	etat = 0;

	while(1){
		switch (etat) //Machine à état
		{
			case 0:
					DDRD = 0x00;        //PORT D en lecture pour lire les interruptions.
					DDRC = 0xff;        //PORT C en écriture pour la DEL.
					ecrire0('C', 0);    //Assure que la DEL est eteinte avant le choix de la couleur.
					ecrire0('C', 1);    //Assure que la DEL est eteinte avant le choix de la couleur.
					initialisationINT0(1,0);
					etat++;  			//A ENLEVER (permet de sauter au prochain etat)
					while(etat == 0){}		//tant que couleur n'est pas choisi
					break;
			case 1:  //tourner 180 degres
					DDRD = 0xFF; //PORT D en sortie pour le signal des moteurs
					initialisationPwmMoteurs(); // Configure les registres d'initialisation du timer1 pour le PWM moteur.
					capteur.tourner180();
					etat++;
					break;
			case 2:  //linetracking() jusqu'a intersection du segment GA
					while (!capteur.estIntersection())		//tant qu'on ne detecte pas d'intersection
					{
						capteur.lecture();
						capteur.lineTracking();
					}

					capteur.intersectionGauche();
					etat++;
                    break;

			case 3:
                //linetracking() jusqu'a l'intersection GAB et tourne a gauche sur le segment AB
				while (!capteur.estIntersection())		//tant qu'on ne detecte pas d'intersection
				{
					capteur.lecture();
					capteur.lineTracking();
				}
				capteur.intersectionGauche();
				etat++;
                break;
            case 4:
            {
                //linetracking() intermittent avec un compteur d'intersection jusqu'a l'emplacement (rouge ou vert) theorique
                //du parking
                uint8_t triggerParking = 0;
                if(couleurChoisie == VERT)
                    triggerParking = 3;
                else
                    triggerParking = 6;

                uint8_t compteurIntersection = 0;

                while(!capteur.estIntersection()){

                    capteur.lecture();
                    capteur.lineTracking();
                }
                compteurIntersection++;
                while(capteur.estIntersection()) {
                  capteur.lecture();
                }

                if(compteurIntersection == triggerParking) {
                    etat++;
                }
            }
                break;
            case 5:
                //linetracking() durant une minuterie prédéterminée
                //la fin de la minuterie active commencerParking, ce qui initialise la Séquence de parking

                minuterie(30000);
                while(!commencerParking) {
                capteur.lecture();
                capteur.lineTracking();
                }
                capteur.tournerGauche();
                _delay_ms(2000);
                ajustementPwmMoteurs(0,0);
                PORTA = 0x01;
                _delay_ms(500);
                PORTA = 0x00;
                _delay_ms(2000);


                do {
                    capteur.lecture();
                    capteur.tournerDroite();
                } while(!capteur.getSensor(2));

                etat++;
                break;
            case 6:
                capteur.lineTracking();
                break;
        }
	}
}
