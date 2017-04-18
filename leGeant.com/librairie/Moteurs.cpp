#define F_CPU 8000000

#include "header.h"

/*************** AJUSTEMENT DES DIRECTION ***************/

/**
 * \fn 
 * 
 * \param
 * 
 * \return
 */
void Moteurs::setDirections(bool droit, bool gauche) {
    directionDroit_ = droit;
    directionGauche_ = gauche;
    ecrire();
}

/**
 * \fn 
 * 
 * \param
 * 
 * \return
 */
void Moteurs::reculer() {
    setDirections(true, true);
}

/**
 * \fn 
 * 
 * \param
 * 
 * \return
 */
void Moteurs::avancer() {
    setDirections(false, false);
}

/**
 * \fn 
 * 
 * \param
 * 
 * \return
 */
void Moteurs::directionDroite() {
    setDirections(true, false);
}

/**
 * \fn 
 * 
 * \param
 * 
 * \return
 */
void Moteurs::directionGauche() {
    setDirections(false, true);
}

/**
 * \fn 
 * 
 * \param
 * 
 * \return
 */
void Moteurs::ecrire() {
    PORTD |= (directionDroit_ << 3) | (directionGauche_ << 2);
    PORTD &= ~(!directionDroit_ << 3) & ~(!directionGauche_ << 2);
}




/*************** AJUSTEMENT PWM ***************/

/**
 * \fn 
 * 
 * \param
 * 
 * \return
 */
void Moteurs::boost() {
    ajustementPwmMoteurs(90,90);
    _delay_ms(150);
}

/**
 * \fn 
 * 
 * \param
 * 
 * \return
 */
void Moteurs::freiner() {
    setDirections(!directionDroit_, !directionGauche_);
    ajustementPwmMoteurs(70,70);
    _delay_ms(100);
    setDirections(!directionDroit_, !directionGauche_);
    ajustementPwmMoteurs(0,0);
}

/**
 * \fn 
 * 
 * \param
 * 
 * \return
 */
void  Moteurs:: lineTracking() {
    Capteurs::lecture();      //lit la valeur des capteurs.
    avancer();                //set la direction des moteurs pour avancer.

    if (Capteurs::getSensor(2) || (Capteurs::getSensor(1) && Capteurs::getSensor(3))) {             //seulement le sensor du millieu. Le robot va tout droit.
        ajustementPwmMoteurs(55,50);
    } else if ((Capteurs::getSensor(1) || Capteurs::getSensor(0)) && !(Capteurs::getSensor(3))) {   //sensors de gauche, mais pas de droite. Le robot doit tourner a gauche.
        ajustementPwmMoteurs(53, 30);
    } else if ((Capteurs::getSensor(3) || Capteurs::getSensor(4)) && !(Capteurs::getSensor(1))) {   //sensors de droite, mais pas de gauche. Le robot doit tourner a droite.
        // Le robot doit tourner a droite.
        ajustementPwmMoteurs(30, 53);
    }
}

/**
 * \fn 
 * 
 * \param
 * 
 * \return
 */
void  Moteurs::lineTrackingTranquille() {
    Capteurs::lecture();
    avancer();
    if (Capteurs::getSensor(0)) {
        // Le robot doit aller a gauche toute.
        ajustementPwmMoteurs(70,30);
    } else if(Capteurs::getSensor(4)) {
        //Le robot doit aller a droite toute!
        ajustementPwmMoteurs(30,70);
    } else if (Capteurs::getSensor(1)) {
        // Le robot doit tourner un peu a gauche.
        ajustementPwmMoteurs(55, 30);
    } else if(Capteurs::getSensor(3)) {
        //Le robot doit tourner un peu a droite
        ajustementPwmMoteurs(30,55);
    } else if (Capteurs::getSensor(2)) {
        // Le robot va tout droit.
        ajustementPwmMoteurs(45, 45);
    }
}

/**
 * \fn 
 * 
 * \param
 * 
 * \return
 */
void Moteurs::tournerGauche() {
    directionGauche();
    boost();
    ajustementPwmMoteurs(42, 42);   //Débuter rotation vers la gauche du robot
}

/**
 * \fn 
 * 
 * \param
 * 
 * \return
 */
void Moteurs::tournerDroite() {
    directionDroite();
    boost();
    ajustementPwmMoteurs(42, 42);   //Débuter roation vers la droite du robot
}

/**
 * \fn 
 * 
 * \param
 * 
 * \return
 */
void Moteurs::intersectionGauche() {
    ajustementPwmMoteurs(50,50);
    _delay_ms(850);             //la ligne perpendiculaire au robot croise son axe de rotation
    Moteurs::freiner();
    tournerGauche();            //debuter rotation
    Capteurs::lecture();        //pour signaler que les capteurs sont eteints
    while (!Capteurs::getSensor(2)) {
        Capteurs::lecture();
    }
    freiner();
}

/**
 * \fn 
 * 
 * \param
 * 
 * \return
 */
void Moteurs::intersectionDroite() {
    ajustementPwmMoteurs(50,50);
    _delay_ms(820);              //la ligne perpendiculaire au robot croise son axe de rotation
    Moteurs::freiner();
    tournerDroite();             //debuter rotation
    Capteurs::lecture();         //pour signaler que les capteurs sont eteints
    while (!Capteurs::getSensor(2)) {
        Capteurs::lecture();
    }
    freiner();
}

/**
 * \fn 
 * 
 * \param
 * 
 * \return
 */
void Moteurs::attendreIntersection() {
    while(!Capteurs::estIntersection())
        lineTracking();
}

/**
 * \fn 
 * 
 * \param
 * 
 * \return
 */
void Moteurs::tourner180Gauche() {
    tournerGauche();
    do {
        Capteurs::lecture();            //Acquisition des données en provenance des capteurs
    }  while (!Capteurs::estPerdu());   //Tourne tant que les capteurs sont actifs, afin de s'assurer que les capteurs quittent la ligne
    while (!Capteurs::getSensor(2)) {   //Continuer de tourner tant que le capteur du milieu n'est pas actif, afin de retrouver la ligne
        Capteurs::lecture();            //Acquisition des données en provenance des capteurs
    }
    freiner();
}

/**
 * \fn 
 * 
 * \param
 * 
 * \return
 */
void Moteurs::tourner180Droite() {
    tournerDroite();
    do {
        Capteurs::lecture();            //Acquisition des données en provenance des capteurs
    } while (!Capteurs::estPerdu());    //Tourne tant que les capteurs sont actifs, afin de s'assurer que les capteurs quittent la ligne

    while (!Capteurs::getSensor(2)) {   //Continuer de tourner tant que le capteur du milieu n'est pas actif, afin de retrouver la ligne
        Capteurs::lecture();            //Acquisition des données en provenance des capteurs
    }
    freiner();
}

/**
 * \fn 
 * 
 * \param
 * 
 * \return
 */
void Moteurs::tourner180GaucheFinal() {
    tournerGauche();
    while (!Capteurs::getSensor(4))   //Continuer de tourner tant que le capteur du milieu n'est pas actif, afin de retrouver la ligne
        Capteurs::lecture();          //Acquisition des données en provenance des capteurs
    while (Capteurs::getSensor(4))
        Capteurs::lecture();
    _delay_ms(200);

}

/**
 * \fn 
 * 
 * \param
 * 
 * \return
 */
void Moteurs::tourner180DroiteFinal() {
    tournerDroite();
    while (!Capteurs::getSensor(0))   //Continuer de tourner tant que le capteur du milieu n'est pas actif, afin de retrouver la ligne
        Capteurs::lecture();          //Acquisition des données en provenance des capteurs
    while (Capteurs::getSensor(0))
        Capteurs::lecture();
    _delay_ms(420);
}


//initialisation des attirbuts statiques de la classe Moteurs a l'exterieur du main
bool Moteurs::directionDroit_;
bool Moteurs::directionGauche_;
