#include "header.h"

//Énumération des états de la machine à état
enum Etats {
    COULEUR, UTURN, TOGA, TOGAB, COMPTEURLIGNE_1, PARKING_1, TOABC, COMPTEURLIGNE_2, ALLERETOUR, CINQ40, PHOTORESISTANCE, INTERMITTENCE, TOAGC, TOGAH, PARKING_2
};

volatile char    couleurChoisie = '\0';       //=null (pas encore choisi)
volatile uint8_t etat = COULEUR;              //Initialisation de la variable etat à l'état initial.
volatile bool    commencerParking = false;
volatile uint8_t triggerBonneIntersection;
volatile uint16_t repetitionMinuterie = 0; 
volatile float    uniteTempsDistance = 0;
volatile uint8_t  compteurIntersection2;
volatile bool     peutRepartir = false;
char cote;          //Variable pour le choix du coté lors de l'étape avec les photorésitances.
bool minuterieActive = false;       //bool qui indique si la minuterie est active

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
            }
            break;
            
            case UTURN:{        //ROTATION 180 DEGRÉS
                    DDRD = 0xFF; //PORT D en sortie pour le signal des moteurs
                    initialisationPwmMoteurs(); // Configure les registres d'initialisation du timer1 pour le PWM moteur.
                    capteur.tourner180Gauche();
                    etat++;
            }
            break;
            
            case TOGA: {        //LINETRACKING JUSQU'À L'INTERSECTION GA
              while (!capteur.estIntersection()){       //tant qu'on ne detecte pas d'intersection
                capteur.lecture();
                capteur.lineTracking();
              }
              capteur.intersectionGauche();
              etat++;
            }
            break;
            
            case TOGAB: {       //LINETRACKING JUSQU'À L'INTERSECTION GAB 
                while (!capteur.estIntersection()){      //linetracking tant que l'intersection n'est pas détecté
                    capteur.lecture();
                    capteur.lineTracking();
                }
                capteur.intersectionGauche();
                etat++;
            }
            break;
            
            case COMPTEURLIGNE_1: {
                //linetracking() intermittent avec un compteur d'intersection jusqu'a l'emplacement (rouge ou vert) theorique
                //du parking
                uint8_t triggerParking = 0;
                const float DISTANCE_CM_INTERSECTION = 15.3;
                uint8_t compteurIntersection = 0;
                
                if(couleurChoisie == VERT)
                        triggerParking = 3;
                        
                else
                        triggerParking = 6;
                       
                while (compteurIntersection < triggerParking)
                {
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
                repetitionMinuterie = 0;
                etat++;
                        
                
            }
            break;
            
            case PARKING_1: {
                
                const float DISTANCE_PARKING  = 26;
                minuterieActive = true;
                minuterie(250);
                
                while (repetitionMinuterie < (DISTANCE_PARKING * uniteTempsDistance))
                {
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
                DDRC &= ~(1<<2);
                while (!(PINC & 0x04)){}
                capteur.tourner180Droite();
                
                
                if (couleurChoisie == VERT){
                    triggerBonneIntersection = 4;
                }
                else {
                    triggerBonneIntersection = 1;
                }
               
                
                
                etat++;
            }
            break;

            case TOABC: {
                //linetracking() jusqu'a l'intersection ABC et tourne a gauche sur le segment BC
               for (uint8_t i = 0; i < triggerBonneIntersection - 1; i++)
               {
                   while (!capteur.estIntersection())
                   {
                       capteur.lecture();
                       capteur.lineTracking();
                   }
                   ajustementPwmMoteurs(50,50); 
                   while (capteur.estIntersection()){
                       capteur.lecture();
                       
                    }
                   
               }
    
                while (!capteur.estIntersection())		//tant qu'on ne detecte pas d'intersection
                {
                    capteur.lecture();
                    capteur.lineTracking();
                }
                capteur.intersectionGauche();
                etat++;
                
                if(couleurChoisie == VERT)             //utile pour le prochain etat
                    triggerBonneIntersection = 2;
                else
                    triggerBonneIntersection = 4;
                
            }
            break;

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
                    }
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
                
            }
            break;
            
            case ALLERETOUR: {
                while (!capteur.estPerdu()){
                    capteur.lecture();
                    capteur.lineTracking();
                }                               //les capteurs detectent la fin de la ligne
                ajustementPwmMoteurs(60,60);
                _delay_ms(700);               //a ajuster pour que axe de rotation du robot soit au bout de la ligne
                
                if (couleurChoisie == VERT){
                    capteur.tourner180Gauche();
                }
                else{
                    capteur.tourner180Droite();     
                }                               //le robot a fait un 180 sur la ligne
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
               
            }
            break;
            
            case CINQ40:{
                minuterie(250);
                while(compteurInterrupt < 156)
                {
                    capteur.lecture();
                    capteur.lineTracking();
                }
                ajustementPwmMoteurs(0,0);
                for(;;);
                /*  while(!capteur.estPerdu()){
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
                    }*/
                etat++;
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
