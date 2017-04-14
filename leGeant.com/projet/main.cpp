#include "header.h"

//Énumération des états de la machine à état
enum Etats {
    COULEUR, UTURN, TO_GA, TO_GAB, COMPTEURLIGNE_1, PARKING_1, TO_ABC, COMPTEURLIGNE_2, ALLERETOUR, CINQ40,INTERSECTION_PHOTO, PHOTORESISTANCE, INTERMITTENCE, TO_AGC, TO_GAH, PARKING_2
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
   /* if (etat == PHOTORESISTANCE){
        switch(cote){
            case DROIT:
                EIMSK &= ~_BV(INT0);  //désactive INT0 si le coté a été choisi
                etat++;     //passe à l'état suivant
                break;
            case GAUCHE:
                EIMSK &= ~_BV(INT0); //désactive INT0 si le coté a été choisi
                etat++;     //passe à l'état suivant
                break;
        }
    }*/
   
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
    if ((etat == COMPTEURLIGNE_1) || (etat == PARKING_1) || (etat == CINQ40)){
        if (minuterieActive){
            repetitionMinuterie++;
            minuterie(250);
        }
    }
    
}


int main() {
	Capteurs capteur;   //Création d'un objet de classe Capteur
	etat = COULEUR;     //Assignation de l'état initiale COULEUR
    DDRB = 0x00;
    initialisationPwmMoteurs();
    
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
                initialisationPwmMoteurs();     // Configure les registres d'initialisation du timer1 pour le PWM moteur.
                capteur.tourner180Gauche();
               // etat = TO_GA;
                etat = PHOTORESISTANCE;
                break;
            }
            
            case TO_GA: {        //LINETRACKING JUSQU'À L'INTERSECTION GA
                while (!capteur.estIntersection()){       //tant qu'on ne detecte pas d'intersection
                    capteur.lecture();
                    capteur.lineTracking();
                }
                capteur.intersectionGauche();
                etat = TO_GAB;
                break;
            }
            
            case TO_GAB: {       //LINETRACKING JUSQU'À L'INTERSECTION GAB 
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
                uint8_t triggerParking = 0;
                
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
                DDRC &= ~_BV(2);
               
                
                while(!(PINC & 0x04)){}
                etat++;               //remet en écriture pour les moteurs.
                break;
            }
        
            case TO_ABC: {        //linetracking() jusqu'a l'intersection ABC et tourne a gauche sur le segment BC
               
                capteur.tourner180Droite();
                switch(couleurChoisie){
                    case VERT: 
                        triggerBonneIntersection = 4;
                        break;
                    case ROUGE:
                        triggerBonneIntersection = 1;
                        break;
                }
                    
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
                etat = COMPTEURLIGNE_2;
                break;
            }
            

            case COMPTEURLIGNE_2: {
                switch(couleurChoisie){
                    case VERT: 
                        triggerBonneIntersection = 3;
                        break;
                    case ROUGE:
                        triggerBonneIntersection = 5;
                        break;
                }
                while(!capteur.estIntersection()){
                    capteur.lecture();
                    capteur.lineTracking();
                }
                compteurIntersection2++;                     //les capteurs detectent une intersection
            
                if(compteurIntersection2 == triggerBonneIntersection) {        //l'axe de rotation du robot est sur la ligne de sa couleur
                
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
                break;

            }
            
            case ALLERETOUR: {
                while (!capteur.estPerdu()){ //tant que la fin de la ligne n'est pas détecté
                    capteur.lecture();
                    capteur.lineTracking();
                }                               
                ajustementPwmMoteurs(60,60);
                _delay_ms(7.00);                 //a ajuster pour que axe de rotation du robot soit au bout de la ligne
                
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
                    ajustementPwmMoteurs(50, 70);
                }
                
                etat = CINQ40;
                break;
            }
            
            case CINQ40:{
                //DDRD = 0xff;
                Moteurs::avancer();
                while(!capteur.estPerdu()){ //avance jusqu'a la fin de la ligne.
                    if (capteur.estIntersection())
                    {
                        while (capteur.estIntersection()){ //On souhaite passer tout droit l'intersection restante
                            ajustementPwmMoteurs(50,50);
                            capteur.lecture();
                        }
                    }
                    capteur.lecture();
                    capteur.lineTracking();
                }
                ajustementPwmMoteurs(0,0);
                
                
               // capteur.tourner180Droite();
                capteur.tournerDroite();
                while (capteur.getSensor(2))
                    capteur.lecture();
                while (!capteur.getSensor(2))
                    capteur.lecture();
                ajustementPwmMoteurs(0,0);
                    
                
                
                
                minuterieActive = true;
                repetitionMinuterie = 0;
                minuterie(255);
                while (repetitionMinuterie < 16)
                {
                    capteur.lecture();
                    capteur.lineTracking();
                }
                minuterieActive = false;
                capteur.tournerGaucheTranquille();
                while (!capteur.getSensor(4))
                    capteur.lecture();
                
                for(uint8_t compteurLignes = 0; compteurLignes < 9; compteurLignes++) {
                    while(capteur.getSensor(2))
                        capteur.lecture();
                    while(!capteur.getSensor(2))
                        capteur.lecture();
                }
                ajustementPwmMoteurs(0,0);
                etat = INTERSECTION_PHOTO;
                break;
            }
            case INTERSECTION_PHOTO:
            {
                
                    while (!capteur.estPerdu())
                    {
                        capteur.lecture();
                        capteur.lineTracking();
                    }
                    capteur.intersectionDroite();
                    while (capteur.estIntersection())
                    {
                        capteur.tournerDroite();
                        capteur.lecture();}
                    while (!capteur.estIntersection())
                    {
                        capteur.lecture();
                        capteur.lineTracking();
                    }
                    capteur.intersectionGauche();
                    while (capteur.estIntersection()){
                        capteur.tournerGauche();
                        capteur.lecture();
                        
                        
                    }
                    while (!capteur.estIntersection())
                    {
                        capteur.lecture();
                        capteur.lineTracking();
                    }
                    ajustementPwmMoteurs(50,50);
                    _delay_ms(900);
                                                                    
                ajustementPwmMoteurs(0,0);
                etat = PHOTORESISTANCE;
                break;
                
            }
            case PHOTORESISTANCE:{
                uint16_t valeurCan16bitDroit;
                uint16_t valeurCan16bitGauche;
                DDRA = 0x00;    //portA en lecture pour les photoresitances.
                can convertisseur;
            /*    initialisationINT0(1,1);   //rising edge
                while(etat==PHOTORESISTANCE) {*/
                    DDRC &= ~_BV(2);
                    while(!(PINC & 0x04)){ 
                        
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
        
                etat = INTERMITTENCE;
                
                break;
                
            }

            case INTERMITTENCE:
            {
                Moteurs::avancer();
               ajustementPwmMoteurs(50,50);
               _delay_ms(600);
                if (cote == GAUCHE)
                {
                    capteur.intersectionGauche();
                    
                        
                }
                else 
                {
                    capteur.intersectionDroite();
                }
                
                while (!capteur.estPerdu()){
                    capteur.lecture();
                    capteur.lineTracking();
                    
                    
                }
                if (cote == GAUCHE){
                    capteur.intersectionDroite();
                }
                else
                {
                    capteur.intersectionGauche();
                }
                while (!capteur.estPerdu())
                {
                    capteur.lecture();
                    capteur.lineTracking();
                }
                
                if (cote == GAUCHE)
                    ajustementPwmMoteurs(40,60);
                
                else
                    ajustementPwmMoteurs(60,40);
                
                while (capteur.estPerdu()){
                    capteur.lecture();
                }
                
                while (!capteur.estIntersection())
                {
                    capteur.lecture();
                    capteur.lineTracking();
                }
                ajustementPwmMoteurs(0,0);
                
                
                
                    
                    
              /*  while (!capteur.estIntersection())
                {
                    capteur.lecture();
                    capteur.lineTracking();
                }
                if (cote == GAUCHE)
                {
                    capteur.intersectionDroite();
                }
                else
                {
                    capteur.intersectionDroite();
                }
                while (!capteur.estPerdu())
                {
                    capteur.lecture();
                    capteur.lineTracking();
                }
                ajustementPwmMoteurs(50,50);
                for (;;){}*/
            }

           // case TO_AGC:
           // case TO_GAH:
           // case PARKING_2:
            }
        }

    }

