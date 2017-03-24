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
#include <avr/interrupt.h>: Interrupts
#include <util/delay.h>
#include <avr/io.h> 
//2 = rouge
//1 = vert 

volatile uint8_t currentState = 0;
volatile short prochainState = 0x00;

void Initialisation()
{
	cli();
	
	DDRC = 0xFF; //OUT
	DDRD = 0x00; //IN
	EIMSK |= _BV(INT0);
	EICRA |= 1<<ISC00;

   
	PORTC= 0x02;
	
	sei();
	
}
void ambre()
	{
        _delay_ms(10);	
		PORTC = 0x01;
		_delay_ms(10);
		PORTC = 0x02;
	}
int main()
{
	Initialisation();
	for(; ;)
	{
	
	switch(currentState)
	{
		
		
		case 0:
		 PORTC = 0x02;
		 prochainState = 0x04;
		 break;
		
		case 1:
		 prochainState = 0x00;
		 ambre();
		 //PORTC= 0x00;
		 break;
		
		case 2:
		 PORTC= 0x01;
		 prochainState = 0x04;
		break;
		
		case 3:		
		 PORTC= 0x02;
		 prochainState = 0x00;
		 break;
		
	    
		
		case 4:
		 prochainState = 0x04;
		 PORTC= 0x00;
		 break;
		
		case 5: 
		 prochainState = 0x00;
		 PORTC = 0x01;
		break;
		
		 
	 }
   }
		
		
		
		

	
}

ISR(INT0_vect)
{
	
	_delay_ms(20);
	
	if (currentState == 5)
	{
		currentState = 0;
		 EIFR |= (1 << INTF0) ;
	}
	else 
	{
	  //if (PIND & prochainState)
	    {
	      currentState++;	
	      EIFR |= (1 << INTF0) ;
        }
    }
}

