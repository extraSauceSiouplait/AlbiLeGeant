#define F_CPU 8000000
#include <avr/io.h> 
#include <util/delay_basic.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "PWM.h"


//************CLASSES************//

PWM::PWM(int frequence, int pourcentage, bool marcheArriere){
    frequence_ = frequence;
    pourcentage_ = pourcentage;
    marcheArriere_ = marcheArriere;
}

void PWM::setFrequence(int frequence){
    frequence_ = frequence;
}

void PWM::setPourcentage(int pourcentage){
    pourcentage_ = pourcentage;
}

void PWM::setDirection(bool marcheArriere){
    marcheArriere_ = marcheArriere;
}

