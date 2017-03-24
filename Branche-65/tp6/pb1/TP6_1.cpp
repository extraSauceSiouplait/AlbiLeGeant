#define F_CPU 8000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "memoire_24.h"
 

int main()
{
  DDRC = 0xFF; //OUT
  DDRD = 0x00; //IN
 

  Memoire24CXXX mem = Memoire24CXXX(); 
  const char Tableau[] = "ECOLE POLYTECHNIQUE\0";
  const int16_t adresseInitiale = 0x00;
  
  int16_t adresse = adresseInitiale;
  for(const char *i = Tableau; *i <='\0' ; i++, adresse++)
  {
    //PORTC= 0x02;
    //for(;;){}
    mem.ecriture(adresse, *i);
    _delay_ms(5);
  }
  
	

  // couleurDel(vert); 
  
   

  adresse = adresseInitiale;
  uint8_t * donneelue;
  bool resultat = false;
  for(const char *j = Tableau; *j<= '\0'; j++, adresse++)
  {
    mem.lecture(adresse, donneelue);
    if((char) *donneelue == *j){
	  resultat = true;
      PORTC = 0x02;
      _delay_ms(3000);
      break;
    }
    else {
		resultat = false;
	}
  }
  if (resultat){
	  PORTC = 0x01;
  }
  else
  {
	  PORTC= 0x02;
  }
  _delay_ms(3000);
  PORTC = 0x00;
  return 0;
}

