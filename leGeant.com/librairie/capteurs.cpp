#include "capteurs.h"

//Constructeur par défaut
//Initialise la lecture des capteurs sur le port A
Capteurs::Capteurs() {
    DDRA = 0x00;
    for (int i = 0; i < 5; i++)
        sensors[i] = false;
}

void Capteurs::lecture() {
    for (int i = 0; i < 5; i++)
        sensors[i] = PINA & (1 << i);
}

void  Capteurs:: lineTracking () {
    if (sensors[2] == true){
        // Le robot doit aller tout droit.
        ajustementPwmMoteurs(100, 100);
    }

    else if (sensors[1] == true){
        // Le robot doit tourner a gauche.
        ajustementPwmMoteurs(90, 100);
    }

    else if (sensors[3] == true){
        // Le robot doit tourner a droite.
        ajustementPwmMoteurs(100, 90);
    }

}
