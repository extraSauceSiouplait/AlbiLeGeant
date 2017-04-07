#define F_CPU 8000000
#include <avr/io.h> 
#include <util/delay_basic.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "librairieFonctions.h"
#include "memoire_24.h"

void tournerGauche();

int main()
{
  enum Etats {INIT,E1};
   Etats etatPresent = INIT;
  bool estFini = false;
  while (!estFini)
  {
        switch(etatPresent)
        {
            case INIT:
            {
				tournerGauche();
					while (PORTC && 0x1F != 0x00){}		//il y a des capteurs actifs
				etatPresent = E1;
			}
			case E1:
			{
				while (PORTC && 0x04 != 0x04){}			//capteur 3 est actif
				ajustementPwmTimer(0,0);				//arret des moteurs`
				estFini = true;
			}			         
            
        }
    
    }
  
    
    
}
void tournerGauche()
{
	 ajustementPwmTimer(180, 180);       
            ecrire1('D', 2);              
            ecrire0('D', 3);              //debuter rotation gauche du robot                                     
        
}

 
      

