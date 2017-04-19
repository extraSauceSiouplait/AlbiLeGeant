/*! \file Moteurs.h
 *  \brief Fichier d'en-tête de la classe Moteurs.
 */

#define F_CPU 8000000

#ifndef MOTEURS_H
#define MOTEURS_H


/** \class Moteurs
 *  \brief Classe pour les moteurs du robot.
 * 
 *  Cette classe comprend toutes les fonctions membres qui ajustent la vitesse des moteurs (eg. lineTracking()).
 *  Elle permet aussi de gérer la direction des moteurs et d'écrire leur valeur sur la broche voulue.
 */

class Moteurs{
public:
    Moteurs() {}
    ~Moteurs() {}
    
    /***** AJUSTEMENT DES DIRECTION *****/
    static void setDirections(bool droit, bool gauche);
    static void reculer();
    static void avancer();
    static void directionGauche();
    static void directionDroite();
    static void ecrire();
    
    
    /***** AJUSTEMENT PWM *****/
    static void boost();
    static void freiner();
    static void lineTracking();
    static void lineTrackingExtreme();
    static void tournerGauche();   
    static void tournerDroite();
    static void intersectionDroite();
    static void intersectionGauche();
    static void attendreIntersection();
    static void tourner180Gauche();
    static void tourner180Droite();
    static void tourner180DroiteFinal();
    static void tourner180GaucheFinal();
  
private:
    static bool directionDroit_;   ///< Booléen qui représente la direction du moteur gauche.
    static bool directionGauche_;  ///< Booléen qui représente la direction du moteur droit.
};

#endif //MOTEURS_H



