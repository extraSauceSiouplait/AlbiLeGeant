#ifndef CAPTEURS_H
#define CAPTEURS_H

#define F_CPU 8000000

#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>


class Capteurs {
    
public:
    Capteurs();
	void lecture();
	void  lineTracking();  // Methode pour faire avancer le robot sur son parcours.
    
private:
    bool sensors[5];
    
};


#endif
