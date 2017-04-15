#define F_CPU 8000000

#include "header.h"

void Moteurs::setDirections(bool droit, bool gauche){
    directionDroit_ = droit;
    directionGauche_ = gauche;  
    ecrire();
}


void Moteurs::ecrire(){
    PORTD |= (directionDroit_ << 3) | (directionGauche_ << 2);
    PORTD &= ~(!directionDroit_ << 3) & ~(!directionGauche_ << 2);
}

void Moteurs::reculer(){
    setDirections(true, true);  
}

void Moteurs::avancer(){
    setDirections(false, false);
}

void Moteurs::freiner(){
    setDirections(!directionDroit_, !directionGauche_); 
    ajustementPwmMoteurs(70,70);
    _delay_ms(70);
    setDirections(!directionDroit_, !directionGauche_);
    ajustementPwmMoteurs(0,0);
}

void Moteurs::tournerDroite(){
    setDirections(true, false);
}

void Moteurs::tournerGauche(){
    setDirections(false, true);
}

//initialisation des attirbuts statiques de la classe moteurs a l'exterieur du main
bool Moteurs::directionDroit_;
bool Moteurs::directionGauche_;
