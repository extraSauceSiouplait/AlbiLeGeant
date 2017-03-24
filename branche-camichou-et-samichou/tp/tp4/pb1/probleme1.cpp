#define F_CPU 8000000
#include <avr/io.h> 
#include <util/delay_basic.h>
#include <util/delay.h>


int main()
{
    DDRA = 0xff;            //PORT A en mode ecriture.
    
    for(;;){
        double temps = 3000;            // 3000 ms = 3 s.
        double frequence = 1000;        // b = 1/frequence * 1000 afin que sa valeur soit en ms.
        double b = 1000/ frequence;     
        double a = b;
        PORTA = 0x01;
        for(int n = 0; n = temps; n ++){
            if (a <= 0 ){               // Permet d'assurer que a ne devient jamais negatif.
                break;
            }
            PORTA = 0x01;
            _delay_loop_2(2000*a);
            PORTA = 0x00;
            _delay_loop_2(2000*(b - a) + 1);  // Lorsque l'argument est 0, c'est la valeur 65 536 
                                              // qui est passee a la fonction. 
            a = a - (b / temps);
           
        }
       
        
    }
    
}
 
      

