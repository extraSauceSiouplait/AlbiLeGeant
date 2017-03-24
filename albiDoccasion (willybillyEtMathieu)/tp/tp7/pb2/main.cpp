/*  TP7 - INF1995
 *  Réalisé par Mathieu Bélanger et William Harvey
 *  24/02/2017
 */

#define F_CPU 8000000
#include <util/delay.h>
#include <avr/io.h> 
#include "can.cpp"

int main(){
    const uint8_t FAIBLE = 0, MOYEN = 85, ELEVE = 170;
    uint16_t intensite = 0x0000;
    DDRA = 0x00;
    DDRD = 0xff;
    can convertisseur;
    
    while(1){
        intensite = convertisseur.lecture(0x01);
        intensite >>= 2; //On décale intensite de 2 vers la droite, afin d'enlever les 2 bits les moins significatifs (Ils sont inutiles ici)
        
        if(ELEVE < intensite){
            PORTD = 0x01;
        }
        else if (MOYEN < intensite && intensite <= ELEVE){
            PORTD = 0x02;
            _delay_ms(3);
            PORTD = 0x01;
            _delay_ms(1);
        }
        else if (FAIBLE < intensite && intensite <= MOYEN){
            PORTD = 0x02;
        }
    }
    return 0;
} 
