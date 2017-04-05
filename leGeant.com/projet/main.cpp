#define F_CPU 8000000
#include "header.h"

#define VERT 'v'    //lors du choix de la couleur
#define ROUGE 'r'   //lors du choix de la couleur
#define GAUCHE 'g'  //lors du choix du coté avec la photorésistance
#define DROIT 'd'   //lors du choix du coté avec la photorésistance

ISR(INT0_vect);
ISR(INT1_vect);
ISR(TIMER1_COMPA_vect);

enum Etats {
    COULEUR, UTURN, TOGA, TOGAB, COMPTEURLIGNE_1, PARKING_1, TOABC, COMPTEURLIGNE_2, ALLERETOUR, CINQ40, FOLLOWLIGNE, PHOTORESISTANCE, INTERMITTENCE, TOAGC, TOGAH, PARKING_2 
    };

volatile char    couleurChoisie = '\0'; //NUL (pas encore choisi)
volatile uint8_t etat = COULEUR;
volatile bool    commencerParking = false;
char cote;

ISR(INT0_vect);
ISR(INT1_vect);
ISR(INT2_vect);
ISR(TIMER1_COMPA_vect);

int main(){
    for(;;){
        switch(etat) {
            case COULEUR:
                DDRC = 0xff;    //mode ecriture pour la DEL
                DDRD = 0x00;    //mode lecture pour lire les interrupts
                initialisationINT2(1,0);    //falling edge activates interrupt.
                while(etat == COULEUR){
                    if (!(PINB & (1<<3))){
                        if(couleurChoisie){  //ne fait rien si la couleur n'a pas encore été choisie.
                            etat = PHOTORESISTANCE;
                            EIMSK &= ~(1 << INT2);   //INT2 désactivé, le choix de couleur ne peut plus être changé.
                        }   
                    }
                }
            case PHOTORESISTANCE:
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
                
        }

    }
}

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

ISR(INT1_vect){ 
    if(couleurChoisie){  //ne fait rien si la couleur n'a pas encore été choisie.
        EIMSK &= ~(1 << INT0) & ~(1 << INT1);   //interruptions désactivées pour INT0 et INT1, le choix de couleur ne peut plus être changé.
        etat = PHOTORESISTANCE; //etat suivant.
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
