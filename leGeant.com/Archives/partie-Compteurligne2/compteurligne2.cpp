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
                //linetracking() intermittent avec un compteur d'intersection jusqu'a l'emplacement (rouge ou vert) theorique
                //du parking
                uint8_t triggerBonneInter = 0;
                if(couleurChoisie == VERT)
                    triggerParking = 2;
                else
                    triggerParking = 4;

                uint8_t compteurIntersection = 0;

                while(!capteur.estIntersection()){

                    capteur.lecture();
                    capteur.lineTracking();
                }
                compteurIntersection++;
                while(capteur.estIntersection()) {
                  capteur.lecture();
                  ajustementPwmMoteurs(50,50);
                }

                if(compteurIntersection == triggerBonneInter) {
                    etat++;
                }
            }
