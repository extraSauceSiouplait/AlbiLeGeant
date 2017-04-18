#include "header.h"

//Constructeur par défaut
//Initialise la lecture des capteurs sur le port A

//! Constructeur de la classe Capteurs
/*!
 * Initialise la valeur des capteurs en mémoire à 0.
 */
Capteurs::Capteurs() {
    DDRA = 0x00; //Port A en entrée pour permettre lecture des capteurs
    for (int i = 0; i < 5; i++)
        sensors_[i] = false;
}

/**
 * \brief Lit la valeur de chaque capteur sur le port A et stocke la valeur dans l'attribut sensors_.
 */
void Capteurs::lecture() {
    for (int i = 0; i < 5; i++)
        sensors_[i] = (PINA >> (i + 2)) & 0x01;     //lit chaque capteur et store l'etat dans un booleen.   (capteurs sur A2 a A7)
}

/**
 * \brief Chercheur de l'attribut sensors_.
 * \param indice L'indice du capteur voulu dans le tableau.
 * \return La valeur à l'indice voulu dans le tableau sensors_.
 */
bool Capteurs::getSensor(uint8_t indice) {
    return sensors_[indice];
}


 /**
 * \brief Utilisé pour déterminer si les capteurs sont à une intersection
 * \return Si les capteurs détectent une intersection.
 */
bool Capteurs::estIntersection() {
    return ((sensors_[0] && sensors_[1] && sensors_[2]) || (sensors_[2] && sensors_[3] && sensors_[4]));       //vrai si tous les capteurs de gauche ou/et droite sont actifs
}

/**
 * \brief Permet de déterminer si tous les capteurs sont éteints pendant une courte durée de temps.
 * \return Si les capteurs sont éteints.
 */
bool Capteurs::estPerdu() {
    for(uint8_t i = 0; i < 200; i++) {
        if (!(sensors_[0] || sensors_[1] || sensors_[2] || sensors_[3] || sensors_[4])) {
            _delay_ms(1);
            lecture();
        } else
            return false;
    }
    return true;
}

/**
 * \brief Permet de déterminer si tous les capteurs sont éteints pendant une plus longue durée de temps.
 * \return Si les capteurs sont éteints.
 */
bool Capteurs::estPerduLong() {
    for(uint16_t i = 0; i < 297; i++) {
        if (!(sensors_[0] || sensors_[1] || sensors_[2] || sensors_[3] || sensors_[4])) {
            _delay_ms(2);
            lecture();
        } else
            return false;
    }
    return true;
}

////initialisation des attirbuts statiques de la classe Capteurs a l'exterieur du main
bool Capteurs::sensors_[5];





