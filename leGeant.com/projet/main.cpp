#include "header.h"

//Énumération des états de la machine à état
enum Etats {
    COULEUR, UTURN, TOGA, TOGAB, COMPTEURLIGNE_1, PARKING_1, TOABC, COMPTEURLIGNE_2, ALLERETOUR, CINQ40, PHOTORESISTANCE, INTERMITTENCE, TOAGC, TOGAH, PARKING_2
};

volatile uint8_t etat = COULEUR;              //Initialisation de la variable etat à l'état initial.
volatile uint8_t triggerBonneIntersection;
volatile uint8_t  compteurIntersection2;
volatile uint16_t repetitionMinuterie = 0; 
volatile char couleurChoisie = '\0';          //=null (pas encore choisi)
volatile bool commencerParking = false;
volatile bool peutRepartir = false;
float uniteTempsDistance;
char cote;                                    //Variable pour le choix du coté lors de l'étape avec les photorésitances.
bool minuterieActive = false;                 //bool qui indique si la minuterie est active

ISR(INT0_vect){
    if (etat == PHOTORESISTANCE){
        switch(cote){
            case DROIT:
                EIMSK &= ~(1 << INT0);  //désactive INT0 si le coté a été choisi
                etat++;     //passe à l'état suivant
                break;
            case GAUCHE:
                EIMSK &= ~(1 << INT0);  //désactive INT0 si le coté a été choisi
                etat++;     //passe à l'état suivant
                break;
        }
    }
    if(etat == PARKING_1){
        etat = COMPTEURLIGNE_2;
    }
}

ISR(INT2_vect){
    if(etat == COULEUR){
        _delay_ms(30);
        if(! (PINB & (1 << 2)) ){   //anti-rebond
            switch(couleurChoisie){
                case VERT:
                    couleurChoisie = ROUGE;
                    PORTC = 0x01;
                    break;
                case ROUGE:
                    couleurChoisie = VERT;
                    PORTC = 0x02;
                    break;
                default:        //quand couleurChoisie est null.
                    couleurChoisie = VERT;
                    PORTC = 0x02;
                    break;
            }
        }
    }
}

ISR(PCINT1_vect){
    if(etat == COULEUR){
        if(couleurChoisie){         //Ne fait rien si la couleur n'a pas encore été choisie.
            EIMSK &= ~_BV(INT2);    //INT2 désactivé, le choix de couleur ne peut plus être changé.
            PCMSK1 &= ~_BV(PCINT8); //
            etat = UTURN;
        }
    }
}

ISR(TIMER2_COMPA_vect) {
    if ((etat == COMPTEURLIGNE_1) || (etat == PARKING_1)){
        if (minuterieActive){
            repetitionMinuterie++;
            minuterie(250);
        }
    }
}


int main() {
	Capteurs capteur;   //Création d'un objet de classe Capteur
	etat = COULEUR;     //Assignation de l'état initiale COULEUR
    uint16_t compteurInterrupt = 0;     //variable qui compte 
    
    
    //Machine à état décrivant le parcours du robot
    for(;;){
        switch(etat) {
            case COULEUR: {       //CHOIX DE LA COULEUR
                DDRC = 0xff;                //mode ecriture pour la DEL.
                DDRD = 0x00;                //mode lecture pour lire les interrupts.
                initialisationINT2(1,0);    //falling edge activates interrupt.
                initialisationPCINT8();     //pin change interrupt activé sur B0.
                while(etat == COULEUR);     //attend le interrupt pour la confirmation de la couleur.
                break;
            }
            
            case UTURN:{        //ROTATION 180 DEGRÉS
                DDRD = 0xFF; //PORT D en sortie pour le signal des moteurs
                initialisationPwmMoteurs(); // Configure les registres d'initialisation du timer1 pour le PWM moteur.
                capteur.tourner180Gauche();
                etat = TOGA;
                break;
            }
            
            case TOGA: {        //LINETRACKING JUSQU'À L'INTERSECTION GA
              while (!capteur.estIntersection()){       //tant qu'on ne detecte pas d'intersection
                capteur.lecture();
                capteur.lineTracking();
              }
              capteur.intersectionGauche();
              etat = TOGAB;
              break;
            }
            
            case TOGAB: {       //LINETRACKING JUSQU'À L'INTERSECTION GAB 
                while (!capteur.estIntersection()){      //linetracking tant que l'intersection n'est pas détecté
                    capteur.lecture();
                    capteur.lineTracking();
                }
                capteur.intersectionGauche();
                etat = COMPTEURLIGNE_1;
                break;
            }
            
            case COMPTEURLIGNE_1: {     //COMPTEUR LIGNE AVANT PARKING_1.
                const float DISTANCE_CM_INTERSECTION = 15.3;
                uint8_t compteurIntersection = 0;
                uint8_t triggerParking;
                
                switch(couleurChoisie){
                    case VERT: 
                        triggerParking = 3;
                        break;
                    case ROUGE:
                        triggerParking = 6;
                        break;
                }
                
                while (compteurIntersection < triggerParking){
                    while(!capteur.estIntersection()){
                        capteur.lecture();
                        capteur.lineTracking();
                    }
                    compteurIntersection++;
                    if (compteurIntersection == (triggerParking-2)){
                        minuterieActive = true;
                        minuterie(250);
                    }
                    if (compteurIntersection == triggerParking){
                        minuterieActive = false;
                        uniteTempsDistance = repetitionMinuterie/DISTANCE_CM_INTERSECTION;
                    }
                    while(capteur.estIntersection()){
                        capteur.lecture();
                        ajustementPwmMoteurs(50,50);
                    }
                }
                etat = PARKING_1;
                break;
            }
            
            case PARKING_1: {
                const float DISTANCE_PARKING = 26.0;
                repetitionMinuterie = 0;
                minuterieActive = true;
                minuterie(250);
                while (repetitionMinuterie < (DISTANCE_PARKING * uniteTempsDistance)){
                    capteur.lecture();
                    capteur.lineTracking();
                }
                minuterieActive = false;
                capteur.tournerGauche();
                _delay_ms(1180);
                Moteurs::reculer();
                ajustementPwmMoteurs(50,50);
                _delay_ms(1500);
                ajustementPwmMoteurs(0,0);
                DDRC &= ~(1 << 2);
                while (!(PINC & 0x04));
                
            
                etat = TOABC;
                break;
            }
        
            case TOABC: {        //linetracking() jusqu'a l'intersection ABC et tourne a gauche sur le segment BC
               
                capteur.tourner180Droite();
                switch(couleurChoisie)
                    case VERT: 
                        triggerBonneIntersection = 4;
                        break;
                    case ROUGE:
                        triggerBonneIntersection = 1;
                        break;{
                    
                for (uint8_t i = 0; i < triggerBonneIntersection - 1; i++){
                    while (!capteur.estIntersection()){
                        capteur.lecture();
                        capteur.lineTracking();
                    }
                    ajustementPwmMoteurs(50,50); 
                    while (capteur.estIntersection()){
                        capteur.lecture();
                    }
                }
    
                while (!capteur.estIntersection()){		//tant qu'on ne detecte pas d'intersection
                    capteur.lecture();
                    capteur.lineTracking();
                }
                capteur.intersectionGauche();
                etat++;
                
                if(couleurChoisie == VERT)             //utile pour le prochain etat
                    triggerBonneIntersection = 2;
                else
                    triggerBonneIntersection = 4;
                break;
            }
            

            case COMPTEURLIGNE_2: {
               while(!capteur.estIntersection()){

                    capteur.lecture();
                    capteur.lineTracking();
                    }
                    compteurIntersection2++;                     //les capteurs detectent une intersection
            
                if(compteurIntersection2 == triggerBonneIntersection+1) {        //l'axe de rotation du robot est sur la ligne de sa couleur
                
                    capteur.tournerGauche();
                
                    while (!capteur.estPerdu()) {       //le capteur du centre est encore sur le segment
                        capteur.lecture();
                    } break;
                    while (!capteur.getSensor(2)){     //le capteur du centre est entre les deux lignes
                    
                        capteur.lecture();
                    }
                    ajustementPwmMoteurs(0,0); //le capteur du centre est sur la nouvelle ligne
                    etat++;
                }

                while(capteur.estIntersection()) {      //le robot est sur l'intersection
                    capteur.lecture();                    
                    ajustementPwmMoteurs(50,50); 
                }
                break;
            }
            
            case ALLERETOUR: {
                while (!capteur.estPerdu()){
                    capteur.lecture();
                    capteur.lineTracking();
                }                               //les capteurs detectent la fin de la ligne
                ajustementPwmMoteurs(60,60);
                _delay_ms(700);                 //a ajuster pour que axe de rotation du robot soit au bout de la ligne
                
                switch(couleurChoisie){
                    case VERT:
                        capteur.tourner180Gauche();   //le robot a fait un 180 sur la ligne
                        break;
                        
                    case ROUGE:
                        capteur.tourner180Droite(); 
                        break;
                }
                                    
                while (!capteur.estIntersection()){
                    capteur.lecture();
                    capteur.lineTracking();
                }
                capteur.intersectionGauche();
                
                capteur.lecture();
                while(capteur.estIntersection()){
                    capteur.lecture();
                    ajustementPwmMoteurs(70,50);
                }
                compteurInterrupt = 1;
                etat++;
                break;
            }
            
            case CINQ40:{
                DDRD = 0xff;
                Moteurs::avancer();
                while(!capteur.estPerdu()){     //avance jusqu'a la fin de la ligne.
                    capteur.lecture();
                    capteur.lineTrackingTranquille();
                }
                Moteurs::reculer();
                ajustementPwmMoteurs(50,50);     //recule pendant 1.2 secondes.
                _delay_ms(4000);
                
                Moteurs::avancer();              
                ajustementPwmMoteurs(40,40);     //robot freine rapidement.
                _delay_ms(50);                   
                ajustementPwmMoteurs(0,0);

                for(uint8_t compteurLignes = 0; compteurLignes < 9; compteurLignes++) {
                    while(capteur.getSensor(2))
                        capteur.tournerGaucheTranquille();
                    
                    while(!capteur.getSensor(2))
                        capteur.tournerGaucheTranquille();
                }
                etat++;
                break;
            }

            case PHOTORESISTANCE:{
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
