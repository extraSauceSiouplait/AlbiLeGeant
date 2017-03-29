

#define F_CPU 8000000
#include "librairie/librairieFonctions.h"
#include "librairie/PWM.h"
#include <util/delay.h>

/*TEST DE LA LIBRAIRIE*/ 
int main()
{	
	
	// Petite routine qui allume la led libre rouge puis une seconde plus tard change sa couleur pour le vert
	DDRA = 0xff;
    DDRB = 0xff;
    DDRC = 0xff;
    DDRD = 0xff;
	PORTA = 0xff;
    PORTB = 0xff;
    PORTC = 0xff;
    PORTD = 0xff;
	return 0;
} 

