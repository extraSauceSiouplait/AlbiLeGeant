#include "capteurs.h"
#include "librairieFonctions.h"

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

void  Capteurs:: lineTracking() {
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

void Capteurs:: ajustDroite(){
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
    }
}
