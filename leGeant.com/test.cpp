

#define F_CPU 8000000
#include "librairie/librairieFonctions.h"
#include "librairie/PWM.h"
#include <util/delay.h>

/*TEST DE LA LIBRAIRIE*/ 
int main()
{	
	
	// Petite routine qui allume la led libre rouge puis une seconde plus tard change sa couleur pour le vert
	DDRA = 0xff;
	ecrire1('A',0);
	_delay_ms(1000);
	ecrire1('A',1);
	ecrire0('A',0);
	return 0;
} 

