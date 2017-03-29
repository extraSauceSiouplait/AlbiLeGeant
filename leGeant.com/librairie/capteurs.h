#define F_CPU 8000000

#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>

class Capteurs {
    
public:
    capteurs();
	void lecture();
	void  lineTracking();  // Methode pour faire avancer le robot sur son parcours.
    
private:
    bool capteurs[5];
    
}


#endif
