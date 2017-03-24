//Etat present             PIND               Etat Suivant                 PORTC
//      A                   0                      A                         2
//      A                   1                      B                     1 PUIS 2
      
//      B                   0                      C                         1
//      B                   1                      B                      1 PUIS 2
      
//      C                   0                      C                         1
//      C                   1                      D                         2
     
//      D                   0                      E                         0
//      D                   1                      D                         2

//      E                   0                      E                         0
//      E                   1                      F                         1

//      F                   0                      A                         2 
//      F                   1                      F                         1
#define F_CPU 8000000 
#include <util/delay.h>
#include <avr/io.h> 
//2 = rouge
//1 = vert
void ambre()
	{
                _delay_ms(10);	
		PORTC = 0x01;
		_delay_ms(10);
		PORTC = 0x02;
	}
int main()
{
	DDRC = 0xFF; //OUT
	DDRD = 0x00; //IN

	enum State {A,B,C,D,E,F};
	State currentState = A;
	
	for (;;) //INFINITE LOOP
	{
		switch (currentState)
		{
		case A:
			PORTC = 0x02; //RED
			while (PIND & 0x00){}
			_delay_ms(10);
			if (PIND & 0x04)
			{
				currentState = B;
			}
					
			
			break;
		
		case B:
			while (PIND & 0x04)
			{
				ambre();
			}
			_delay_ms(10);
			if (!(PIND & 0x04))
			{
				currentState = C;
			}
			break;
		
		case C:
			PORTC = 0x01;//GREEN
			
			
			while (!(PIND & 0x04)){}
			
			
			
			_delay_ms(10);
			if (PIND & 0x04)
			{
				currentState = D;
			}
			break;
		
		case D:
			PORTC = 0x02; //RED			
			while (PIND & 0x04){}
			_delay_ms(10);
			if (!(PIND & 0x04))
			{
				currentState = E;
			}
			break;
		
		case E:
			PORTC = 0x00;
			while (!(PIND & 0x04)){}
			_delay_ms(10);
			if (PIND & 0x04)
			{
				currentState = F;
			}							
			break;
		case F:	
			PORTC = 0x01;//GREEN
			while (PIND & 0x04) {}
			_delay_ms(10);
			if (!(PIND & 0x04))
			{
				currentState = A;
			}
			break;		
		}
	}
}
