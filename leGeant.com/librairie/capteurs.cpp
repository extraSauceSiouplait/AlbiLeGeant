#include capteurs.h

//Constructeur par défaut
//Initialise la lecture des capteurs sur le port A
capteurs::capteurs() {
	DDRA = 0x00;
	for (int i = 0; i < 5; i++)
		capteurs[i] = 0;
}

void capteurs::lecture() {

	for (int i = 0; i < 5; i++)
		capteurs[i] = PORTA[i];
}
