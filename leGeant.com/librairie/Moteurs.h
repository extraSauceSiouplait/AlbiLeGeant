#define F_CPU 8000000

#ifndef MOTEURS_H
#define MOTEURS_H

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
    static void lineTrackingTranquille();
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
    static bool directionDroit_;
    static bool directionGauche_;
};

#endif //MOTEURS_H



