/********************************************
* Titre: Librairie test 1 en-tête
* Date: Début le 5 mars 2017
* Auteurs: William Harvey, Mathieu Bélanger, Samuel Meilleur, 
* Camille Bergeron, Olivier Boivin, Armand Hyeudip
*******************************************/

#include <avr/interrupt.h>
#include "memoire_24.h"


//************FONCTIONS************//

// Test antirebond pour le boutonPoussoir
// Retourne l'état
bool verifierRebondMecanique();

// Configure les registres d'initialisation du timer1 pour le PWM moteur.
void initialisationPwmMoteurs();

// Ajuster les registres OCR1A et OCR1B pour ajuster le PWM du moteur.
void ajustementPwmMoteurs(uint8_t pourcentageA, uint8_t pourcentageB);

void ajustementPwmFrequence(double frequence);

// Initialise external interrupt request 0 
void initialisationINT0(bool modeBit1, bool modeBit0);
    
// Initialise external interrupt request 1 
void initialisationINT1(bool modeBit1, bool modeBit0);

// Initialise external interrupt request 2 
void initialisationINT2(bool modeBit1, bool modeBit0);

// Interrupt lorsque le timer atteint la valeur voulue
void minuterie(uint16_t duree);

// Ecrire 1 sur une broche quelconque d'un port quelconque
void ecrire1(char port, int broche);

// Ecrire 0 sur une broche quelconque d'un port quelconque
void ecrire0(char port, int broche);

// Initialise le UART pour recevoir et transmettre des donnees
void initialisationUART();

// Transmet jusqu'au port USB la donnee en parametre.
void transmissionUART(uint8_t donnee);

// Recois une donnee par le port USB et la retourne.
unsigned char receptionUART(void);

// Lis chaque byte en memoire a partir de adresseDebut jusqu'a adresseFin et les transmet au port USB avec le UART.
void readMemoryUART(uint16_t adresseDebut, uint16_t adresseFin, uint8_t* donnee, Memoire24CXXX&);

// Switch-case pour le byteCode (tp9)
void decodeurByteCode(uint8_t instruction,uint8_t operande, uint8_t& adresse, bool estDbt, bool estFini);

// Switch-case pour toutes les frequences de son pour le byteCode (tp9)
void jouerSonorite(uint8_t operande);


