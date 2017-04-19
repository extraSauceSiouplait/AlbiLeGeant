/*! \file capteurs.h
 *  \brief Fichier d'en-tête de la classe Capteurs.
 */

#ifndef CAPTEURS_H
#define CAPTEURS_H

#define F_CPU 8000000

/** \class Capteurs
 *  \brief Classe pour les capteurs du robot.
 * 
 *  Elle permet de lire l'état des 5 capteurs attachés à l'avant du robot pour déterminer l'état physique du robot.
 */

class Capteurs {
    
public:
    Capteurs();
    static void lecture();
    static bool getSensor(uint8_t indice);
    static bool estIntersection();
    static bool estPerdu();
    static bool estPerduLong();
    
private:
    static bool sensors_[5]; //!< Valeur des 5 capteurs du robot stockée dans un tableau de booléen.
};


#endif
