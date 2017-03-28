#include Capteurs.h

//Constructeur par défaut
//Initialise la lecture des capteurs sur le port A
Capteurs::capteurs() {
	DDRA = 0x00;
	for (int i = 0; i < 5; i++)
		Capteurs[i] = 0;
}

void Capteurs::lecture() {

	for (int i = 0; i < 5; i++)
		capteurs[i] = PORTA[i];
}


void  Capteurs:: lineTracking () {
	
	   if (capteurs[2]==1){
		
		// Le robot doit aller tout droit.
		
		ajustementPwmMoteurs(100, 100);
		
		}
		
		
		else if (capteurs[1]==1){
		
		// Le robot doit tourner a gauche.
		
		ajustementPwmMoteurs(90, 100);
		
		}
		
		else if (capteurs[3]==1){
		
		// Le robot doit tourner a droite.
		
		ajustementPwmMoteurs(100, 90);
		
		}
	
	
	
	
	}
