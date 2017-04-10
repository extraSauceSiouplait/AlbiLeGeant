#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay_basic.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "librairieFonctions.h"
#include "memoire_24.h"
#include "capteurs.h"
#define VERT 'v'
#define ROUGE 'r'

void main {
  DDRD = 0xFF;
  initialisationPwmMoteurs();
  ajustementPwmMoteurs(90,90);
  Capteurs capteur;







}
