

#define F_CPU 8000000
#include <avr/io.h> 
#include <util/delay_basic.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "librairieFonctions.h"
#include "memoire_24.h"

//************FONCTIONS************//

bool verifierRebondMecanique(){
    DDRD = 0xff;
    if(PIND == 0x04){                    // On appelle la fonction après qu'un signal du bouton-poussoir soit détecté. Elle s'assure que celui-ci (le signal)
        _delay_ms(10);                   //est encore present 10ms plus tard pour etre sur que le contact soit vrai.
       if (PIND == 0x04)
            return true;
    }
    return false;
}

    
void ajustementPwmProcesseur(int temps, double pourcentage, double frequence){	//PWM avec les ports
   for (int n = 0; n < temps * frequence; n++){
        PORTB |= 1 << 0 ; 														//1 sur le bit 0 Enable du moteur   (changer le port et le bit si necessaire).
        _delay_loop_2((pourcentage/100) * F_CPU/(4*frequence) + 1);             
        PORTB &= (0xfe); 														//0 sur le bit 0.
        _delay_loop_2((F_CPU/(4*frequence)) * (1 - (pourcentage/100)) + 1);		//Lorsque l'argument est 0, c'est la valeur 65 536 qui est passee a la fonction, d'ou le +1.
    }
}

void ajustementPwmFrequence(double frequence){
	TCNT0 = 0x00;
	TCCR0A &= ~(1 << COM0A0);					  //toggle OC0A  on compare match A for timer0.
	TCCR0A |= (1 << COM0A0);  					  //toggle OC0A  on compare match A for timer0.
	TCCR0A &= ~(1 << COM0B1) & ~(1 << COM0B0);   //normal port operation, OC0B disconnected.
	TCCR0A |= (1 << WGM01);                       //CTC mode.
	TCCR0A &= ~(1 << WGM00);					  //CTC mode.
	
	TCCR0B &= ~(1 << WGM02); 		  			  //CTC mode.
	TCCR0B |= (1 << CS02) | (1 << CS00);		  //clk/1024 (from prescaler)
	TCCR0B &= ~(1 << CS01);		 			  	  //clk/1024 (from prescaler)
	
	OCR0A = uint8_t((F_CPU/1024) / (2*frequence));
}



void ajustementPwmTimer(uint8_t pourcentageA, uint8_t pourcentageB) {

TCNT1 = 0x0000;      
TCCR1A |= ((1 << COM1A1) | (1 << COM1A0));    //Set output to 1 on compare match A for timer1.
TCCR1A |= ((1 << COM1B1) | (1 << COM1B0));    //Set output to 1 on compare match B for timer1.
TCCR1A |= (1 << WGM10);                       //Set to PWM, Phase Correct, 8-bit, TOP 0xff.
TCCR1B = (1 << CS11) ; // division d'horloge par 8 - implique une frequence de PWM fixe.
TCCR1C = 0;


pourcentageA *= 0.92;
	
OCR1A = 255 * (100 - pourcentageA)/100;
OCR1B = 255 * (100 - pourcentageB)/100;

}

void initialisationINT0(bool modeBit0, bool modeBit1){
    cli();          //Interuptions désactivées
    
    EIMSK |= _BV(INT0);
    EICRA |= (modeBit1 << ISC01) | (modeBit0 << ISC00) ;  //interrupt sense control p.68
    
    sei();          //Interruptions réactivées
    printf ("Characters: %c %c \n", 'a', 65);
}  

void initialisationINT1(bool modeBit0, bool modeBit1){
    cli();          //Interuptions désactivées

    EIMSK |= _BV(INT1);
    EICRA |= (modeBit1 << ISC11) | (modeBit0 << ISC10) ; //interrupt sense control p.68

    
    sei();          //Interruptions réactivées
}  

void initialisationINT2(bool modeBit0, bool modeBit1){
    cli();          //Interuptions désactivées

    EIMSK |= _BV(INT2);
    EICRA |= (modeBit1 << ISC21) | (modeBit0 << ISC20) ;  //interrupt sense control p.68
    
    sei();          //Interruptions réactivées
}  

void minuterie(uint16_t duree){
   // minuterieExpiree = 0;
    TCNT1 = 0x0000;
    OCR1A = duree;
    
    TCCR1A |= ((1 << COM1A1) | (1 << COM1A0));    //Set output to 1 on compare match for timer1.
    TCCR1A &= (~(1 << WGM11) & ~(1 << WGM10));     //Set CTC mode on timer1 1 (part1).
    
    TCCR1B = (1 << CS12) | (0 << CS11) | (1 << CS10);   //clk/1024 from prescaler.
    TCCR1B &= ~(1 << WGM13);
    TCCR1B |= (1 << WGM12);          //Set CTC mode on timer 1 (part2).
    TCCR1B |= (1 << ICES1);      //Event triggered on rising edge.
    
    TCCR1C = 0;
    TIMSK1 |= (1 << OCIE1A);     //Timer 1, Output compare A match interrupt enable
}

ISR(INT0_vect) {}
ISR(INT1_vect) {}
ISR(INT2_vect) {}
ISR(TIMER1_COMPA_vect) {}
ISR(TIMER1_COMPB_vect) {}

void ecrire1(char port, int broche){
    switch (port){
        case 'A':{
            PORTA |= (1 << broche);
        } break;
        case 'B':{
             PORTB |= (1 << broche);
        } break;
        case 'C':{
             PORTC |= (1 << broche);
        } break;
        case 'D':{
             PORTD |= (1 << broche);
        } break;
    }
}

void ecrire0(char port, int broche){
      switch (port){
        case 'A':{
            PORTA &= ~(1 << broche);
        } break;
        case 'B':{
             PORTB &= ~(1 << broche);
        } break;
        case 'C':{
             PORTC &= ~(1 << broche);
        } break;
        case 'D':{
             PORTD &= ~(1 << broche);
        } break;
    }
}

void transmissionUART ( uint8_t donnee ) {

    while (!(UCSR0A & (1<<UDRE0))){}
    
    UDR0 = donnee;
    
}

unsigned char receptionUART(void)
{
/* Wait for data to be received */
while (!(UCSR0A & (1<<RXC0)))
;
/* Get and return received data from buffer */
return UDR0;
}

void initialisationUART () {

    // 2400 bauds. Nous vous donnons la valeur des deux

    // premier registres pour vous éviter des complications

    UBRR0H = 0;

    UBRR0L = 0xCF;

    // permettre la reception et la transmission par le UART0

    UCSR0A &= ~(1 << U2X0);
    
    UCSR0B |= _BV(RXC0) | _BV(TXC0);
    UCSR0B |= (1 << RXEN0); //Receiver enable.
    UCSR0B |= (1 << TXEN0); //Transmitter enable.
    UCSR0B &= ~(1 << UCSZ02); //character size 8-bit.

    // Format des trames: 8 bits, 1 stop bits, none parity

    UCSR0C &= ~(1 << UMSEL01) & ~(1 << UMSEL00); //asynchronous USART.
    UCSR0C &= ~(1 << UPM01) & ~(1 << UPM00);  //parity mode disabled.
    UCSR0C &= (1 << USBS0);        //1 stop-bits.
    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00); //character size 8-bit (part 2).

}

void readMemoryUART(uint16_t adresseDebut, uint16_t adresseFin, uint8_t* donnee, Memoire24CXXX& memoire){
	while (adresseDebut < adresseFin){
		memoire.lecture(adresseDebut++, donnee);
		_delay_ms(5);
		transmissionUART(*donnee);
	}
}

void jouerSonorite(uint8_t operande)  {
    switch(operande) {
        
 
        case 45:
            ajustementPwmFrequence(110);
            break;
        case 46:
            ajustementPwmFrequence( 116.54);
            break;
        case 47:
            ajustementPwmFrequence( 123.471);
            break;
        case 48:
            ajustementPwmFrequence( 130.813);
            break;
        case 49:
            ajustementPwmFrequence( 138.591);
            break;
        case 50:
            ajustementPwmFrequence( 146.832);
            break;
        case 51:
            ajustementPwmFrequence( 155.563);
            break;
        case 52:
            ajustementPwmFrequence( 164.814);
            break;
        case 53:
            ajustementPwmFrequence( 174.614);
            break;
        case 54:
            ajustementPwmFrequence( 185);
            break;
        case 55:
            ajustementPwmFrequence( 196);
            break;
        case 56:
            ajustementPwmFrequence( 207.65);
            break;
        case 57:
            ajustementPwmFrequence( 220);
            break;
        case 58:
            ajustementPwmFrequence( 233.082);
            break;
        case 59:
            ajustementPwmFrequence( 246.942);
            break;
        case 60:
            ajustementPwmFrequence( 261.63);
            break;
        case 61:
            ajustementPwmFrequence( 277.18);
            break;
        case 62:
            ajustementPwmFrequence( 293.66);
            break;
        case 63:
            ajustementPwmFrequence( 311.13);
            break;
        case 64:
            ajustementPwmFrequence(329.627);
            break;
        case 65:
            ajustementPwmFrequence( 349.28);
            break;
        case 66:
            ajustementPwmFrequence( 370);
            break;
        case 67:
            ajustementPwmFrequence( 392);
            break;
        case 68:
            ajustementPwmFrequence( 415.3);
            break;
        case 69:
            ajustementPwmFrequence(440);
            break;
        case 70:
            ajustementPwmFrequence( 466.16);
            break;
        case 71:
            ajustementPwmFrequence( 493.89);
            break;
        case 72:
            ajustementPwmFrequence( 523.25);
            break;
        case 73:
            ajustementPwmFrequence( 554.36);
            break;
        case 74:
            ajustementPwmFrequence( 587.33);
            break;
        case 75:
            ajustementPwmFrequence( 622.26);
            break;
        case 76:
            ajustementPwmFrequence( 659.26);
            break;
        case 77:
            ajustementPwmFrequence( 698.456);
            break;
        case 78:
            ajustementPwmFrequence( 739.99);
            break;
        case 79:
            ajustementPwmFrequence( 784);
            break;
        case 80:
            ajustementPwmFrequence( 830.6);
            break;
        case 81:
            ajustementPwmFrequence( 880);
            break;  
    }
}
