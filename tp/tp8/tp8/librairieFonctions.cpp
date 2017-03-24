

#define F_CPU 8000000
#include <avr/io.h> 
#include <util/delay_basic.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "librairieFonctions.h"
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

void ajustementPwmMicrocontrolleur(uint8_t pourcentageA, uint8_t pourcentageB) {       //PWM avec minuteries (périphériques) - variation de fréquence non-intégré (peut-être non possible)

TCNT1 = 0x0000;      
TCCR1A |= ((1 << COM1A1) | (1 << COM1A0));    //Set output to 1 on compare match A for timer1.
TCCR1A |= ((1 << COM1B1) | (1 << COM1B0));    //Set output to 1 on compare match B for timer1.
TCCR1A |= (1 << WGM10);                       //Set to PWM, Phase Correct, 8-bit, TOP 0xff.
TCCR1B = (1 << CS11); 						  // division d'horloge par 8 - implique une frequence de PWM fixe.
TCCR1C = 0;

OCR1A = 255 * (100 - pourcentageA)/100;
OCR1B = 255 * (100 - pourcentageB)/100;

PORTA = OCR1B;

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
    
    TCCR1A = ((1 << COM1A1) | (1 << COM1A0));    //Set output to 1 on compare match for timer1.
    TCCR1A |= ((0 << WGM11) | (0 << WGM10));     //Set CTC mode on timer1 1 (part1).
    
    TCCR1B = (1 << CS12) | (0 << CS11) | (1 << CS10);   //clk/1024 from prescaler.
    TCCR1B |= (0 << WGM13) | (1 << WGM12);      //Set CTC mode on timer 1 (part2).
    TCCR1B |= (1 << ICES1);      //Event triggered on rising edge.
    
    TCCR1C = 0;
    TIMSK1 = (1 << OCIE1A);     //Timer 1, Output compare A match interrupt enable
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


