#define F_CPU 8000000

#include <avr/io.h> 
#include "Moteurs.h"

void Moteurs::setDirections(bool droit, bool gauche){
    directionDroit_ = droit;
    directionGauche_ = gauche;    
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
    setDirections(true, false);
    ecrire();
}

void Moteurs::tournerGauche(){
    setDirections(false, true);
    ecrire();
}

//initialisation des attirbuts statiques de la classe moteurs a l'exterieur du main
bool Moteurs::directionDroit_;
bool Moteurs::directionGauche_;
