#define F_CPU 8000000

#include "header.h"

/*************** AJUSTEMENT DES DIRECTION ***************/
void Moteurs::setDirections(bool droit, bool gauche){
    directionDroit_ = droit;
    directionGauche_ = gauche;  
    ecrire();
}

void Moteurs::reculer(){
    setDirections(true, true);  
}

void Moteurs::avancer(){
    setDirections(false, false);
}

void Moteurs::directionDroite(){
    setDirections(true, false);
}

void Moteurs::directionGauche(){
    setDirections(false, true);
}

void Moteurs::ecrire(){
    PORTD |= (directionDroit_ << 3) | (directionGauche_ << 2);
    PORTD &= ~(!directionDroit_ << 3) & ~(!directionGauche_ << 2);
}




/*************** AJUSTEMENT PWM ***************/    
void Moteurs::boost(){
    ajustementPwmMoteurs(90, 90);
    _delay_ms(140);
}    

void Moteurs::freiner(){
    setDirections(!directionDroit_, !directionGauche_); 
    boost();
    setDirections(!directionDroit_, !directionGauche_);
    ajustementPwmMoteurs(0,0);
}    
    
void  Moteurs:: lineTracking() {
    Capteurs::lecture();      //lit la valeur des capteurs.
    avancer();                //set la direction des moteurs pour avancer.
    
    if (Capteurs::getSensor(2) || (Capteurs::getSensor(1) && Capteurs::getSensor(3))){            //seulement le sensor du millieu. Le robot va tout droit.
        ajustementPwmMoteurs(52,48);
    }
    else if ((Capteurs::getSensor(1) || Capteurs::getSensor(0)) && !(Capteurs::getSensor(3))){    //sensors de gauche, mais pas de droite. Le robot doit tourner a gauche.
        ajustementPwmMoteurs(53, 30);
    }
    else if ((Capteurs::getSensor(3) || Capteurs::getSensor(4)) && !(Capteurs::getSensor(1))){    //sensors de droite, mais pas de gauche. Le robot doit tourner a droite.
        // Le robot doit tourner a droite.
        ajustementPwmMoteurs(30, 53);
    }
}

void  Moteurs::lineTrackingTranquille() {  
    Capteurs::lecture();
    avancer();
    if (Capteurs::getSensor(0)){      
        // Le robot doit aller a gauche toute.
        ajustementPwmMoteurs(70,35);
    }
     else if(Capteurs::getSensor(4)) {
        //Le robot doit aller a droite toute!
        ajustementPwmMoteurs(35,70);
    }   
    else if (Capteurs::getSensor(1)){   
        // Le robot doit tourner un peu a gauche.
        ajustementPwmMoteurs(55, 30);
    }
    else if(Capteurs::getSensor(3)) {
        //Le robot doit tourner un peu a droite
        ajustementPwmMoteurs(30,55);
    }
    else if (Capteurs::getSensor(2)){
        // Le robot va tout droit.
        ajustementPwmMoteurs(50, 50);
    }
}

void Moteurs::tournerGauche(){
    directionGauche();
    boost();
    ajustementPwmMoteurs(42, 42);   //Débuter rotation vers la gauche du robot
}

void Moteurs::tournerDroite(){
    directionDroite();
    boost();
    ajustementPwmMoteurs(42, 42);	//Débuter roation vers la droite du robot
}

void Moteurs::intersectionGauche(){
    ajustementPwmMoteurs(50,50);
    _delay_ms(800);             //la ligne perpendiculaire au robot croise son axe de rotation
    Moteurs::freiner();
    tournerGauche();            //debuter rotation
    Capteurs::lecture();        //pour signaler que les capteurs sont eteints
    while (!Capteurs::getSensor(2)){
        Capteurs::lecture();
    }
    freiner();
}

void Moteurs::intersectionDroite(){
    ajustementPwmMoteurs(50,50);
    _delay_ms(800);              //la ligne perpendiculaire au robot croise son axe de rotation
    Moteurs::freiner();
    tournerDroite();             //debuter rotation
    Capteurs::lecture();         //pour signaler que les capteurs sont eteints
    while (!Capteurs::getSensor(2)){
        Capteurs::lecture();
    }
    freiner();
}

void Moteurs::attendreIntersection(){
    while(!Capteurs::estIntersection())
        lineTracking();
}

void Moteurs::tourner180Gauche(){
    tournerGauche();
    do {
        Capteurs::lecture();            //Acquisition des données en provenance des capteurs
    }  while (!Capteurs::estPerdu());   //Tourne tant que les capteurs sont actifs, afin de s'assurer que les capteurs quittent la ligne
    while (!Capteurs::getSensor(2)) {   //Continuer de tourner tant que le capteur du milieu n'est pas actif, afin de retrouver la ligne
        Capteurs::lecture();            //Acquisition des données en provenance des capteurs
    }
    freiner();
}

void Moteurs::tourner180Droite(){
    tournerDroite();
    do {
        Capteurs::lecture();            //Acquisition des données en provenance des capteurs
    } while (!Capteurs::estPerdu());    //Tourne tant que les capteurs sont actifs, afin de s'assurer que les capteurs quittent la ligne

    while (!Capteurs::getSensor(2)) {   //Continuer de tourner tant que le capteur du milieu n'est pas actif, afin de retrouver la ligne
        Capteurs::lecture();            //Acquisition des données en provenance des capteurs
    }
    freiner();
}

void Moteurs::tourner180GaucheFinal(){
    tournerGauche();
    while (!Capteurs::getSensor(4)) //Continuer de tourner tant que le capteur du milieu n'est pas actif, afin de retrouver la ligne
        Capteurs::lecture();        //Acquisition des données en provenance des capteurs
    while (Capteurs::getSensor(4)) 
        Capteurs::lecture();
    _delay_ms(200);

}

void Moteurs::tourner180DroiteFinal(){
    tournerDroite();
    while (!Capteurs::getSensor(0)) //Continuer de tourner tant que le capteur du milieu n'est pas actif, afin de retrouver la ligne
        Capteurs::lecture();        //Acquisition des données en provenance des capteurs
    while (Capteurs::getSensor(0)) 
        Capteurs::lecture();
    freiner();
}

void Moteurs::tournerGaucheTranquille(){
    directionGauche();
    boost();
    ajustementPwmMoteurs(45, 45);	//Débuter rotation vers la gauche du robot
    
}

//initialisation des attirbuts statiques de la classe moteurs a l'exterieur du main
bool Moteurs::directionDroit_;
bool Moteurs::directionGauche_;
