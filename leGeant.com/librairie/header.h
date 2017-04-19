#ifndef HEADER_H
#define HEADER_H

#include "librairieFonctions.h"
#include "capteurs.h"
#include "can.h"
#include "Moteurs.h"
#include <avr/io.h> 
#include <util/delay.h>
#include <util/delay_basic.h>
#include <avr/interrupt.h>

#define F_CPU 8000000
#define dbt 0x01
#define att 0x02
#define dal 0x44
#define det 0x45
#define sgo 0x48
#define sar 0x09
#define mar 0x60
#define mav 0x62
#define mre 0x63
#define trd 0x64
#define trg 0x65
#define dbc 0xC0
#define fbc 0xC1
#define fin 0xff

#define VERT 'v'    //lors du choix de la couleur
#define ROUGE 'r'   //lors du choix de la couleur
#define GAUCHE 'g'  //lors du choix du coté avec la photorésistance
#define DROIT 'd'   //lors du choix du coté avec la photorésistance

#endif //HEADER_H

