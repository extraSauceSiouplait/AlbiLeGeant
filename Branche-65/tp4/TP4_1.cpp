#define F_CPU 8000000 
#include <util/delay.h>
#include <avr/io.h> 
int main()
{
	DDRC = 0xFF; //OUT
	DDRD = 0x00; //IN
	
	while(1)
	
	{
	PORTC = 0x03;
	_delay_ms(30);
	PORTC= 0x00;
	_delay_ms(60);
	}
}



	



