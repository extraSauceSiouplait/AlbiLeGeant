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

// Ajuster le PWM du moteur avec les délais du processeur
void ajustementPwmProcesseur(int duree, double pourcentage, double frequence);

// Ajuster le PWM du moteur avec le compteur1.
void ajustementPwmTimer(uint8_t pourcentageA, uint8_t pourcentageB);

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

void initialisationUART();

void transmissionUART(uint8_t donnee);

unsigned char receptionUART(void);

void readMemoryUART(uint16_t adresseDebut, uint16_t adresseFin, uint8_t* donnee, Memoire24CXXX&);

void decodeurByteCode(uint8_t instruction,uint8_t operande, uint8_t& adresse, bool estDbt, bool estFini);

void jouerSonorite(uint8_t operande);


