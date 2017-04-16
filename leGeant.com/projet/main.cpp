#include "header.h"

//Énumération des états de la machine à état
enum Etats {
    COULEUR, UTURN, TO_GA, TO_GAB, COMPTEURLIGNE_1, PARKING_1, TO_ABC, COMPTEURLIGNE_2, ALLERETOUR, CINQ40,INTERSECTION_PHOTO, PHOTORESISTANCE, GUIDAGEFOURCHE , INTERMITTENCE, TO_GAH, PARKING_2
};


volatile uint8_t etat = COULEUR;              //Initialisation de la variable etat à l'état initial.
volatile uint16_t repetitionMinuterie = 0; 
volatile char couleurChoisie = '\0';          //=null (pas encore choisi)
float uniteTempsDistance;
uint8_t triggerBonneIntersection;
uint8_t  compteurIntersection2;
char cote;                      //Variable pour le choix du coté lors de l'étape avec les photorésitances.
bool minuterieActive = false;   //bool qui indique si la minuterie est active
uint8_t compteurPhoto = 0;


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
    if (minuterieActive){
        repetitionMinuterie++;
        minuterie(250);
    }

}


int main() {
	DDRB = 0x00;                //mode lecture pour lire les interrupts.
	DDRC = 0x03;                //mode ecriture pour la DEL.
	DDRD = 0xFF; //PORT D en sortie pour le signal des moteurs
	
	Capteurs capteur;   //Création d'un objet de classe Capteurs.
    
    //Machine à état décrivant le parcours du robot
    for(;;){
        switch(etat) {
            case COULEUR: {       //CHOIX DE LA COULEUR                
                initialisationINT2(1,0);    //falling edge activates interrupt.
                initialisationPCINT8();     //pin change interrupt activé sur B0.
                while(etat == COULEUR);     //attend le interrupt pour la confirmation de la couleur.
                break;
            }
            
            case UTURN:{        //ROTATION 180 DEGRÉS
                initialisationPwmMoteurs();
                capteur.tourner180Gauche();
                etat = PARKING_2;
                break;
            }
            
            case TO_GA: {        //LINETRACKING JUSQU'À L'INTERSECTION GA
                while (!capteur.estIntersection()){       //tant qu'on ne detecte pas d'intersection
                    capteur.lineTracking();
                }
                capteur.intersectionGauche();
                etat = TO_GAB;
                break;
            }
            
            case TO_GAB: {       //LINETRACKING JUSQU'À L'INTERSECTION GAB 
                while (!capteur.estIntersection()){      //linetracking tant que l'intersection n'est pas détecté
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
                    capteur.lineTracking();
                }
                minuterieActive = false;
                capteur.tournerGauche();
                _delay_ms(1300);
                Moteurs::reculer();
                ajustementPwmMoteurs(50,50);
                _delay_ms(1200);
                Moteurs::freiner();            
                
                while(!(PINC & 0x04)){}
                etat = TO_ABC;               //remet en écriture pour les moteurs.
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
                        capteur.lineTracking();
                    }
                    ajustementPwmMoteurs(50,50); 
                    while (capteur.estIntersection()){
                        capteur.lecture();
                    }
                }
    
                while (!capteur.estIntersection()){		//tant qu'on ne detecte pas d'intersection
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
                    Moteurs::freiner();
                    etat = ALLERETOUR;
                }

                while(capteur.estIntersection()) {      //le robot est sur l'intersection
                    capteur.lecture();                    
                    ajustementPwmMoteurs(50,50); 

                }
                break;

            }
            
            case ALLERETOUR: {
                while (!capteur.estPerduLong()){ //tant que la fin de la ligne n'est pas détecté
                    //capteur.lecture();
                    capteur.lineTracking();
                }     
                Moteurs::freiner();
                _delay_ms(500);
                
                switch(couleurChoisie){
                    case VERT:
                        capteur.tourner180Gauche();   //le robot a fait un 180 sur la ligne
                        break;
                        
                    case ROUGE:
                        capteur.tourner180Droite(); 
                        break;
                }
                                    
                while (!capteur.estIntersection()){
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
                Moteurs::avancer();
                while(!(capteur.estPerdu())){       //avance jusqu'a la fin de la ligne.
                    if (capteur.estIntersection())
                    {
                        while (capteur.estIntersection()){ //On souhaite passer tout droit l'intersection restante
                            ajustementPwmMoteurs(50,50);
                            capteur.lecture();
                        }
                    }
                    capteur.lineTracking();
                }
                Moteurs::freiner();
                _delay_ms(100);
                
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
                    capteur.lineTracking();
                }
                minuterieActive = false;
                capteur.tournerGauche();
                while (!capteur.getSensor(4))
                    capteur.lecture();
                
                for(uint8_t compteurLignes = 0; compteurLignes < 9; compteurLignes++) {
                    while(capteur.getSensor(2))
                        capteur.lecture();
                    while(!capteur.getSensor(2))
                        capteur.lecture();
                }
                 Moteurs::freiner();
                etat = INTERSECTION_PHOTO;
                break;
            }
            case INTERSECTION_PHOTO:
            {
                
                while (!capteur.estIntersection())
                {
                    capteur.lineTrackingTranquille();
                }
                
                capteur.intersectionDroite();  
                
                while (!capteur.estIntersection())
                {;
                    capteur.lineTracking();
                }
                capteur.intersectionGauche();
                
                while (!capteur.estIntersection())
                {
                    capteur.lineTracking();
                }
                
                                                                    
                Moteurs::freiner();
                etat = PHOTORESISTANCE;
                break;
                
            }
            case PHOTORESISTANCE:{
                uint16_t valeurCan16bitDroit;
                uint16_t valeurCan16bitGauche;
                DDRA = 0x00;    //portA en lecture pour les photoresitances.
                can convertisseur;
                DDRC &= ~_BV(2);
                while(!(PINC & 0x04)){ 
                    valeurCan16bitDroit = convertisseur.lecture(PINA1);   //lecture pin1 portA
                    valeurCan16bitDroit >>= 2;
                    valeurCan16bitGauche = convertisseur.lecture(PINA0);    //lecture pin0 portA
                    valeurCan16bitGauche >>= 2;

                    if(valeurCan16bitDroit > 240 && (valeurCan16bitDroit > valeurCan16bitGauche)) {
                        cote = DROIT;
                        _delay_ms(50);
                    }
                    else if(valeurCan16bitGauche > 240 ) {
                        cote = GAUCHE;
                        _delay_ms(50);
                    }
                }
                if (cote == DROIT || cote == GAUCHE) {
                    etat = GUIDAGEFOURCHE;
                }
                break;
                
            }

            case GUIDAGEFOURCHE:
            {
                Moteurs::avancer();
                Moteurs::boost();
                ajustementPwmMoteurs(50,50);
                while(!capteur.estPerdu()){
                    capteur.lecture();
                }
                Moteurs::freiner();
                _delay_ms(1000);
                
                switch(cote){
                    case GAUCHE:
                        capteur.tournerGauche();
                        while(!capteur.getSensor(3))
                            capteur.lecture();
                        while(!capteur.estPerdu()){
                            capteur.lineTrackingTranquille();
                        }
                        Moteurs::freiner();
                        capteur.tournerDroite();
                        _delay_ms(200);
                        Moteurs::avancer();
                        ajustementPwmMoteurs(50,50);
                        while(capteur.estPerdu())
                            capteur.lecture();
                            
                        break;
                        
                    case DROIT:
                        capteur.tournerDroite();
                        while(!capteur.getSensor(1))
                            capteur.lecture();
                        while(!capteur.estPerdu())
                            capteur.lineTrackingTranquille();
                        Moteurs::freiner();
                        capteur.tournerGauche();
                        _delay_ms(200);
                        Moteurs::avancer();
                        ajustementPwmMoteurs(50,50);
                        while(capteur.estPerdu())
                            capteur.lecture();
                        
                        break;
                }
                                                //le robot a atteint le prochain segment
                if (compteurPhoto < 1)           //doit passer par un autre T ou est rendu au pointillé
                {
                    while (!capteur.estIntersection())
                    {
                        capteur.lineTracking();
                    }
                    Moteurs::freiner();
                    etat = PHOTORESISTANCE;
                    cote = '\0';
                    compteurPhoto++;
                }
                else
                    etat = INTERMITTENCE;
                break;
                
                
                
           /*
                   
               
                Moteurs::freiner();
                _delay_ms(100);
                Moteurs::avancer();
                ajustementPwmMoteurs(50,50);
                while (!capteur.estPerdu()){
                    capteur.lecture();
                }
                
                Moteurs::freiner();
                _delay_ms(100);
                
                Moteurs::reculer();
              
                switch(cote){
                    case GAUCHE:
                        ajustementPwmMoteurs(60,50);
                        while(!capteur.getSensor(4))
                            capteur.lecture();
                        capteur.tournerDroite();
                        while(!capteur.getSensor(2))
                            capteur.lecture();
                        break;
                    case DROIT:
                        ajustementPwmMoteurs(50,60);
                        while (!capteur.getSensor(0))
                            capteur.lecture();
                        capteur.tournerGauche();
                        while(!capteur.getSensor(2))
                            capteur.lecture();
                        break;
                }                   
                Moteurs::freiner();
                 
                while (!capteur.estPerdu()){
                    capteur.lineTracking();
                }
                _delay_ms(200);
                Moteurs::freiner();
    
                switch(cote){
                    case GAUCHE:
                        capteur.tournerDroite();                //on ajuste l'angle du robot pour qu'il soit face au prochain segment
                        _delay_ms(700);
                        break;
                    case DROIT:
                        capteur.tournerGauche();                //on ajuste l'angle du robot pour qu'il soit face au prochain segment
                        _delay_ms(350);
                        break;
                }
                Moteurs::freiner();
                _delay_ms(100);
                
                Moteurs::avancer();
                ajustementPwmMoteurs(60,60);
                while (capteur.estPerdu())     //le robot n'est pas rendu au segment
                {
                    capteur.lecture();
                }
                                               //le robot a atteint le prochain segment
                if (compteurPhoto < 1)         //doit passer par un autre T ou est rendu au pointillé
                {
                    while (!capteur.estIntersection())
                    {
                        capteur.lineTracking();
                    }
                    Moteurs::freiner();
                    etat = PHOTORESISTANCE;
                    cote = '\0';
                    compteurPhoto++;
                }
                else
                    etat = INTERMITTENCE;
                break;*/
            }

            case INTERMITTENCE:{
                for (uint8_t compteurBlanc = 0; compteurBlanc < 5; compteurBlanc++){
                    while (!capteur.estPerdu())
                    {
                        capteur.lineTracking();
                    }                                   //si compteur = 0, on commence les pointillé
                    ajustementPwmMoteurs(50,50);
                    ajustementPwmPiezo(440);
                    
                    while (capteur.estPerdu())
                        capteur.lecture();
                    
                }
                arretPiezo();
                
                while (!capteur.estIntersection())
                {
                    capteur.lineTracking();
                }
                capteur.intersectionGauche();
                etat = TO_GAH;
                break;
            }
                
           case TO_GAH:
           {
               while (!capteur.estIntersection())
               {
                   capteur.lineTracking();
               }
               capteur.intersectionGauche();
               while (!capteur.estPerdu())
               {
                   capteur.lineTracking();
               }
               ajustementPwmMoteurs(0,0);
               etat = PARKING_2;
               break;
           }
            case PARKING_2:
            {
                while(!(PINC & 0x04)){}
                
                switch(couleurChoisie){
                    case VERT:
                        capteur.tourner180GaucheFinal();
                        Moteurs::reculer();
                        
                        repetitionMinuterie = 0;
                        minuterieActive = true;
                        minuterie(250);
                        ajustementPwmMoteurs(68,60);
                        while (repetitionMinuterie < (100)){}
                        minuterieActive = false;

                        Moteurs::freiner();
                        /*
                        
                        Moteurs::reculer();
                        ajustementPwmMoteurs(70,70);
                        
                        _delay_ms(500);
                        Moteurs::freiner();
                        */
                        break;
                    
                    case ROUGE:
                        capteur.tourner180DroiteFinal();
                        Moteurs::reculer();
                        
                        repetitionMinuterie = 0;
                        minuterieActive = true;
                        minuterie(250);
                        ajustementPwmMoteurs(72,65);
                        while (repetitionMinuterie < (85)){}
                        minuterieActive = false;

                        Moteurs::freiner();
                        
                        capteur.tournerGauche();
                        _delay_ms(700);
                        Moteurs::freiner();
                        

                        break;
                     
        
                while(1){};
                }
                
            }
            
        }

    }
}

