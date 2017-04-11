#include "header.h"
#define VERT 'v'     //lors du choix de la couleur
#define ROUGE 'r'   //lors du choix de la couleur
#define GAUCHE 'g'  //lors du choix du coté avec la photorésistance
#define DROIT 'd'   //lors du choix du coté avec la photorésistance


//Énumération des états de la machine à état
enum Etats {COULEUR = 0, UTURN = 1, TOGA = 2, TOGAB = 3, COMPTEURLIGNE_1 = 4, PARKING_1 = 5, TOABC = 6, COMPTEURLIGNE_2 = 7, ALLERETOUR = 8, CINQ40 = 9, PHOTORESISTANCE = 10, INTERMITTENCE = 11, TOAGC = 12, TOGAH = 13, PARKING_2 = 14};

//Variables globales
volatile char    couleurChoisie = '\0'; //NUL (pas encore choisi)
volatile uint8_t etat = 0; //Initialisation de la variable etat permettant de décrire l'état présent de la machine à états
volatile bool    commencerParking = false; 
volatile uint8_t compteurIntersection = 0;
char cote;
uint8_t compteurInterrupt = 0;


ISR(INT0_vect){
    
      if (etat == PHOTORESISTANCE){
         switch(cote){
         case DROIT:
                  /* etat = TOURNER_DROIT_1 */

                ecrire1('C', 0);    /* A RETIRER */
                ecrire0('C', 1);    /* A RETIRER */

                EIMSK &= ~(1 << INT0);  //désactive INT0 si le coté a été choisi
                etat++;     //passe à l'état suivant
                break;
            case GAUCHE:
                /* etat = TOURNER_GAUCHE_1 */

                ecrire1('C', 1);    /* A RETIRER */
                ecrire0('C', 0);    /* A RETIRER */

                EIMSK &= ~(1 << INT0);  //désactive INT0 si le coté a été choisi
                etat++;     //passe à l'état suivant
                break;
        }
    }
}


ISR(INT2_vect){
    if(etat == COULEUR){
        _delay_ms(30);
        if(! (PINB & (1 << 2)) ){   //anti-rebond
            switch(couleurChoisie){
                case VERT:
                    couleurChoisie = ROUGE;
                    ecrire1('C', 0);
                    ecrire0('C', 1);
                    break;
                case ROUGE:
                    couleurChoisie = VERT;
                    ecrire1('C', 1);
                    ecrire0('C', 0);
                    break;
                default:        //quand couleurChoisie est null.
                    couleurChoisie = VERT;
                    ecrire1('C', 1);
                    ecrire0('C', 0);
                    break;
            }
        }
    }
}

ISR(TIMER2_COMPA_vect) {
	if(etat == PARKING_1){
	  if(compteurInterrupt < 50){
			minuterie(255);
			compteurInterrupt++;
		}
	  else
			commencerParking = true;
	}
}


int main() {
	Capteurs capteur;  //Création d'un objet de classe Capteur
	etat = COULEUR;   //Assignation de l'état initiale COULEUR

     //Machine à état décrivant le parcours du robot
    for(;;){
        switch(etat) {
            case COULEUR:
            {
                DDRC = 0xff;    //mode ecriture pour la DEL
                DDRD = 0x00;    //mode lecture pour lire les interrupts
                initialisationINT2(1,0);    //falling edge activates interrupt.
                while(etat == COULEUR){
                    if (!(PINB & (1<<3))){
                        if(couleurChoisie){  //ne fait rien si la couleur n'a pas encore été choisie.
                            etat++;
                            EIMSK &= ~(1 << INT2);   //INT2 désactivé, le choix de couleur ne peut plus être changé.
                        }
                    }
                }
            }
            case UTURN: //tourner 180 degres
            {
                    DDRD = 0xFF; //PORT D en sortie pour le signal des moteurs
                    initialisationPwmMoteurs(); // Configure les registres d'initialisation du timer1 pour le PWM moteur.
                    capteur.tourner180Gauche();
                    etat++;
                    break;
            }

            case TOGA: //linetracking() jusqu'a intersection du segment GA
            {
              while (!capteur.estIntersection())		//tant qu'on ne detecte pas d'intersection
              {
                capteur.lecture();
                capteur.lineTracking();
              }
              capteur.intersectionGauche();
              etat++;
					break;
            }
            case TOGAB:
            {
                //linetracking() jusqu'a l'intersection GAB et tourne a gauche sur le segment AB
                while (!capteur.estIntersection())		//tant qu'on ne detecte pas d'intersection
                {
					capteur.lecture();
					capteur.lineTracking();
				}
				capteur.intersectionGauche();
				etat++;

                break;
            }

            case COMPTEURLIGNE_1:
            {
                //linetracking() intermittent avec un compteur d'intersection jusqu'a l'emplacement (rouge ou vert) theorique
                //du parking

                //Tant qu'on ne se retrouve pas sur l'intersection, on suit la ligne.
                while(!capteur.estIntersection()){
                    capteur.lecture();
                    capteur.lineTracking();
                }
                compteurIntersection++;
                //Tant qu'on se trouve sur l'intersection, on avance tout droit.
                while(capteur.estIntersection()) {
                  capteur.lecture();
                  ajustementPwmMoteurs(50,50); 
                }
                
                //Selon la couleurChoisie au début, on vérifie si on a dépasser le bon nombre de ligne avant de passer à l'état suivant (le stationnement du robot)
                switch (couleurChoisie){
                    case VERT:
                        if (compteurIntersection == 3)
                            etat++;
                        break;
                    
                    case ROUGE:
                        if (compteurIntersection == 6)
                            etat++;
                        break;
                }

                break;
            }
            case PARKING_1:
            {
              _delay_ms(9999999999);
            }

            case TOABC:
            {
                //linetracking() jusqu'a l'intersection ABC et tourne a gauche sur le segment BC
				while (!capteur.estIntersection())		//tant qu'on ne detecte pas d'intersection
				{
					capteur.lecture();
					capteur.lineTracking();
				}
				capteur.intersectionGauche();
				etat++;
                break;
            }


            case COMPTEURLIGNE_2:
            {
                //
                // Compte le nombre d'intersection en fonction de la couleur choisie
                //
                uint8_t triggerBonneIntersection = 0;
                if(couleurChoisie == VERT)
                    triggerBonneIntersection = 2;
                else
                    triggerBonneIntersection = 4;

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

                if(compteurIntersection == triggerBonneIntersection) {
                    etat++;
                }
            }

            case ALLERETOUR:
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
                        capteur.tourner180Gauche();
                        triggerSwitchEtat = 1;
                    }


                    while(!capteur.estIntersection()) {
                        capteur.lecture();
                        capteur.lineTracking();
                    }

                    capteur.intersectionGauche();

                    for (uint8_t i = 0; i < triggerSwitchEtat; i++){

                        while(!capteur.estIntersection()) {
                        capteur.lecture();
                        capteur.lineTracking();
                        }
                        while(capteur.estIntersection())
                            ajustementPwmMoteurs(60,60);
                    }

                    etat++;
                }

            case CINQ40:
            {

                while(!capteur.estPerdu()){
                    capteur.lecture();
                    capteur.lineTracking();
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

                for(uint8_t compteurLignes = 0; compteurLignes < 9; compteurLignes++) {

                    while(!capteur.estPerdu())
                        capteur.tournerGauche();

                    while(capteur.estPerdu())
                    capteur.tournerGauche();
                }
                etat++;
            }

            case PHOTORESISTANCE:
            {
                uint16_t valeurCan16bitDroit;
                uint16_t valeurCan16bitGauche;
                DDRC = 0xff;    /* A RETIRER */
                DDRA = 0x00;    //portA en lecture pour les photoresitances.
                can convertisseur;
                initialisationINT0(1,1);   //rising edge
                while(etat==PHOTORESISTANCE) {
                    valeurCan16bitDroit = convertisseur.lecture(PINA1);   //lecture pin1 portA
                    valeurCan16bitDroit >>= 2;
                    valeurCan16bitGauche = convertisseur.lecture(PINA0);    //lecture pin0 portA
                    valeurCan16bitGauche >>= 2;

                    if(valeurCan16bitDroit > 230) {
                        cote = DROIT;
                        _delay_ms(50);
                    }
                    else if(valeurCan16bitGauche > 230 ) {
                        cote = GAUCHE;
                        _delay_ms(50);
                    }
                }

            //case INTERMITTENCE:

           // case TOAGC:
           // case TOGAH:
           // case PARKING_2:
            }
        }

    }
}
