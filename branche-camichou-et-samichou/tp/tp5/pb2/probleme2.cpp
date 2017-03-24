/*
 * Titre:          TP5 - probleme2.cpp
 * Auteurs :       Camille Bergeron-Miron et Samuel Meilleur 
 */

#define F_CPU 8000000
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>

void initialisation();
void partirMinuterie(uint16_t);
ISR(INT0_vect);
ISR(TIMER1_COMPA_vect);

volatile uint8_t minuterieExpiree;
volatile uint8_t boutonPoussoir;


int main(){
    initialisation();
    _delay_ms(5000);
    PORTA = 0x01;       //lumiere rouge.
    _delay_ms(100);
    PORTA = 0x00;
    partirMinuterie(7812);
    for (;;){
        if (minuterieExpiree == 1){
            PORTA = 0x01;
        }
        else if (boutonPoussoir == 1){
            PORTA = 0x02;
        }
        else {
            PORTA = 0x00;
        }
    }
    
    
    
}



void initialisation(){
    cli();          //Interuptions désactivées
    DDRA = 0xff;    //PORT A en mode sortie
    PORTA = 0x00;    //lumiere eteinte
    DDRD = 0x00;    //PORT D en mode entree
    boutonPoussoir = 0;
    minuterieExpiree = 1;
    EIMSK |= _BV(INT0);
    EICRA |= 0x01;  //1 sur le bit 0 et 0 sur le bit 1 du registre EICRA (interrupt with rising and falling edge)
    
    sei();          //Interruptions réactivées
}   

void partirMinuterie(uint16_t duree){
    minuterieExpiree = 0;
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


 
ISR(INT0_vect){
    if (minuterieExpiree != 1){
        boutonPoussoir = 1;
        PORTA = 0x02;  //lumiere verte
    }
}

ISR(TIMER1_COMPA_vect){
    if(boutonPoussoir != 1){
        minuterieExpiree = 1;
        PORTA = 0x01;
    }
}
