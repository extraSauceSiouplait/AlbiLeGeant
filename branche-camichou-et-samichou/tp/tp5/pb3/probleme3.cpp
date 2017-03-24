/*
 * Titre:          TP5 - probleme3.cpp
 * Auteurs :       Camille Bergeron-Miron et Samuel Meilleur 
 */

#define F_CPU 8000000
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
/*
 * D4 et D5 signal E.
 * D2 et D3 signal D.
 * E = power (0 = off; 1 = on).
 * D = direction (0 = forwards; 1 = backwards).
*/
void ajustementPWM (uint8_t pourcentage, uint8_t);
uint8_t temps = 2;  //variable en seconde

void ajustementPwmFrequence(double frequence){
	TCNT0 = 0x00;
	TCCR0A &= ~(1 << COM0A0);					  //toggle OC0A  on compare match A for timer0.
	TCCR0A |= (1 << COM1A0);  					  //toggle OC0A  on compare match A for timer0.
	TCCR0A &= ~(1 << COM1B1) & ~(1 << COM1B0);   //normal port operation, OC0B disconnected.
	TCCR0A |= (1 << WGM01);                       //CTC mode.
	TCCR0A &= ~(1 << WGM00);					  //CTC mode.
	
	TCCR0B &= ~(1 << WGM02); 		  			  //CTC mode.
	TCCR0B |= (1 << CS02) | (1 << CS00);		  //clk/1024 (from prescaler)
	TCCR0B &= ~(1 << CS01);		 			  //clk/1024 (from prescaler)
	
	OCR0A = uint8_t((F_CPU/1024) / (2*frequence));
}


int main() {
    
    DDRB = 0xff ;
    //PORTB = 0x00;   
    ajustementPwmFrequence(440);
    /*    
    ajustementPWM(0);
    _delay_ms(2000);
    ajustementPWM(25);
    _delay_ms(2000);
    ajustementPWM(50);
    _delay_ms(2000);
   ajustementPWM(75);
    _delay_ms(2000);
    ajustementPWM(100);
    _delay_ms(2000);
    ajustementPWM(0);
   */
}


void ajustementPWM (uint8_t pourcentageA, uint8_t pourcentageB) {

TCNT1 = 0x0000;      
TCCR1A |= ((1 << COM1A1) | (1 << COM1A0));    //Set output to 1 on compare match A for timer1.
TCCR1A |= ((1 << COM1B1) | (1 << COM1B0));    //Set output to 1 on compare match B for timer1.
TCCR1A |= (1 << WGM10);                       //Set to PWM, Phase Correct, 8-bit, TOP 0xff.
TCCR1B = (1 << CS11) ; // division d'horloge par 8 - implique une frequence de PWM fixe.
TCCR1C = 0;

OCR1A = 255 * (100 - pourcentageA)/100;
OCR1B = 255 * (100 - pourcentageB)/100;

PORTA = OCR1B;

}

