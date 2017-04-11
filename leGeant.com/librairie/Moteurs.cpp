#define F_CPU 8000000

#include "header.h"

void Moteurs::setDirections(bool droit, bool gauche){
    directionDroit_ = droit;
    directionGauche_ = gauche;    
}

void Moteurs::setPourcentages(uint8_t droit, uint8_t gauche){
    pourcentageDroit_ = droit;
    pourcentageGauche_ = gauche;
}

void Moteurs::ecrire(){
    PORTD |= (directionDroit_ << 3) | (directionGauche_ << 2);
    PORTD &= ~(!directionDroit_ << 3) & ~(!directionGauche_ << 2);
}

void Moteurs::reculer(){
    setDirections(true, true);  
    ecrire();
}

void Moteurs::avancer(){
    setDirections(false, false);
    ecrire();
}

void Moteurs::tournerDroit(){
    setDirections(false, true);
    ecrire();
}

void Moteurs::tournerGauche(){
    setDirections(true, false);
    ecrire();
}
