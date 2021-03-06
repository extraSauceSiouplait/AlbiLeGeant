/***********************************************************************************************************
Fichier: TP7_2.cpp
Auteurs: Olivier Boivin et Armand Hyeudip
Description: Permet d'evaluer le niveau de lumiere avec une photoresistance et afficher ce niveau sur une del. 
Date de creation: 27 fevrier 2017
************************************************************************************************************/
#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "can.h"

/* SETUP :
 
	-La sortie du breadboard (bleu) est relier au PA0(0x00)
*/
	void ambree()
 	{
	 	 PORTC = 0x01;
	 	_delay_ms(10);
	 	 PORTC = 0x02;
	 	_delay_ms(10);
 	}
 
 
 

	enum LIGHT { A, B ,C}; //les niveaux de bronzage
	LIGHT level; 
	const uint8_t FAIBLE = 80; 
	const uint8_t MOYEN = 150; 
	const uint8_t FORT= 225; 	

int main() 
{

	DDRA = 0x00; //IN
	DDRC = 0xff; //OUT
	
	
	
	can capteur; 
	uint8_t force; 
	for(;;)
	{
		force = capteur.lecture(0) >> 2; 
		if (force <= FAIBLE) 
			level = A;
		else if ((force <= MOYEN) && (force > FAIBLE )) 
			level = B;
		else if (force >= FORT)
			level = C;
	
		switch (level)
		{
			case A :
				PORTC= 0x01; //VERT
			break;

			case B :
				ambree();
			break;

			case C :
				PORTC=0x02; //ROUGE
			break;
		}
	}
}
