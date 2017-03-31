#ifndef CAPTEURS_H
#define CAPTEURS_H

#define F_CPU 8000000

#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>


class Capteurs {
    
public:
    Capteurs();
	void lecture();		  // Méthode permettant l'acquisition des données en provenance du capteur de ligne
	void lineTracking();  // Methode pour faire avancer le robot sur son parcours.
	void tourner180();	  // Methode pour exécuter le 180 degrés depuis la ligne de départ
	void tournerGauche();
	void tournerDroite();
    
private:
    bool sensors_[5];
    
};


#endif
