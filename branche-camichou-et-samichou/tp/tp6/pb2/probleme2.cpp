/*
 * Titre:          TP6 - probleme1.cpp
 * Auteurs :       Camille Bergeron-Miron et Samuel Meilleur 
 */

#define F_CPU 8000000

#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>


#define ROUGE 0x01
#define VERT 0x02

// THE POWER OF THE SCARF FLOWS THROUGH ME !!!!!!!!!


void initialisationUART ( void );
void transmissionUART ( uint8_t donnee );

int main()
{
    initialisationUART();
    char mots[21] = "Le robot en INF1995\n";

    uint8_t i, j;

    for ( i = 0; i < 10; i++ ) {

        for ( j=0; j < 20; j++ ) {

            transmissionUART ( mots[j]);

        }

    }
    
    
    
}
    
void initialisationUART ( void ) {

    // 2400 bauds. Nous vous donnons la valeur des deux

    // premier registres pour vous éviter des complications

    UBRR0H = 0;

    UBRR0L = 0xCF;

    // permettre la reception et la transmission par le UART0

    //UCSR0A = 'modifier ici' ;
    
    UCSR0B |= (1 << RXEN0); //Receiver enable.
    UCSR0B |= (1 << TXEN0); //Transmitter enable.
    UCSR0B |= (0 << UCSZ02); //character size 8-bit.

    // Format des trames: 8 bits, 1 stop bits, none parity

    UCSR0C |= (0 << UMSEL01) | (0 << UMSEL00); //asynchronous USART.
    UCSR0C |= (0 << UPM01) | (0 << UPM00);  //parity mode disabled.
    UCSR0C |= (0 << USBS0); //1 stop-bits.
    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00); //character size 8-bit (part 2).

}
void transmissionUART ( uint8_t donnee ) {

    while (!(UCSR0A & (1<<UDRE0))){}
    
    UDR0 = donnee;
    
}

