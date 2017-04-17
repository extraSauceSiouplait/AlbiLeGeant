#include "header.h"

//Énumération des états de la machine à état
enum Etats {
    COULEUR, UTURN, TO_GA, TO_GAB, COMPTEURLIGNE_1, PARKING_1, TO_ABC, COMPTEURLIGNE_2, ALLERETOUR, CINQ40,INTERSECTION_PHOTO, PHOTORESISTANCE, GUIDAGEFOURCHE , INTERMITTENCE, TO_GAH, PARKING_2
};


volatile uint8_t etat = COULEUR;              //Initialisation de la variable etat à l'état initial.
volatile char couleurChoisie = '\0';          //=null (pas encore choisi)

volatile uint16_t repetitionMinuterie = 0; 
bool minuterieActive = false;   //bool qui indique si la minuterie est activ

float uniteTempsDistance;
uint8_t triggerBonneIntersection;
uint8_t  compteurIntersection2;
char cote;                      //Variable pour le choix du coté lors de l'étape avec les photorésitances.
uint8_t compteurPhoto = 0;


/*
 * Gestion des interruptions: Choix de couleur.
 * 
 * A chaque pression du bouton poussoir, le choix de couleur est inverse 
 * Rouge/Vert
 */
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

/*
 * Gestion des interruptions: Confirmation du choix de couleur.
 * 
 * Si le bouton poussoir de confirmation est enfonce ET qu'une couleur à déjà été choisie,
 * le choix est confirmé et couleurChoisie ne peut plus être changée. 
 */
ISR(PCINT1_vect){
    if(etat == COULEUR){
        if(couleurChoisie){         //Ne fait rien si la couleur n'a pas encore été choisie.
            EIMSK &= ~_BV(INT2);    //INT2 désactivé, le choix de couleur ne peut plus être changé.
            PCMSK1 &= ~_BV(PCINT8); //
            etat = UTURN;
        }
    }
}

/*
 * Gestion des interruptions: Actif si TIMER2 arrive a comparaison
 * 
 * Si TimeR2 arrive a comparaison et que la minuterie est "activée" par l'utilisateur,
 * repetitionMinuterie est incrémentée et on repart la minuterie.
 */
ISR(TIMER2_COMPA_vect) {
    if (minuterieActive){
        repetitionMinuterie++;
        minuterie(250);
    }

}


int main() {
    DDRB = 0x00;       //mode lecture pour lire les interrupts.
    DDRC = 0x03;       //mode ecriture pour la DEL.
    DDRD = 0xFF;       //PORT D en sortie pour le signal des moteurs
    
    //Machine à état décrivant le parcours du robot
    for(;;){
        switch(etat) {
            /*
             * CHOIX DE LA COULEUR  
             */
            case COULEUR: {                 
                initialisationINT2(1,0);    //falling edge activates interrupt.
                initialisationPCINT8();     //pin change interrupt activé sur B0.
                while(etat == COULEUR);     //attend le interrupt pour la confirmation de la couleur.
                break;
            }
            
            /*
             * ROTATION 180 DEGRÉS
             */
            case UTURN:{    
                initialisationPwmMoteurs();
                Moteurs::tourner180Gauche();
                etat = TO_GA;
                break;
            }
            
            /*
             * LINETRACKING JUSQU'À L'INTERSECTION GA
             */
            case TO_GA: {           
                Moteurs::attendreIntersection();
                Moteurs::intersectionGauche();
                etat = TO_GAB;
                break;
            }
            
            /*
             * LINETRACKING JUSQU'À L'INTERSECTION GAB
             */
            case TO_GAB: {           
                Moteurs::attendreIntersection();
                Moteurs::intersectionGauche();
                etat = COMPTEURLIGNE_1;
                break;
            }
            
            /*
             * COMPTE LES INTERSECTIONS AVANT PARKING_1 (VERT OU ROUGE)
             */
            case COMPTEURLIGNE_1: {         
                const float DISTANCE_CM_INTERSECTION = 15.3;
                uint8_t compteurIntersection = 0;
                uint8_t triggerParking = 0;
                
                switch(couleurChoisie){     //Le robot doit se rendre à l'espace de Parking en fonction de couleurChoisie.
                    case VERT: 
                        triggerParking = 3;
                        break;
                    case ROUGE:
                        triggerParking = 6;
                        break;
                }
                
                while (compteurIntersection < triggerParking){ 
                    Moteurs::attendreIntersection();
                    compteurIntersection++;
                    
                    if (compteurIntersection == (triggerParking-2)){
                        minuterieActive = true;
                        minuterie(250);
                    }
                    
                    if (compteurIntersection == triggerParking){
                        minuterieActive = false;
                        uniteTempsDistance = repetitionMinuterie/DISTANCE_CM_INTERSECTION;
                    }
                    
                    while(Capteurs::estIntersection()){
                        Capteurs::lecture();
                        ajustementPwmMoteurs(50,50);
                    }
                }
                etat = PARKING_1;
                break;
            }
            
            /*
             * EFFECTUE LE PARKING DANS LE BON ESPACE
             */
            case PARKING_1: {  
                
                const float DISTANCE_PARKING = 26.0;
                repetitionMinuterie = 0;
                minuterieActive = true;
                minuterie(250);
                while (repetitionMinuterie < (DISTANCE_PARKING * uniteTempsDistance)){
                    Moteurs::lineTracking();
                }
                minuterieActive = false;
                Moteurs::tournerGauche();
                _delay_ms(1300);
                Moteurs::reculer();
                ajustementPwmMoteurs(50,50);
                _delay_ms(1200);
                Moteurs::freiner();            
                
                while(!(PINC & 0x04)){}
                etat = TO_ABC;               //remet en écriture pour les moteurs.
                break;
            }
            
            /*
             * LINETRACKING() JUSQU'A L'INTERSECTION ABC ET TOURNE A GAUCHE SUR LE SEGMENT BC
             */
            case TO_ABC: {  
                Moteurs::tourner180Droite();
                switch(couleurChoisie){
                    case VERT: 
                        triggerBonneIntersection = 4;
                        break;
                    case ROUGE:
                        triggerBonneIntersection = 1;
                        break;
                }
                    
                for (uint8_t i = 0; i < triggerBonneIntersection - 1; i++){
                    Moteurs::attendreIntersection();
                    ajustementPwmMoteurs(50,50); 
                    while (Capteurs::estIntersection()){
                        Capteurs::lecture();
                    }
                }
                
                Moteurs::attendreIntersection();
                Moteurs::intersectionGauche();
                etat = COMPTEURLIGNE_2;
                break;
            }
            
            /*
             * COMPTE LES INTERSECTIONS JUSQUA LA LIGNE SOUHAITÉE + 1 ET EFFECTUE UN VIRAGE GAUCHE SUR LA LIGNE 
             */
            case COMPTEURLIGNE_2: {            
                switch(couleurChoisie){
                    case VERT: 
                        triggerBonneIntersection = 3;
                        break;
                    case ROUGE:
                        triggerBonneIntersection = 5;
                        break;
                }
                
                Moteurs::attendreIntersection();
                compteurIntersection2++;                     //les capteurs detectent une intersection
            
                if(compteurIntersection2 == triggerBonneIntersection) {        //l'axe de rotation du robot est sur la ligne de sa couleur
                
                    Moteurs::tournerGauche();
                    
                    while (!Capteurs::estPerdu()) {       //le capteur du centre est encore sur le segment
                        Capteurs::lecture();
                    } 
                    
                    while (!Capteurs::getSensor(2)){     //le capteur du centre est entre les deux lignes
                        Capteurs::lecture();
                    }
                    Moteurs::freiner();
                    etat = ALLERETOUR;
                }

                while(Capteurs::estIntersection()) {      //le robot est sur l'intersection
                    Capteurs::lecture();                    
                    ajustementPwmMoteurs(50,50); 

                }
                break;

            }
            
            /*
             * EFFECTUE UN ALLERETOUR SUR LA LIGNE 
            */
            case ALLERETOUR: {              
                while (!Capteurs::estPerduLong()){ //tant que la fin de la ligne n'est pas détecté
                    Moteurs::lineTracking();
                }     
                Moteurs::freiner();
                _delay_ms(500);
                
                switch(couleurChoisie){
                    case VERT:
                        Moteurs::tourner180Gauche();   //le robot a fait un 180 sur la ligne
                        break;
                        
                    case ROUGE:
                        Moteurs::tourner180Droite(); 
                        break;
                }
                                    
                Moteurs::attendreIntersection();
                Moteurs::intersectionGauche();

                Capteurs::lecture();
                while(Capteurs::estIntersection()){
                    Capteurs::lecture();
                    ajustementPwmMoteurs(50, 70);
                }
                
                etat = CINQ40;
                break;
            }
            
            /*
             * SUIT LA LIGNE JUSQUA CE QUE LE CENTRE DE ROT. DU ROBOT SOIT AU DESSUS DE C. ROTATE DE 540* VERS LA GAUCHE.
             */
            case CINQ40:{              
                Moteurs::avancer();
                while(!(Capteurs::estPerdu())){       //avance jusqu'a la fin de la ligne.
                    if (Capteurs::estIntersection())
                    {
                        while (Capteurs::estIntersection()){ //On souhaite passer tout droit l'intersection restante
                            ajustementPwmMoteurs(50,50);
                            Capteurs::lecture();
                        }
                    }
                    Moteurs::lineTracking();
                }
                Moteurs::freiner();
                _delay_ms(100);
                
                Moteurs::tournerDroite();
                while (Capteurs::getSensor(2))
                    Capteurs::lecture();
                while (!Capteurs::getSensor(2))
                    Capteurs::lecture();
                ajustementPwmMoteurs(0,0);
                    
                
                
                
                minuterieActive = true;
                repetitionMinuterie = 0;
                minuterie(255);
                while (repetitionMinuterie < 16){
                    Moteurs::lineTracking();
                }
                minuterieActive = false;
                Moteurs::tournerGauche();
                while (!Capteurs::getSensor(4))
                    Capteurs::lecture();
                
                for(uint8_t compteurLignes = 0; compteurLignes < 9; compteurLignes++) {
                    while(Capteurs::getSensor(2))
                        Capteurs::lecture();
                    while(!Capteurs::getSensor(2))
                        Capteurs::lecture();
                }
                 Moteurs::freiner();
                etat = INTERSECTION_PHOTO;
                break;
            }
            
            /*
             * SUIT LA LIGNE JUSQUA L'INTERSECTION EN FOURCHE
             */
            case INTERSECTION_PHOTO:
            {
                
                while (!Capteurs::estIntersection()){
                    Moteurs::lineTrackingTranquille();
                }
                
                Moteurs::intersectionDroite();  
                
                Moteurs::attendreIntersection();
                Moteurs::intersectionGauche();
                
                Moteurs::attendreIntersection();
                
                Moteurs::freiner();
                etat = PHOTORESISTANCE;
                break;
                
            }
            
            /*
             * ATTEND UN SIGNAL LUMINEUX PUIS UNE PRESSION SUR LE BOUTON INTERRUPT 
             */
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
            
            
            /*
             * EMPRUNTE LE CHEMIN GAUCHE OU DROIT EN FONCTION DU COTÉ INDIQUE PAR LE SIGNAL LUMINEUX, 
             */
            case GUIDAGEFOURCHE:
            {
                Moteurs::avancer();
                Moteurs::boost();
                ajustementPwmMoteurs(50,50);
                while(!Capteurs::estPerdu()){
                    Capteurs::lecture();
                }
                Moteurs::freiner();
                _delay_ms(1000);
                
                switch(cote){
                    case GAUCHE:
                        Moteurs::tournerGauche();
                        while(!Capteurs::getSensor(3))
                            Capteurs::lecture();
                        while(!Capteurs::estPerdu()){
                            Moteurs::lineTrackingTranquille();
                        }
                        Moteurs::freiner();
                        
                        Moteurs::tournerDroite();
                        _delay_ms(200);
                        Moteurs::avancer();
                        ajustementPwmMoteurs(50,50);
                        while(Capteurs::estPerdu())
                            Capteurs::lecture();
                            
                        break;
                        
                    case DROIT:
                        Moteurs::tournerDroite();
                        while(!Capteurs::getSensor(1))
                            Capteurs::lecture();
                        while(!Capteurs::estPerdu())
                            Moteurs::lineTrackingTranquille();
                        Moteurs::freiner();
                        Moteurs::tournerGauche();
                        _delay_ms(200);
                        Moteurs::avancer();
                        ajustementPwmMoteurs(50,50);
                        while(Capteurs::estPerdu())
                            Capteurs::lecture();
                        
                        break;
                }
                                                //le robot a atteint le prochain segment
                if (compteurPhoto < 1)           //doit passer par un autre T ou est rendu au pointillé
                {
                    Moteurs::attendreIntersection();
                    Moteurs::freiner();
                    etat = PHOTORESISTANCE;
                    cote = '\0';
                    compteurPhoto++;
                }
                else
                    etat = INTERMITTENCE;
                break;
                
            }

            /*
             * PARCOURS LA LIGNE POINTILLÉ EN EMMETTANT UN BRUIT AIGU
             */
            case INTERMITTENCE:{
                for (uint8_t compteurBlanc = 0; compteurBlanc < 5; compteurBlanc++){
                    Moteurs::attendreIntersection();             //si compteur = 0, on commence les pointillé
                    ajustementPwmMoteurs(50,50);
                    ajustementPwmPiezo(990);
                    
                    while (Capteurs::estPerdu())
                        Capteurs::lecture();
                    
                }
                arretPiezo();
                
                Moteurs::attendreIntersection();
                Moteurs::intersectionGauche();
                etat = TO_GAH;
                break;
            }
                
           /*
            * SUIT LA LIGNE JUSQU'À L'INTERSECTION GAH
            */     
           case TO_GAH:
           {
                Moteurs::attendreIntersection();
                Moteurs::intersectionGauche();
                while (!Capteurs::estPerdu()){
                    Moteurs::lineTracking();
                }
                ajustementPwmMoteurs(0,0);
                etat = PARKING_2;
                break;
           }
           
           /*
            * EFFECTUE LE PARKING FINAL EN FONCTION DE COULEURCHOISIE
            */
            case PARKING_2:
            {
                while(!(PINC & 0x04)){}
                
                switch(couleurChoisie){
                    case VERT:
                        Moteurs::tourner180GaucheFinal();
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
                        Moteurs::tourner180DroiteFinal();
                        Moteurs::reculer();
                        
                        repetitionMinuterie = 0;
                        minuterieActive = true;
                        minuterie(250);
                        ajustementPwmMoteurs(72,65);
                        while (repetitionMinuterie < (85)){}
                        minuterieActive = false;

                        Moteurs::freiner();
                        
                        Moteurs::tournerGauche();
                        _delay_ms(700);
                        Moteurs::freiner();
                        

                        break;
                     
        
                while(1){};
                }
                
            }
            
        }

    }
}

