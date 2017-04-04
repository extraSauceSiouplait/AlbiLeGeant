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


/*void Capteurs:: ajustDroite(){
		ajustementPwmMoteurs(vitesseMoy,vitesseLow)
		_delay_ms(400);
	}

void Capteurs:: ajustGauche() {
		ajustementPwmMotweurs(vitesseLow,vitesseMax)
		_delay_ms(400);
	}

void Capteurs:: lineTrackingOpti() {
    
bool ligneTrouvee = false;
#define vitesseMax 80
#define vitesseMoy 50
#define vitesseLow 20
#define facteurVitesse (80 / 2.5)

    for(int i = 0; i < 5; i++) {
        if(sensors_[i]) 			//Verifie si la ligne est en vue
            ligneTrouvee = true;
    }
    
                           
    if (ligneTrouvee){     
        for(int i = 0; i < 5; i++) {	//Si le signal de la ligne est perdu, on garde les infos de l'ancienne
            oldLigne_[i] = sensors_[i];	//lecture
        }
    }
    
    for(int i = 0; i < 5; i++) {		//Stockage
            newLigne_[i] = sensors_[i];
        }

    
    for(int i = 0; i < 5; i++) {        //Mise a jour des valeurs des 5 capteurs dans 
        capteurs_[i] = (1 << i &= PORTA)//capteurs_
    }
    
    ajustementPwmMoteurs(vitesseMoy, vitesseMoy);         //Déplacement intial du robot en ligne droite

    
    if (!ligneTrouvee){             //In case the line is out of reach, rely on the last valid
        if (oldLigne > 45){         //reading to decide whether to pivot right or
            ajustDroite();          //left to reach the line again.
            ajustementPwmMoteurs(vitesseMax,vitesseMax);
        }
        else{
            ajustGauche();
			ajustementPwmMoteurs(vitesseMax,vitesseMax);
        }  
    }
    
    else{
		if(((capteurs[0] && (capteurs[4] || capteurs[5])) || (capteurs[5] && (capteurs[0] || capteurs[1])))  {
			///
			/// IL FAUT RAJOUTER LA GESTION DES INTERSECTIONS!	
			///
			
			} //Si intersection gauche ou droite
			
			
        if(oldLigne != newLigne){
        //Calculate the average reading of the line.
            position_  = (capteurs[0]) + (capteurs[1]*2) +(capteurs[2]*3)+(capteurs[3]*4)+(capteurs[4]*5);
            position_ /= (capteurs[0]+capteur[1]+capteurs[2]+capteurs[3]+capteurs[4]);
            //Calculate the required right and left speed
            //according to the graph. 
            
            nexVitesseGauche = floor(((6-position_)*facteurVitesse)+0.5);
            nextVitesseDroit = floor((position_*facteurVitesse)+0.5);


            if (nextVitesseGauche > vitesseMax) {
				nextVitesseGauche = vitesseMax;
                ajustementPwmMoteurs(nextVitesseGauche, vitesseMax);
			}
            else
				ajustementPwmMoteurs(nextVitesseGauche,nextVitesseDroit);
				

            if (nextVitesseDroit > vitesseMax)
                ajustementPwmMoteurs(nextVitesseGauche, vitesseMax);
            else
				ajustementPwmMoteurs(nextVitesseGauche,nextVitesseDroit)
            }
        }
    }*/




