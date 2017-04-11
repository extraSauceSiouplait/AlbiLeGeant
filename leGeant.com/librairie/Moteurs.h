#ifndef MOTEURS_H
#define MOTEURS_H

#define F_CPU 8000000


class Moteurs{
public:
    Moteurs() {}
    ~Moteurs() {}
    static void setDirections(bool droit, bool gauche);
    static void setPourcentages(uint8_t droit, uint8_t gauche);
    static void reculer();
    static void avancer();
    static void tournerGauche();
    static void tournerDroit();
    static void ecrire();
    
private:
    static bool directionDroit_;
    static bool directionGauche_;
    static uint8_t pourcentageDroit_;
    static uint8_t pourcentageGauche_;

};







#endif //MOTEURS_H
