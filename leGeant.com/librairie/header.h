#ifndef F_CPU
#define F_CPU 8000000
#endif //F_CPU


#include "librairieFonctions.h"
#include "memoire_24.h"
#include "capteurs.h"
#include "can.h"
#include <util/delay.h>
#include <util/delay_basic.h>
#include <avr/interrupt.h>

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

