/********************************************
* Titre: Librairie test 1 en-tête
* Date: Début le 5 mars 2017
* Auteurs: William Harvey, Mathieu Bélanger, Samuel Meilleur, 
* Camille Bergeron, Olivier Boivin, Armand Hyeudip
*******************************************/
/*! \file librairieFonctions.h
 *  \brief Fichier d'en-tête des fonctions de la librairie.
 */

#include <avr/interrupt.h>


//************FONCTIONS************//

// Configure les registres d'initialisation du timer0 pour le PWM moteur.
void initialisationPwmMoteurs();

// Ajuste les registres OCR0A et OCR0B pour ajuster le PWM du moteur.
void ajustementPwmMoteurs(float pourcentageA, float pourcentageB);

// Initialise le timer2 en mode pwm (onde carrée) et ajuste la frequence  de celui-ci 
void ajustementPwmPiezo(double frequence);

//Réinitialise les ports du piezo à leurs utilisations normales.
void arretPiezo();

// Initialise external interrupt request 0 
void initialisationINT0(bool modeBit1, bool modeBit0);
    
// Initialise external interrupt request 1 
void initialisationINT1(bool modeBit1, bool modeBit0);

// Initialise external interrupt request 2 
void initialisationINT2(bool modeBit1, bool modeBit0);

//  Initialise le pin-change interrupt sur la broche B0.
void initialisationPCINT8();

// Initialise le timer1 pour generer une interruption on compare match
void initialisationMinuterie();

// reset le compteur et ajuste la valeur de OCR1A qui va declancher une interruption (duree en ms)
void minuterie(uint8_t duree);

void jouerFrequence(int frequence);

void jouerNote(int, float);

bool jouerPacMan();
void jouerMario();


