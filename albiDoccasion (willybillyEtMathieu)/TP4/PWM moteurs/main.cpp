#define F_CPU 8000000UL 
#include <util/delay.h>
#include <avr/io.h> 


int main()
{
  DDRB = 0xff; // PORT A est en mode sortie
  uint16_t a = 0, b = 16666;
  
    while(1) { 
      a = 0;
      b = 16666;
        while(a <= 16666){
            for (uint16_t i = 0;i < 120; i++){
               PORTB = 0x01;
               for(uint16_t i =0; i < a; i++){
                   _delay_us(1);
               }
               PORTB = 0x00;
               for (uint16_t i = 0; i < (b-a); i++){
                   _delay_us(1);
               }
              
            }
             a += 4166; 
        }
        
        a = 0, b = 2500;
        
        while(a <= 2500){
            for (uint16_t i = 0;i < 800; i++){
               PORTB = 0x01;
               for(uint16_t i =0; i < a; i++){
                   _delay_us(1);
               }
               PORTB = 0x00;
               for (uint16_t i = 0; i < (b-a); i++){
                   _delay_us(1);
               }
              
            }
             a += 625; 
        }
    }
return 0;
}
