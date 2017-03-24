

#define F_CPU 8000000 
#include <util/delay.h>
#include <avr/io.h> 
int main()
{
	DDRC = 0xFF; //OUT
	DDRD = 0x00; //IN

	for (;;)
	{
	 if (PIND & 0x04)
	 {
		 PORTC = 0x01;
	 }
	 else
	 {
		 PORTC= 0x00;
	 }
		
	}
}



	



