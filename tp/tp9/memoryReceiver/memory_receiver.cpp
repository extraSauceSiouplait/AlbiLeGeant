#define F_CPU 8000000
#include "../header.h"
#include "memoire_24.h"



/*TEST DE LA LIBRAIRIE*/ 


int main()
{	
	
	ecrire1('A',1);
	uint8_t donnee;
	uint16_t addresse = 0x0000;
	Memoire24CXXX mem;
	initialisationUART();
        
	while (1)
	{
		donnee = receptionUART();
		mem.ecriture_delay(addresse,donnee);
		addresse += 0x0001;
	}
} 

/*

uint8_t USART_Receive( void )
{
// Wait for data to be received 
while ( !(UCSR0A & (1<<RXC0)) ); 
// Get and return received data from buffer 
return UDR0;
}
void USART_Init()
{
UBRR0H = 0;

UBRR0L = 0xCF;

// permettre la reception et la transmission par le UART0

UCSR0A &= ~(1<<U2X0);
UCSR0B |= _BV(RXC0) | _BV(TXC0);

UCSR0B |= (1<<TXEN0 )|(1<<RXEN0);

// Format des trames: 8 bits, 1 stop bits, none parity

UCSR0C &= ~(1<<UMSEL00) & ~(1<<UPM00)& ~(1<<UPM01) &~(1<<USBS0)  ;
UCSR0C |= (1 <<UCSZ01) | (1<<UCSZ00);
}

*/
