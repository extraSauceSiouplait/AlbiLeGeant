#ifndef CAPTEURS_H
#define CAPTEURS_H

#define F_CPU 8000000

#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>


class Capteurs {
    
public:
    Capteurs();
	void   lecture();
	void   lineTracking();  // Methode pour faire avancer le robot sur son parcours.
    void   lineTrackingOptimal();
    
private:
    bool sensors_[5];
    bool oldLigne_[5];
    bool newLigne_[5];
    
};


#endif
