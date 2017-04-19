
/*! \file Moteurs.cpp
 *  \brief Implémentation des fonctions membre de la classe Moteurs
 */

#define F_CPU 8000000

#include "header.h"

/*************** AJUSTEMENT DES DIRECTION ***************/

/**
 * \brief Mutateur des attributs directionGauche/Droite. Envoie les valeurs aux moteurs.
 * \param gauche direction du moteur gauche (0 = avant, 1 = arrière).
 * \param droit direction du moteur droit (0 = avant, 1 = arrière).
 */
void Moteurs::setDirections(bool droit, bool gauche) {
    directionDroit_ = droit;
    directionGauche_ = gauche;
    ecrire();
}

/**
 * \brief Change la direction des moteurs: mode marche arrière.
 */
void Moteurs::reculer() {
    setDirections(true, true);
}

/** 
 * \brief Change la direction des moteurs: mode marche avant.
 */
void Moteurs::avancer() {
    setDirections(false, false);
}

/**
 * \brief Change la direction des moteurs: mode rotation à Droite.
 */
void Moteurs::directionDroite() {
    setDirections(true, false);
}

/**
 * \brief Change la direction des moteurs: mode rotation à Gauche.
 */
void Moteurs::directionGauche() {
    setDirections(false, true);
}

/** 
 * \brief Écrit sur les broches des moteurs les attributs de classe.
 */
void Moteurs::ecrire() {
    PORTD |= (directionDroit_ << 3) | (directionGauche_ << 2);
    PORTD &= ~(!directionDroit_ << 3) & ~(!directionGauche_ << 2);
}




/*************** AJUSTEMENT PWM ***************/

/**
 * \brief Brise l'inertie d'arrêt des engrenages afin d'activer les roues. Précède les changements de mouvement du robot.
 */
void Moteurs::boost() {
    ajustementPwmMoteurs(90,90);
    _delay_ms(150);
}

/**
 * \brief Brise l'inertie de mouvement des engrenages afin d'arrêter les roues.
 */
void Moteurs::freiner() {
    setDirections(!directionDroit_, !directionGauche_);
    ajustementPwmMoteurs(70,70);
    _delay_ms(100);
    setDirections(!directionDroit_, !directionGauche_);
    ajustementPwmMoteurs(0,0);
}

/**
 * \brief Suit la ligne enregistrée par les capteurs IR.
 */
void  Moteurs:: lineTracking() {
    Capteurs::lecture();      //lit la valeur des capteurs.
    avancer();                //set la direction des moteurs pour avancer.

    if (Capteurs::getSensor(2) || (Capteurs::getSensor(1) && Capteurs::getSensor(3))) {             //seulement le sensor du millieu. Le robot va tout droit.
        ajustementPwmMoteurs(55,50);
    } else if ((Capteurs::getSensor(1) || Capteurs::getSensor(0)) && !(Capteurs::getSensor(3))) {   //sensors de gauche, mais pas de droite. Le robot doit tourner a gauche.
        ajustementPwmMoteurs(52, 30);
    } else if ((Capteurs::getSensor(3) || Capteurs::getSensor(4)) && !(Capteurs::getSensor(1))) {   //sensors de droite, mais pas de gauche. Le robot doit tourner a droite.
        // Le robot doit tourner a droite.
        ajustementPwmMoteurs(35, 53);
    }
}

/**
 * \brief Version plus réactive de lineTracking().
 */
void  Moteurs::lineTrackingExtreme() {
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
 * \brief Ajuste la force des moteurs: virage Gauche.
 */
void Moteurs::tournerGauche() {
    directionGauche();
    boost();
    ajustementPwmMoteurs(42, 42);   //Débuter rotation vers la gauche du robot
}

/**
 * \brief Ajuste la force des moteurs: virage Droite.
 */
void Moteurs::tournerDroite() {
    directionDroite();
    boost();
    ajustementPwmMoteurs(42, 42);   //Débuter roation vers la droite du robot
}

/**
 * \brief  Effectue un virage Gauche dans le cadre d'une intersection.
 */
void Moteurs::intersectionGauche() {
    ajustementPwmMoteurs(50,50);
    _delay_ms(850);             //la ligne perpendiculaire au robot croise son axe de rotation
    Moteurs::freiner();
    tournerGauche();            //debuter rotation
    while(Capteurs::getSensor(2))        //commence a tourner tant que le capteur du mileu est allumé
        Capteurs::lecture();            
    while (!Capteurs::getSensor(2)) {    //continue tant que le capteur du milieu est éteint.
        Capteurs::lecture();
    }
    freiner();
}

/**
 * \brief  Effectue un virage Gauche dans le cadre d'une intersection.
 */
void Moteurs::intersectionDroite() {
    ajustementPwmMoteurs(50,50);
    _delay_ms(820);              //la ligne perpendiculaire au robot croise son axe de rotation
    Moteurs::freiner();
    tournerDroite();             //debuter rotation
    while(Capteurs::getSensor(2))        //commence a tourner tant que le capteur du mileu est allumé
        Capteurs::lecture();            
    while (!Capteurs::getSensor(2)) {    //continue tant que le capteur du milieu est éteint.
        Capteurs::lecture();
    }
    freiner();
}

/**
 * \brief Suit la ligne jusqu'à la prochaine intersection.
 */
void Moteurs::attendreIntersection() {
    while(!Capteurs::estIntersection())
        lineTracking();
}

/**
 * \brief Sur une ligne droite, effectue un virage Gauche de 180 degrés.
 * \sa tournerGauche()
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
 * \brief Sur une ligne droite, effectue un virage à Droite de 180 degrés.
 * \sa tournerDroite()
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
 * \brief Pour l'état PARKING_2, effectue un virage Gauche de 180 degrés.
 * \sa tournerGauche()
 */
void Moteurs::tourner180GaucheFinal() {
    tournerGauche();
    while (!Capteurs::getSensor(4))   //Continuer de tourner tant que le capteur du milieu n'est pas actif, afin de retrouver la ligne
        Capteurs::lecture();          //Acquisition des données en provenance des capteurs
    while (Capteurs::getSensor(4))
        Capteurs::lecture();
    _delay_ms(560);

}

/**
 * \brief Pour l'état PARKING_2, effectue un virage Gauche de 180 degrés.
 * \sa tournerDroite()
 */
void Moteurs::tourner180DroiteFinal() {
    tournerDroite();
    while (!Capteurs::getSensor(0))   //Continuer de tourner tant que le capteur du milieu n'est pas actif, afin de retrouver la ligne
        Capteurs::lecture();          //Acquisition des données en provenance des capteurs
    while (Capteurs::getSensor(0))
        Capteurs::lecture();
    _delay_ms(500);
}


//initialisation des attirbuts statiques de la classe Moteurs a l'exterieur du main
bool Moteurs::directionDroit_;
bool Moteurs::directionGauche_;
