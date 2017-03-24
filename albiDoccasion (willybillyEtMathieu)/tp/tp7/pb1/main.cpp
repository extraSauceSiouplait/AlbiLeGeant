/*  TP7 - INF1995
 *  Réalisé par Mathieu Bélanger et William Harvey
 *  24/02/2017
 */




#define F_CPU 8000000
#include <util/delay.h>
#include <avr/io.h> 
#include <avr/interrupt.h>

volatile uint8_t minuterieDesactive = 0;    //Initialisation de variables globales utiles lors des interruptions
volatile uint8_t boutonPoussoir = 0; 
volatile uint8_t compteur = 0;      //S'incémente à chaque fois que TCNT1 atteint 1//10 de secondes
volatile uint8_t etat = 0;      //variable représentant l'état d'une machine à états finis


//Routine d'interruption du timer
ISR (TIMER1_COMPA_vect) {  
    compteur++;     
    minuterieDesactive = 1; //À chaque fois que la comparaison est vraie on désactive la minuterie
    if(compteur == 120)
        etat++;             //Si le compteur atteint 120, on passe à l'état suivant
}

//Routine d'interruption du bouton poussoir
ISR (INT0_vect) {
    // anti-rebond
    _delay_ms(10);
    
    if(PIND & 0x04)
        boutonPoussoir = 1;
    else {
        boutonPoussoir = 0;
        if(etat == 1)   
            etat++;         //Lorsque le bouton est relâché et qu'on est à l'état 1, on passe à l'état suivant
    }
}

void partirMinuterie ( uint16_t duree ) {       //On configure la minuterie afin que la comparaison est vraie après 0,1 sec

minuterieDesactive = 0;
TCNT1 = 0;
OCR1A = duree;
TCCR1A |= 1 << COM1A0;
TCCR1B |= (1 << CS10) | (1 << CS12); //ClockCPU/1024
TCCR1C = 0;
TIMSK1 |= (1<<OCIE1A);
}

//Fonction d'initialisation des interruptions
void initialisation ( void ) {
    cli();
    EIMSK |= _BV(INT0);
    EICRA |= (1<<ISC00);
    sei();
}


int main()
{
    enum etats {INIT, E1, E2 };
    DDRA = 0xff; //Sortie
    DDRD = 0x00; //Entrée
    initialisation();
    minuterieDesactive = 1;   
    
    while(1) {
    switch(etat)
    {
        case INIT:
            compteur =0;
            PORTA = 0x00;
            
            if(boutonPoussoir)
                etat++;
            break;
            
        case E1:
            if(minuterieDesactive) {        //Si la minuterie est désactivé, on fait partirMinuterie, et ce en boucle, jusqu'à ce que le compteur atteigne 120 ou que le bouton soit relâché. Cela permet de compter de 0 à 0x30D en boucle à chaque fois.
                minuterieDesactive = 0;
                partirMinuterie(0x30D); //Ici la valeur en paramètre est 1/10 de la freq de TCNT1.
            }

            break;
            
        case E2:
            PORTA = 0x02;
            _delay_ms(500);
            PORTA = 0x00;
            _delay_ms(2000);
            for(int i = 0; i < compteur/2 ; i++) {
                PORTA = 0x01;
                _delay_ms(100);
                PORTA = 0x00;
                _delay_ms(400);
            }
            etat=INIT;
    }    

    }
    cli ();

    // Verifier la reponse

    return 0;
} 
