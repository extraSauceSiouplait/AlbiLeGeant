

#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay_basic.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "header.h"



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

// Permet l'ajustement de la fréquence pour jouer une sonorité
void ajustementPwmFrequence(double frequence){          //TIMER 2 (D7 et D6)
    TCNT2 = 0x00;
    TCCR2A &= ~(1 << COM2A1);                     //toggle OC2A  on compare match A.
    TCCR2A |= (1 << COM2A0);                      //toggle OC2A  on compare match A.
    TCCR2A &= ~(1 << COM2B1) & ~(1 << COM2B0);    //normal port operation, OC2B disconnected.
    TCCR2A |= (1 << WGM21);                       //CTC mode.
    TCCR2A &= ~(1 << WGM20);                      //CTC mode.

    TCCR2B &= ~(1 << WGM22);                        //CTC mode.
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); //clk/1024 (from prescaler)

    OCR2A = uint8_t((F_CPU/1024) / (2*frequence));
    }

// Configure les registres d'initialisation du timer0 pour le PWM moteur.
void initialisationPwmMoteurs(){                        //TIMER 0 (B3 et B4)
    cli();
/*
    TCNT1 = 0x00;
    TCCR1A |= (1 << COM0A1) | (1 << COM0A0);      //Set output to 1 on compare match A for timer0.
    TCCR1A |= (1 << COM0B1) | (1 << COM0B0);      //Set output to 1 on compare match B for timer0..

    TCCR1A |= (1 << WGM00);                       //PWM, Phase Correct, 8-bit, TOP 0xff
    TCCR1A &= ~(1 << WGM01);                      //PWM, Phase Correct, 8-bit, TOP 0xff
    TCCR1B &= ~(1 << WGM02);                      //PWM, Phase Correct, 8-bit, TOP 0xff

    TCCR1B |= (1 << CS01);                        //clk/8 (from prescaler)
    TCCR1B &= ~(1 << CS02) & ~(1 << CS00);        //clk/8 (from prescaler)

    TCCR1C = 0;
    */

    TCNT1 = 0x0000;
    TCCR1A |= ((1 << COM1A1) | (1 << COM1A0));    //Set output to 1 on compare match A for timer1.
    TCCR1A |= ((1 << COM1B1) | (1 << COM1B0));    //Set output to 1 on compare match B for timer1.
    TCCR1A |= (1 << WGM10);                       //Set to PWM, Phase Correct, 8-bit, TOP 0xff.
    TCCR1B = (1 << CS11) ; // division d'horloge par 8 - implique une frequence de PWM fixe.
    TCCR1C = 0;

    sei();
}

void ajustementPwmMoteurs(float a, float b) {     //TIMER 0 (B3 et B4)
   
  /*  pourcentageA *= 0.7;       //Coefficient de vitesse de la roue gauche (ajustement, afin que les roues tournent à la même vitesse).
    OCR1A = 255 * (100.0 - pourcentageA)/100.0;
    OCR1B = 255 * (100.0 - pourcentageB)/100.0;*/
    //a*= 0.8;       //Coefficient de vitesse de la roue gauche (ajustement, afin que les roues tournent à la même vitesse).
    OCR1A = 255 * (100.0 - a)/100.0;
    OCR1B = 255 * (100.0 - b)/100.0;

}
void ajustementPwmMoteursTEST(float a, float b) {     //TIMER 0 (B3 et B4)
   
    a*= 0.7;       //Coefficient de vitesse de la roue gauche (ajustement, afin que les roues tournent à la même vitesse).
    OCR1A = a;
    OCR1B = b;

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

void initialisationMinuterie(){             //TIMER 2 (8-bits) (D6 ou D7)
    cli();

    TCNT2 = 0x00;

    TCCR2A |= ((1 << COM2A1) | (1 << COM2A0));    //Set output to 1 on compare match for timer1.
    TCCR2A &= (~(1 << WGM21) & ~(1 << WGM20));    //Set CTC mode on timer1 1 (part1).

    TCCR2B |= (1 << CS22) | (0 << CS21) | (1 << CS20);   //clk/1024 from prescaler.
    TCCR2B &= ~(1 << WGM13);
    TCCR2B |= (1 << WGM22);           //Set CTC mode on timer 1 (part2).
    TCCR2B |= (1 << ICES1);           //Event triggered on rising edge.

    //TCCR2B = 0;
    TIMSK2 |= (1 << OCIE2A);          //Timer 1, Output compare A match interrupt enable

    sei();
}

void minuterie(uint8_t duree){
   // minuterieExpiree = 0;
     
     cli();

    TCNT2 = 0x00;

    TCCR2A |= ((1 << COM2A1) | (1 << COM2A0));    //Set output to 1 on compare match for timer1.
    TCCR2A &= (~(1 << WGM21) & ~(1 << WGM20));    //Set CTC mode on timer1 1 (part1).

    TCCR2B |= (1 << CS22) | (0 << CS21) | (1 << CS20);   //clk/1024 from prescaler.
    TCCR2B &= ~(1 << WGM13);
    TCCR2B |= (1 << WGM22);           //Set CTC mode on timer 1 (part2).
    TCCR2B |= (1 << ICES1);           //Event triggered on rising edge.

    //TCCR2B = 0;
    TIMSK2 |= (1 << OCIE2A);          //Timer 1, Output compare A match interrupt enable
    
    OCR2A = duree;
    
    sei();
    
    
}

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

void transmissionUART (uint8_t donnee) {

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

void decodeurByteCode(uint8_t instruction,uint8_t operande, uint8_t& adresse, bool& estDbt, bool& estFini){
    uint16_t adresseBoucle = 0;
    uint8_t compteurBoucle = 0;
    initialisationPwmMoteurs();
    if(!estDbt){
        if(instruction == dbt)
            estDbt = true;
    }
    else{
        switch(instruction)
        {
        case att:
            for(int i = 0; i < 25 * operande; i++){
                _delay_loop_2(F_CPU/(4*1000));           //on ne peut pas faire 25 ms * operande avec _delay_ms, car la valeur doit etre connu a la compilation. _delay_loop_2(parametre) attend 4*parametre cycles du cpu donc ici, il attend 1 ms.
            }
            break;

        case dal:
            PORTA = operande;               //si les pins de PORTA sont les entrées des dels
            break;

        case det:
            PORTA &= ~(operande);       //si les pins de PORTA sont les entrées des dels
            break;

        case sgo:
            TCCR0A &= ~(1 << COM0A1);                     //toggle OC0A  on compare match A for timer0.
            TCCR0A |= (1 << COM0A0);                     //toggle OC0A  on compare match A for timer0.
            jouerSonorite(operande);
            break;

        case sar:
            TCCR0A &= ~(1 << COM0A0);                    //normal port utilisation
            TCCR0A &= ~(1 << COM0A1);                     //normal port utilisation
        break;

        case mar:
            ajustementPwmMoteurs(0,0);        //Arret des moteurs
        break;

        case 0x61:
            ajustementPwmMoteurs(0,0);        //Arret des moteurs
        break;

        case mav:
            ecrire0('D', 2);            //Set direction = 0
            ecrire0('D', 3);
            ajustementPwmMoteurs(100 * operande/255, 100 * operande/255);     //ajustement de vitesse
        break;

        case mre:
            ecrire1('D', 2);        //Set direction = 1;
            ecrire1('D', 3);
            ajustementPwmMoteurs(100*operande/255, 100*operande/255);     //ajustement de vitesse
        break;

        case trd:
            ajustementPwmMoteurs(180, 180);           //debuter rotation droite du robot
            ecrire1('D', 3);
            ecrire0('D', 2);
            _delay_ms(1000);                        //ajuster pour que le robot vire de 90 degres
            ajustementPwmMoteurs(0,0);                //Set vitesse = 0
        break;

        case trg:
            ajustementPwmMoteurs(180, 180);
            ecrire1('D', 2);
            ecrire0('D', 3);              //debuter rotation gauche du robot
            _delay_ms(1000);                  //ajuster pour que le robot vire de 90 degres
            ajustementPwmMoteurs(0,0);            //Set vitesse =0
        break;

        case dbc:
            adresseBoucle = adresse;
            compteurBoucle = operande + 1;                                                 //on enregistre l'addresse du debut de la boucle
        break;

        case fbc:
            if (compteurBoucle > 0)
            {
                compteurBoucle -= 0x01;                                           //on est passer dans boucle donc compteurBoucle--
                adresse = adresseBoucle;                                          //on retourne au debut de la boucle
            }
        break;

        case fin:
            estFini = true;
        break;
        }
    }
}

//Fonction permettant de jouer une sonorité. Prend en parametre une opérande situé entre 45 et 81 décrivant une note. (Voir table associative entre notes et fréquences)
void jouerSonorite(uint8_t operande)  {
    switch(operande) {

        case 45:
            ajustementPwmFrequence(110);
            break;
        case 46:
            ajustementPwmFrequence(116.54);
            break;
        case 47:
            ajustementPwmFrequence(123.471);
            break;
        case 48:
            ajustementPwmFrequence(130.813);
            break;
        case 49:
            ajustementPwmFrequence(138.591);
            break;
        case 50:
            ajustementPwmFrequence(146.832);
            break;
        case 51:
            ajustementPwmFrequence(155.563);
            break;
        case 52:
            ajustementPwmFrequence(164.814);
            break;
        case 53:
            ajustementPwmFrequence(174.614);
            break;
        case 54:
            ajustementPwmFrequence(185);
            break;
        case 55:
            ajustementPwmFrequence(196);
            break;
        case 56:
            ajustementPwmFrequence(207.65);
            break;
        case 57:
            ajustementPwmFrequence(220);
            break;
        case 58:
            ajustementPwmFrequence(233.082);
            break;
        case 59:
            ajustementPwmFrequence(246.942);
            break;
        case 60:
            ajustementPwmFrequence(261.63);
            break;
        case 61:
            ajustementPwmFrequence(277.18);
            break;
        case 62:
            ajustementPwmFrequence(293.66);
            break;
        case 63:
            ajustementPwmFrequence(311.13);
            break;
        case 64:
            ajustementPwmFrequence(329.627);
            break;
        case 65:
            ajustementPwmFrequence(349.28);
            break;
        case 66:
            ajustementPwmFrequence(370);
            break;
        case 67:
            ajustementPwmFrequence(392);
            break;
        case 68:
            ajustementPwmFrequence(415.3);
            break;
        case 69:
            ajustementPwmFrequence(440);
            break;
        case 70:
            ajustementPwmFrequence(466.16);
            break;
        case 71:
            ajustementPwmFrequence(493.89);
            break;
        case 72:
            ajustementPwmFrequence(523.25);
            break;
        case 73:
            ajustementPwmFrequence(554.36);
            break;
        case 74:
            ajustementPwmFrequence(587.33);
            break;
        case 75:
            ajustementPwmFrequence(622.26);
            break;
        case 76:
            ajustementPwmFrequence(659.26);
            break;
        case 77:
            ajustementPwmFrequence(698.456);
            break;
        case 78:
            ajustementPwmFrequence(739.99);
            break;
        case 79:
            ajustementPwmFrequence(784);
            break;
        case 80:
            ajustementPwmFrequence(830.6);
            break;
        case 81:
            ajustementPwmFrequence(880);
            break;
    }
}
