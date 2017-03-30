#define F_CPU 8000000
#include "header.h"

int main(){
    
    can convertisseur;
    DDRA = 0x00;
    uint16_t gauche16bits = 0x10, 
        droit16bits = 0x10;
    uint8_t gauche8bits = 5, 
        droit8bits = 5;
    initialisationUART();
    
    for(uint8_t i = 0; i < 10; i++){
        gauche16bits = convertisseur.lecture(0x40);
        droit16bits = convertisseur.lecture(0x80);
        gauche16bits >>= 2;
        droit16bits >>= 2; 
        gauche8bits = gauche16bits;
        droit8bits = droit16bits;
        
        transmissionUART('1');
        transmissionUART('\t');
        transmissionUART(droit8bits);
        transmissionUART('\t');
        transmissionUART(gauche8bits);
        transmissionUART('\n');
        
        _delay_ms(50);
    }
    
}
