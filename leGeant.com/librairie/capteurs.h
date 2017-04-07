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
    void   lineTrackingOptimal();
    void intersectionDroite();	//Methode qui gere une intersection de droite depuis sa detection par les capteurs
    void intersectionGauche();	//Methode qui gere une intersection de gauche depuis sa detection par les capteurs	
    bool estIntersection();		//Methode qui retourne vrai si les capteurs detectent une intersection
    bool getSensor(uint8_t indice);
    bool estPerdu();
    void tourner180Gauche();
    void tourner180Droite();
private:
    bool sensors_[5];
   // bool oldLigne_[5];
   // bool newLigne_[5];

    
};


#endif
