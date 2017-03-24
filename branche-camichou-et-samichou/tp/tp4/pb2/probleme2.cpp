#define F_CPU 8000000
#include <avr/io.h> 
#include <util/delay_basic.h>
#include <util/delay.h>

//fil orange - signal E.
//fil jaune - signal D.
//B0 = power (0 = off; 1 = on).
//B1 = direction (0 = forwards; 1 = backwards).



void fonctionPWM(int temps, double a, double b);

double frequence = 60;                             
const double temps = 2000;                         // 2000 ms = 2 s.
const double periode = 1000/ frequence;            // periode = 1/frequence * 1000 afin que sa valeur soit en ms.
                                                                                                
const double delayClock = 2000 / periode;           //Permet d'utiliser _delay_loop_2 avec un delai de 
                                                    //8000 cycles (1 ms) peu importe la valeur de B.

int main()
{
    DDRB = 0xff;            //PORT B en mode ecriture.

    enum frequences {
        F0 = 0, F25 = 25, F50 = 50, F75 = 75, F100 = 100, OVER
    };
    
    int etatPresent = F0;
    double a;                                       // Un certain pourcentage de b.
    PORTB |= 1 << 1;    //direction.
    
    for(;;){
        PORTB |= 1 << 1;
        a = 50;
        
        fonctionPWM(temps, a, frequence);
        /*
        switch(etatPresent){
            case F0 :{
                a =  0 * periode;
                fonctionPWM(temps, a, periode);   
                etatPresent = F25;
            } break;
            case F25 :{
                a = periode;                           //petit boost, car 25% est trop faible.
                fonctionPWM(50, a ,periode);        
                a = 0.25 * periode;
                fonctionPWM(temps - 50, a, periode); 
                etatPresent = F50;
            } break;
            case F50 :{
                a = 0.5 * periode;
                
                fonctionPWM(temps, a, periode);   
                etatPresent = F75;
            } break;
            case F75 :{
                a = 0.75 * periode;
                fonctionPWM(temps, a, periode);   
                etatPresent = F100;
            } break;
            case F100 :{
                a = periode;
                fonctionPWM(temps, a, periode);   
                if (frequence == 400){
                    etatPresent = OVER;
                }
                else{
                    etatPresent = F0;
                    frequence = 400;
                }
            } break;
        }
       if (etatPresent == OVER){
           break;
        }
       */
    }
    
    PORTB = 0x0;
    
}

void fonctionPWM(int temps, double pourcentage, double frequence){
   for (int n = 0; n < temps * frequence; n++){
        PORTB |= 1 << 0;                           
        _delay_loop_2((pourcentage/100) * F_CPU/(4*frequence) + 1);             
        PORTB &= (0xff) & (0 << 0);
        _delay_loop_2((F_CPU/(4*frequence)) * (1 - (pourcentage/100)) + 1);    // Lorsque l'argument est 0, c'est la valeur 65 536 
                                                                            // qui est passee a la fonction, d'ou le +1.
    }
}
 
      

