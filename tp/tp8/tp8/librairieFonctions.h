/********************************************
* Titre: Librairie test 1 en-tête
* Date: Début le 5 mars 2017
* Auteurs: William Harvey, Mathieu Bélanger, Samuel Meilleur, 
* Camille Bergeron, Olivier Boivin, Armand Hyeudip
*******************************************/

#include <avr/interrupt.h>


//************FONCTIONS************//

// Test antirebond pour le boutonPoussoir
// Retourne l'état
bool verifierRebondMecanique();

// Ajuster le PWM du moteur avec les délais du processeur
void ajustementPwmProcesseur(int duree, double pourcentage, double frequence);

// Ajuster le PWM du moteur avec le compteur1.
void ajustementPwmMicrocontrolleur(uint8_t pourcentageA, uint8_t pourcentageB);

// Initialise external interrupt request 0 
void initialisationINT0(bool modeBit0, bool modeBit1);
    
// Initialise external interrupt request 1 
void initialisationINT1(bool modeBit0, bool modeBit1);

// Initialise external interrupt request 2 
void initialisationINT2(bool modeBit0, bool modeBit1);

// Declarations des interrupt service resquest pour les differentes interrutpions du microcontrolleur (le code a l'interieur de ces interruptions sera a ecrire dans le futur.)
ISR(INT0_vect);
ISR(INT1_vect);
ISR(INT2_vect);
ISR(TIMER1_COMPA_vect);
ISR(TIMER1_COMPB_vect);

// Interrupt lorsque le timer atteint la valeur voulue
void minuterie(uint16_t duree);

// Ecrire 1 sur une broche quelconque d'un port quelconque
void ecrire1(char port, int broche);

// Ecrire 0 sur une broche quelconque d'un port quelconque
void ecrire0(char port, int broche);

