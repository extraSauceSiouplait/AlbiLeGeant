/***********************************************************************************************************
Fichier: TB7_1.cpp
Auteurs: Olivier Boivin et Armand Hyeudip
Description: Permet d'evaluer le niveau de lumiere avec une photoresistance et afficher ce niveau sur une del. 
Date de creation: 27 fevrier 2017
************************************************************************************************************/

#define F_CPU 8000000 
#include <util/delay.h>
#include <avr/io.h> 
/* SETUP :
 
	-La sortie du breadboard (bleu) est relier au PD2(0x04)
*/

volatile uint8_t compteur = 0;

//Fonction qui fait clignoter vert pendant 0,5 secondes
void clignoterVert() 
{
	for ( int i=0; i<10; i++)
		{
			PORTC = 0x01; 
			_delay_ms(25);
			PORTC = 0x00;
			_delay_ms(25);
		}
}
//Fonction qui fait clignoter rouge 2 fois par secondes
void clignoterRouge() 
{
	PORTC= 0x02;
					_delay_ms(250);
					PORTC= 0x00;
					_delay_ms(250);
					PORTC= 0x02;
					_delay_ms(250);
					PORTC= 0x00;
					_delay_ms(250);
}

int main()
{
	
	DDRC = 0xFF; //OUT
	DDRD = 0x00; //IN
	
	
	enum state {A,B,C,D};
	state currentState = A;
	
	switch(currentState)
	{
		case A: 
				compteur = 0;
				while(PIND&0x04){}
				currentState = B;
				
		case B:
				while (!(PIND&0x04) && compteur < 120)
					{
						_delay_ms(10);
						compteur++;
					}   
			    _delay_ms(5); //antirebond
			    if (PIND & 0x04)
					{
						currentState = C;
					}
			  
		case C: 
				clignoterVert();
				PORTC = 0x00;
				_delay_ms(2000);
				
			
		case D:
				for (int i =0 ; i<= compteur/2; i++)
					{
						clignoterRouge();
					
					}
				PORTC= 0x01;
				_delay_ms(1000);
				PORTC= 0x00;
				break;
				
	}
}






	



