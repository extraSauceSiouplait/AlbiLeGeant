#define F_CPU 8000000UL 
#include <util/delay.h>
#include <avr/io.h> 


int main()
{
  DDRA = 0xff; // PORT A est en mode sortie
  uint16_t a = 1000, b = 1000;
  
  while(1) { 
      a = 500;
      b = 500;
        while(a > 0){                                   //Ce cycle dure 1ms
            PORTA = 0x01; //Allume durant le temps déterminé par la boucle ci-dessous
            for (uint16_t i = 0;i < (a); i += 1)
                _delay_us(10);
            
            PORTA = 0x00;   //Est éteint durant le temps déterminé par la boucle ci-dessous
            for (uint16_t i = 0; i < (b-a); i += 1)
                _delay_us(10);
            a -= 1;  // Après 1000ms, a=0
        }
    }
return 0;
}
