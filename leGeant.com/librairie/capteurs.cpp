#include "capteurs.h"
#include "librairieFonctions.h"

//Constructeur par défaut
//Initialise la lecture des capteurs sur le port A
Capteurs::Capteurs() {
    DDRA = 0x00; //Port A en entrée pour permettre lecture des capteurs
    for (int i = 0; i < 5; i++)
        sensors_[i] = false;
}

void Capteurs::lecture() {
    for (int i = 0; i < 5; i++)
        sensors_[i] = PINA & (1 << i);
}

void  Capteurs:: lineTracking() {
	ecrire0('D',2);
	ecrire0('D',3);
						
    if (sensors_[2] || (sensors_[1] && sensors_[3]))			//seulement le sensor du millieu = 1
    {
        // Le robot doit aller tout droit.
        ajustementPwmMoteurs(100, 100);
    }

    else if ((sensors_[1]||sensors_[0]) && !(sensors_[3]))		//sensor de gauche et/ou du centre = 1 
    {
        // Le robot doit tourner a gauche.
        ajustementPwmMoteurs(100, 50);
    }

    else if ((sensors_[3]||sensors_[4]) && !(sensors_[1]))
    {
        // Le robot doit tourner a droite.
        ajustementPwmMoteurs(50, 100);
    }
    
   /* else if (sensors_[3] && sensors_[1])			//intersection ?
    {
		
		if (sensors_[4])							//il y a une intersection a gauche
			{
				ajustementPwmMoteurs(20,80);
			}
		else if (sensors_[0])					//il y a une intersection a droite
			{
				ajustementPwmMoteurs(80,20);
			}
		else
			{
				ajustementPwmMoteurs(80,80);    //seulement les 3 sensors du centre sont actifs
			}
	}*/
	

}

void Capteurs::tournerGauche()
{
	ecrire1('D', 2); //On fixe les directions de rotation des roues, afin qu'elles tournent en sens inverse l'une par rapport à l'autre. Ainsi, l'axe de rotation est approximativement le centre du robot.          
    ecrire0('D', 3); 
	ajustementPwmMoteurs(80, 80);	//Débuter rotation vers la gauche du robot 
}
void Capteurs::tournerDroite()
{
	ecrire0('D',2);	//On fixe les directions de rotation des roues, afin qu'elles tournent en sens inverse l'une par rapport à l'autre. Ainsi, l'axe de rotation est approximativement le centre du robot. 
	ecrire1('D',3);
	ajustementPwmMoteurs(80,80);	//Débuter roation vers la droite du robot
	
}
void Capteurs::tourner180()
{
	tournerGauche(); 
	do
	{
		lecture(); //Acquisition des données en provenance des capteurs
	}while (sensors_[0] || sensors_[1] || sensors_[2] || sensors_[3] || sensors_[4]);//Tourne tant que les capteurs sont actifs, afin de s'assurer que les capteurs quittent la ligne
	
	while (!sensors_[2]) //Continuer de tourner tant que le capteur du milieu n'est pas actif, afin de retrouver la ligne
	{
		lecture(); //Acquisition des données en provenance des capteurs
	} 
	ajustementPwmMoteurs(0,0); //On arrête les moteurs lorsque la présente routine est terminée
}



