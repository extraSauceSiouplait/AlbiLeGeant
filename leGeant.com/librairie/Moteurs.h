#define F_CPU 8000000

#ifndef MOTEURS_H
#define MOTEURS_H

class Moteurs{
public:
    Moteurs() {}
    ~Moteurs() {}
    static void setDirections(bool droit, bool gauche);
    static void reculer();
    static void avancer();
    static void freiner();
    static void tournerGauche();
    static void tournerDroite();
    static void ecrire();
    
    
private:
    static bool directionDroit_;
    static bool directionGauche_;
};

#endif //MOTEURS_H



