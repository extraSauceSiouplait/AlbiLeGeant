/*  TP3 - INF1995
 *  Réalisé par Mathieu Bélanger et William Harvey
 *  31/01/2017
*/


#define F_CPU 8000000
#include <util/delay.h>
#include <avr/io.h> 

void ajustementPWM (uint8_t vitesse1, uint8_t vitesse2) {

// mise a un des sorties OC1A et OC1B sur comparaiso
// reussie en mode PWM 8 bits, phase correcte
// et valeur de TOP fixe a 0xFF (mode #1 de la table 16-5
// page 130 de la description technique du ATmega324PA)

TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << WGM10);  //Out =1, lorsque comparaison vraie PWM, //Phase correcte, 8-bits
OCR1A = vitesse1;
OCR1B = vitesse2;

// division d'horloge par 8 - implique une frequence de PWM fixe

TCCR1B |= (1<<CS11) ;
TCCR1C = 0;
}
char caca = 'C'; 
int main()
{   while(caca) {
    DDRD = 0xff;
    
    ajustementPWM(0x40, 0x40);
    _delay_ms(2000);
    ajustementPWM(0x80, 0x80);
    _delay_ms(2000);
    ajustementPWM(0xBF, 0xBF);
    _delay_ms(2000);
    ajustementPWM(0xFF, 0xFF);
    _delay_ms(2000);
    ajustementPWM(0x00, 0x00);
    }
        return 0;
} 
