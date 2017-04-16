

#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay_basic.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "header.h"



//************FONCTIONS************//

/*
bool verifierRebondMecanique(){
    if(PIND == 0x04){                    // On appelle la fonction après qu'un signal du bouton-poussoir soit détecté. Elle s'assure que celui-ci (le signal)
        _delay_ms(10);                   //est encore present 10ms plus tard pour etre sur que le contact soit vrai.
       if (PIND == 0x04)
            return true;
    }
    return false;
}
*/

// Permet l'ajustement de la fréquence pour jouer une sonorité
void ajustementPwmPiezo(double frequence){          //TIMER 2 (D7 et D6)
    TCNT0 = 0x00;
    TCCR0A &= ~(1 << COM2A1);                     //toggle OC2A  on compare match A.
    TCCR0A |= (1 << COM2A0);                      //toggle OC2A  on compare match A.
    TCCR0A &= ~(1 << COM2B1) & ~(1 << COM2B0);    //normal port operation, OC2B disconnected.
    TCCR0A |= (1 << WGM21);                       //CTC mode.
    TCCR0A &= ~(1 << WGM20);                      //CTC mode.

    TCCR0B &= ~(1 << WGM22);                        //CTC mode.
    TCCR0B |= (1 << CS22) | (1 << CS20); //clk/1024 (from prescaler)
    TCCR0B &= ~(1 << CS01);
  

    OCR0A = uint8_t((F_CPU/1024) / (2*frequence));
}

void arretPiezo() {     
    
    TCCR0A &= ~(1 << COM0A0);                    //normal port utilisation
    TCCR0A &= ~(1 << COM0A1);                     //normal port utilisation
}


// Configure les registres d'initialisation du timer0 pour le PWM moteur.
void initialisationPwmMoteurs(){                       
    cli();

    TCNT1 = 0x0000;
    TCCR1A |= ((1 << COM1A1) | (1 << COM1A0));    //Set output to 1 on compare match A for timer1.
    TCCR1A |= ((1 << COM1B1) | (1 << COM1B0));    //Set output to 1 on compare match B for timer1.
    TCCR1A |= (1 << WGM10);                       //Set to PWM, Phase Correct, 8-bit, TOP 0xff.
    TCCR1B = (1 << CS11) ; // division d'horloge par 8 - implique une frequence de PWM fixe.
    TCCR1B &= ~(1 << CS12) & ~(1 << CS10);        //clk/8 (from prescaler)
    
    TCCR1C = 0;

    sei();
}

void ajustementPwmMoteurs(float pourcentageA, float pourcentageB) {     //TIMER 0 (B3 et B4)
    OCR1A = 255 * (100.0 - pourcentageA)/100.0;
    OCR1B = 255 * (100.0 - pourcentageB)/100.0;
}


void initialisationINT0(bool modeBit1, bool modeBit0){
    cli();          //Interuptions désactivées

    EIMSK |= _BV(INT0);                                         //interrupt sense control p.68
    EICRA |= (modeBit1 << ISC01) | (modeBit0 << ISC00) ;        //met un 1 au bit spécifié si modeBitN = 1 (XX | 10 = 1X) et ne fait rien si modeBitN = 0 (XX | 00 = 00).
    EICRA &= ~(!modeBit1 << ISC01) & ~(!modeBit0 << ISC00);     //met un 0 au bit spécifié si modeBitN = 0 (XXX & 101 = X0X) et ne fait rien si modeBitN = 0 (XXX & 111 = 111).

    sei();          //Interruptions réactivées
}

void initialisationINT1(bool modeBit1, bool modeBit0){
    cli();          //Interuptions désactivées

    EIMSK |= _BV(INT1);
    EICRA |= (modeBit1 << ISC11) | (modeBit0 << ISC10) ; //interrupt sense control p.68
    EICRA &= ~(!modeBit1 << ISC11) & ~(!modeBit0 << ISC10);

    sei();          //Interruptions réactivées
}

void initialisationINT2(bool modeBit1, bool modeBit0){
    cli();          //Interuptions désactivées

    EIMSK |= _BV(INT2);
    EICRA |= (modeBit1 << ISC21) | (modeBit0 << ISC20) ;  //interrupt sense control p.68
    EICRA &= ~(!modeBit1 << ISC21) & ~(!modeBit0 << ISC20); // EICRA &= ~(~modeBit1 << ISC21) & ~(~modeBit0 << ISC20); Ancienne ligne

    sei();          //Interruptions réactivées
}

void initialisationPCINT8(){
    cli();
    
    PCICR |= _BV(PCIE1);
    PCMSK1 |= _BV(PCINT8);
    
    sei();
    
}

void initialisationPCINT12(){
    cli();
    
    PCICR |= _BV(PCIE1);
    PCMSK1 |= _BV(PCINT12);
    
    sei();
    
}

void initialisationMinuterie(){             //TIMER 2 (8-bits) (D6 ou D7)
    cli();

    TCNT2 = 0x00;

    TCCR2A |= ((1 << COM2A1) | (1 << COM2A0));    //Set output to 1 on compare match for timer1.
    TCCR2A &= (~(1 << WGM21) & ~(1 << WGM20));    //Set CTC mode on timer1 1 (part1).

    TCCR2B |= (1 << CS22) | (0 << CS21) | (1 << CS20);   //clk/1024 from prescaler.
    TCCR2B &= ~(1 << WGM13);
    TCCR2B |= (1 << WGM22);           //Set CTC mode on timer 1 (part2).
    TCCR2B |= (1 << ICES1);           //Event triggered on rising edge.

    TIMSK2 |= (1 << OCIE2A);          //Timer 1, Output compare A match interrupt enable

    sei();
}

void minuterie(uint8_t duree){
   // minuterieExpiree = 0;
     cli();

    TCNT2 = 0x00;

    TCCR2A |= ((1 << COM2A1) | (1 << COM2A0));    //Set output to 1 on compare match for timer1.
    TCCR2A &= (~(1 << WGM21) & ~(1 << WGM20));    //Set CTC mode on timer1 1 (part1).

    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);   //clk/1024 from prescaler.
    TCCR2B &= ~(1 << WGM13);
    TCCR2B |= (1 << WGM22);           //Set CTC mode on timer 1 (part2).
    TCCR2B |= (1 << ICES1);           //Event triggered on rising edge.

    //TCCR2B = 0;
    TIMSK2 |= (1 << OCIE2A);          //Timer 1, Output compare A match interrupt enable
    
    OCR2A = duree;
    
    sei();
}

