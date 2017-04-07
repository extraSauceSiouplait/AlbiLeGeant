#include "capteurs.h"
#include "librairieFonctions.h"

//Constructeur par défaut
//Initialise la lecture des capteurs sur le port A

bool Capteurs::getSensor(uint8_t indice) {
    return sensors_[indice];
}


Capteurs::Capteurs() {
    DDRA = 0x00; //Port A en entrée pour permettre lecture des capteurs
    for (int i = 0; i < 5; i++)
        sensors_[i] = false;
}

void Capteurs::lecture() {
    for (int i = 0; i < 5; i++)
        sensors_[i] = PINA & (1 << (i+2)); //capteurs sur PINA2 a 7
}

void  Capteurs:: lineTracking() {
	ecrire0('D',2);
	ecrire0('D',3);
						
    if (sensors_[2] || (sensors_[1] && sensors_[3]))			//seulement le sensor du millieu = 1
    {
        // Le robot doit aller tout droit.
        ajustementPwmMoteurs(60,60);
    }

    else if ((sensors_[1]||sensors_[0]) && !(sensors_[3]))		//sensor de gauche et/ou du centre = 1 
    {
        // Le robot doit tourner a gauche.
        ajustementPwmMoteurs(70, 40);
    }

    else if ((sensors_[3]||sensors_[4]) && !(sensors_[1]))
    {
        // Le robot doit tourner a droite.
        ajustementPwmMoteurs(40, 70);
    }

}

void Capteurs::tournerGauche()
{
	ecrire1('D', 2); //On fixe les directions de rotation des roues, afin qu'elles tournent en sens inverse l'une par rapport à l'autre. Ainsi, l'axe de rotation est approximativement le centre du robot.          
    ecrire0('D', 3); 
	ajustementPwmMoteurs(30, 30);	//Débuter rotation vers la gauche du robot 
}
void Capteurs::tournerDroite()
{
	ecrire0('D',2);	//On fixe les directions de rotation des roues, afin qu'elles tournent en sens inverse l'une par rapport à l'autre. Ainsi, l'axe de rotation est approximativement le centre du robot. 
	ecrire1('D',3);
	ajustementPwmMoteurs(30,30);	//Débuter roation vers la droite du robot
	
}
void Capteurs::tourner180Gauche()
{
	tournerGauche(); 
	do{
        
		lecture(); //Acquisition des données en provenance des capteurs
	}while (!estPerdu());//Tourne tant que les capteurs sont actifs, afin de s'assurer que les capteurs quittent la ligne
	
	while (!sensors_[2]) //Continuer de tourner tant que le capteur du milieu n'est pas actif, afin de retrouver la ligne
	{
		lecture(); //Acquisition des données en provenance des capteurs
	} 
	ajustementPwmMoteurs(0,0); //On arrête les moteurs lorsque la présente routine est terminée
}

void Capteurs::tourner180Droite()
{
	tournerDroite(); 
	do
	{
		lecture(); //Acquisition des données en provenance des capteurs
	}while (!estPerdu());//Tourne tant que les capteurs sont actifs, afin de s'assurer que les capteurs quittent la ligne
	
	while (!sensors_[2]) //Continuer de tourner tant que le capteur du milieu n'est pas actif, afin de retrouver la ligne
	{
		lecture(); //Acquisition des données en provenance des capteurs
	} 
	ajustementPwmMoteurs(0,0); //On arrête les moteurs lorsque la présente routine est terminée
}



void Capteurs::intersectionGauche()
{
	ajustementPwmMoteurs(80,80);
	_delay_ms(2000);//MODIFIER VALEUR					//la ligne perpendiculaire au robot croise son axe de rotation
	
	
	tournerGauche();			//debuter rotation
	lecture();					
	while (!sensors_[2])
	{
		lecture();
	}
	ajustementPwmMoteurs(0,0);		//on arrete les moteurs lorsque le robot est parallele a la ligne
	
	
}
void Capteurs::intersectionDroite()
{
	ajustementPwmMoteurs(80,80);
	_delay_ms(2000);					//la ligne perpendiculaire au robot croise son axe de rotation
	
	
	tournerDroite();				//debuter rotation	
	while (!sensors_[2])
	{
		lecture();
	}
	ajustementPwmMoteurs(0,0);		//on arrete les moteurs lorsque le robot est parallele a la ligne
	
	
}
bool Capteurs::estIntersection()
{
	return ((sensors_[0] && sensors_[1] && sensors_[2]) || (sensors_[2] && sensors_[3] && sensors_[4]));
				//vrai si tous les capteurs de gauche ou/et droite sont actifs
}


bool Capteurs::estPerdu()
{
    return (( !sensors_[0] || !sensors_[1] || !sensors_[2] || !sensors_[3] || !sensors_[4]));
}
