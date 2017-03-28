#define F_CPU 8000000

#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
class LineTracking : Public Capteurs {

public:
	lineTracking();
	void avancer();

private:
	



}

#endif