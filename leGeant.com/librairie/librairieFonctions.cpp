

#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay_basic.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "header.h"



//************FONCTIONS************//

// Permet l'ajustement de la fréquence pour jouer une sonorité
/**
 * \fn 
 * 
 * \param
 * 
 * \return
 */
void ajustementPwmPiezo(double frequence) {       //TIMER 2 (D7 et D6)
    TCNT0 = 0x00;
    TCCR0A &= ~(1 << COM2A1);                     //toggle OC2A  on compare match A.
    TCCR0A |= (1 << COM2A0);                      //toggle OC2A  on compare match A.
    TCCR0A &= ~(1 << COM2B1) & ~(1 << COM2B0);    //normal port operation, OC2B disconnected.
    TCCR0A |= (1 << WGM21);                       //CTC mode.
    TCCR0A &= ~(1 << WGM20);                      //CTC mode.

    TCCR0B &= ~(1 << WGM22);                      //CTC mode.
    TCCR0B |= (1 << CS22) | (1 << CS20);          //clk/1024 (from prescaler)
    TCCR0B &= ~(1 << CS01);


    OCR0A = ((F_CPU/1024) / (2*frequence));
}

/**
 * \fn 
 * 
 * \param
 * 
 * \return
 */
void arretPiezo() {
    TCCR0A &= ~(1 << COM0A0);            //normal port utilisation
    TCCR0A &= ~(1 << COM0A1);            //normal port utilisation
}


// Configure les registres d'initialisation du timer0 pour le PWM moteur.
/**
 * \fn 
 * 
 * \param
 * 
 * \return
 */
void initialisationPwmMoteurs() {
    cli();

    TCNT1 = 0x0000;
    TCCR1A |= ((1 << COM1A1) | (1 << COM1A0));    //Set output to 1 on compare match A for timer1.
    TCCR1A |= ((1 << COM1B1) | (1 << COM1B0));    //Set output to 1 on compare match B for timer1.
    TCCR1A |= (1 << WGM10);                       //Set to PWM, Phase Correct, 8-bit, TOP 0xff.
    TCCR1B = (1 << CS11) ;                        //division d'horloge par 8 - implique une frequence de PWM fixe.
    TCCR1B &= ~(1 << CS12) & ~(1 << CS10);        //clk/8 (from prescaler)

    TCCR1C = 0;

    sei();
}

/**
 * \fn 
 * 
 * \param
 * 
 * \return
 */
void ajustementPwmMoteurs(float pourcentageA, float pourcentageB) {     //TIMER 0 (B3 et B4)
    OCR1A = 255 * (100.0 - pourcentageA)/100.0;
    OCR1B = 255 * (100.0 - pourcentageB)/100.0;
}

/**
 * \fn 
 * 
 * \param
 * 
 * \return
 */
void initialisationINT0(bool modeBit1, bool modeBit0) {
    cli();          //Interuptions désactivées

    EIMSK |= _BV(INT0);                                         //interrupt sense control p.68
    EICRA |= (modeBit1 << ISC01) | (modeBit0 << ISC00) ;        //met un 1 au bit spécifié si modeBitN = 1 (XX | 10 = 1X) et ne fait rien si modeBitN = 0 (XX | 00 = 00).
    EICRA &= ~(!modeBit1 << ISC01) & ~(!modeBit0 << ISC00);     //met un 0 au bit spécifié si modeBitN = 0 (XXX & 101 = X0X) et ne fait rien si modeBitN = 0 (XXX & 111 = 111).

    sei();          //Interruptions réactivées
}

/**
 * \fn 
 * 
 * \param
 * 
 * \return
 */
void initialisationINT1(bool modeBit1, bool modeBit0) {
    cli();          //Interuptions désactivées

    EIMSK |= _BV(INT1);
    EICRA |= (modeBit1 << ISC11) | (modeBit0 << ISC10) ; //interrupt sense control p.68
    EICRA &= ~(!modeBit1 << ISC11) & ~(!modeBit0 << ISC10);

    sei();          //Interruptions réactivées
}

/**
 * \fn 
 * 
 * \param
 * 
 * \return
 */
void initialisationINT2(bool modeBit1, bool modeBit0) {
    cli();          //Interuptions désactivées

    EIMSK |= _BV(INT2);
    EICRA |= (modeBit1 << ISC21) | (modeBit0 << ISC20) ;    //interrupt sense control p.68
    EICRA &= ~(!modeBit1 << ISC21) & ~(!modeBit0 << ISC20); // EICRA &= ~(~modeBit1 << ISC21) & ~(~modeBit0 << ISC20); Ancienne ligne

    sei();          //Interruptions réactivées
}

/**
 * \fn 
 * 
 * \param
 * 
 * \return
 */
void initialisationPCINT8() {
    cli();

    PCICR |= _BV(PCIE1);
    PCMSK1 |= _BV(PCINT8);

    sei();
}

/**
 * \fn 
 * 
 * \param
 * 
 * \return
 */
void initialisationPCINT12() {
    cli();

    PCICR |= _BV(PCIE1);
    PCMSK1 |= _BV(PCINT12);

    sei();
}

/**
 * \fn 
 * 
 * \param
 * 
 * \return
 */
void initialisationMinuterie() {            //TIMER 2 (8-bits) (D6 ou D7)
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

/**
 * \fn 
 * 
 * \param
 * 
 * \return void
 */
void minuterie(uint8_t duree) {
    cli();

    TCNT2 = 0x00;

    TCCR2A |= ((1 << COM2A1) | (1 << COM2A0));    //Set output to 1 on compare match for timer1.
    TCCR2A &= (~(1 << WGM21) & ~(1 << WGM20));    //Set CTC mode on timer1 1 (part1).

    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);   //clk/1024 from prescaler.
    TCCR2B &= ~(1 << WGM13);
    TCCR2B |= (1 << WGM22);           //Set CTC mode on timer 1 (part2).
    TCCR2B |= (1 << ICES1);           //Event triggered on rising edge.

    TIMSK2 |= (1 << OCIE2A);          //Timer 1, Output compare A match interrupt enable

    OCR2A = duree;

    sei();
}

/**
 * \fn 
 * 
 * \param
 * 
 * \return void
 */
void jouerFrequence (int frequence) {
    int periode = 1000000 / frequence;
    PORTB |= (1 << 1);
    for(int i = 0; i < periode; i += 2)
        _delay_us(1);
    PORTB &= ~(1 << 1);
    for(int i = 0; i < periode; i += 2)
        _delay_us(1);
}

/**
 * \fn 
 * 
 * \param
 * 
 * \return void
 */
void jouerNote(int frequence, float temps) {
    const int DUREE_TEMPS = 520;
    float duree = DUREE_TEMPS * temps;
    duree *= 1000;
    float periode = 1000000 / frequence;
    for(float i = 0; i < duree / periode; i++)
        jouerFrequence(frequence);
}

/*************************************
 * PacMan mesdames et messieurs
 * Author: Tōru Iwatani
 * Interpreted by: William Harvey et Samuel Meilleur
 ***********************************/
/**
 * \fn 
 * 
 * \param
 * 
 * \return 
 */
bool jouerPacMan() {

    const float RONDE = 2;
    const float BLANCHE = RONDE/2;
    const float NOIRE = BLANCHE/2;
    const float CROCHE = NOIRE/2;
    const float DOUBLE_CROCHE = CROCHE/2;
    const float TRIPLE_CROCHE = DOUBLE_CROCHE/2;

    jouerNote(494,DOUBLE_CROCHE);    //Si (0)
    jouerNote(988,DOUBLE_CROCHE);    //Si (+1)
    jouerNote(740,DOUBLE_CROCHE);    //Fa# (+1)
    jouerNote(623,DOUBLE_CROCHE);    //Mib (+1)

    jouerNote(988,TRIPLE_CROCHE);    //Si (+1)
    jouerNote(740,TRIPLE_CROCHE*1.5);    //Fa# (+1)
    jouerNote(622,CROCHE);    //Mib (+1)


    jouerNote(523,DOUBLE_CROCHE);    //Do (+1)
    jouerNote(1046,DOUBLE_CROCHE); //Do (+2)
    jouerNote(784,DOUBLE_CROCHE);    //Sol (+1)
    jouerNote(659,DOUBLE_CROCHE);    //Mi (+1)

    jouerNote(1046,TRIPLE_CROCHE); //Do (+2)
    jouerNote(784,TRIPLE_CROCHE*1.5);    //Sol (+1)
    jouerNote(659,CROCHE);    //Mi (+1)


    jouerNote(494,DOUBLE_CROCHE);    //Si (0)
    jouerNote(990,DOUBLE_CROCHE);    //Si (+1)
    jouerNote(740,DOUBLE_CROCHE);    //Fa# (+1)
    jouerNote(622,DOUBLE_CROCHE);    //Mib (+1)

    jouerNote(988,TRIPLE_CROCHE);    //Si (+1)
    jouerNote(740,TRIPLE_CROCHE*1.5);    //Fa# (+1)
    jouerNote(622,CROCHE);    //Mib (+1)

    jouerNote(622,TRIPLE_CROCHE);    //Mib (+1)
    jouerNote(659,TRIPLE_CROCHE);    //Mi (+1)
    jouerNote(698,DOUBLE_CROCHE*0.9);  //Fa (+1)

    _delay_ms(20); //Silence pour staccato

    jouerNote(698,TRIPLE_CROCHE);  //Fa (+1)
    jouerNote(740,TRIPLE_CROCHE);    //Fa# (+1)
    jouerNote(784,DOUBLE_CROCHE*0.9);    //Sol (+1)

    _delay_ms(20); //Silence pour staccato
    jouerNote(784,TRIPLE_CROCHE);   //Sol (+1)
    jouerNote(830,TRIPLE_CROCHE);   //lab (+1)
    jouerNote(880,DOUBLE_CROCHE);   //la (+1)
    jouerNote(988,DOUBLE_CROCHE);   //la (+1)

    return true; //Retourne un bool pour signifier au robot que la chanson est terminée

}

void jouerMario(){
    const float RONDE = 1.2;
    const float BLANCHE = RONDE/2;
    const float NOIRE = BLANCHE/2;

    jouerNote(1046, BLANCHE * 0.7);
    jouerNote(2, NOIRE);    //silence
    jouerNote(784, BLANCHE * 0.7);
    jouerNote(660, NOIRE * 1.5);
    jouerNote(880, BLANCHE * 0.67);
    jouerNote(988, BLANCHE * 0.67);
    jouerNote(880, BLANCHE * 0.67);
    jouerNote(830, BLANCHE);
    jouerNote(932, BLANCHE);
    jouerNote(830, BLANCHE * 1.2);
    jouerNote(784, RONDE);
}
