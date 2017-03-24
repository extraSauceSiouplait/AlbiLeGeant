
//Etat present              PIND              Etat Suivant                 PORTC
//      A                   0                      A                         0
//      A                   1                      B                         0
      
//     B                    0                       B                        0
//     B                    1                       C                        0     

//     C                    0                       C                        0
//     C                    1                       D                        0
      
//     D                    0                       D                        0
//     D                    1                       E                        0

 //    E                    0                       E                        0
 //    E                    1                       A                        1

#define F_CPU 8000000 
#include <util/delay.h>
#include <avr/io.h> 
int main()
{
	DDRC = 0xFF; //OUT
	DDRD = 0x00; //IN
	enum State { A, B, C, D, E};

	State currentState = A;
	for (;;)
	{
		if (PIND & 0x04)
			_delay_ms(10);
		if (PIND & 0x04)
		{
			switch (currentState)
			{
			case A:
			        
				currentState = B;				
				break;
			case B:
			        
				currentState = C;
				break;
			case C:			
				currentState = D;
				break;
			case D:
				currentState = E;
				break;
			case E:
			       while (PIND & 0X04) {};			  
			        PORTC = 0x02;
				_delay_ms(1000);
		                PORTC = 0x00;
				currentState = A;
					
				break;
				
			}
			while (PIND & 0X04) {};
			
			
		}
	}
}



	



