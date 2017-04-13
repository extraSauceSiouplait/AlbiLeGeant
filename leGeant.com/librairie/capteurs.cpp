#include "header.h"

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
        sensors_[i] = (PINA >> (i + 2)) & 0x01;     //lit chaque capteur et store l'etat dans un booleen.   (capteurs sur A2 a A7)
}

void  Capteurs:: lineTracking() {
	Moteurs::avancer();
    if (sensors_[2] || (sensors_[1] && sensors_[3])){       //seulement le sensor du millieu = 1
        // Le robot doit aller tout droit.
        ajustementPwmMoteurs(75,75);
    }
    else if ((sensors_[1] || sensors_[0]) && !(sensors_[3])){    //sensor de gauche et/ou du centre = 1
        // Le robot doit tourner a gauche.
        ajustementPwmMoteurs(50, 75);
    }
    else if ((sensors_[3] || sensors_[4]) && !(sensors_[1])){
        // Le robot doit tourner a droite.
        ajustementPwmMoteurs(75, 50);
    }
}

void  Capteurs:: lineTrackingTranquille() {
	Moteurs::avancer();
    if (sensors_[0]){      
        // Le robot doit aller a gauche toute.
        ajustementPwmMoteurs(40,90);
    }
    else if (sensors_[1]){   
        // Le robot doit tourner un peu a gauche.
        ajustementPwmMoteurs(40, 70);
    }
    else if (sensors_[2]){
        // Le robot va tout droit.
        ajustementPwmMoteurs(60, 60);
    }
    else if(sensors_[3]) {
        //Le robot doit tourner un peu a droite
        ajustementPwmMoteurs(70,40);
    }
    else if(sensors_[4]) {
        //Le robot doit aller a droite toute!
        ajustementPwmMoteurs(90,40);
    }
}

void Capteurs::tournerGauche(){
	Moteurs::tournerGauche();
    ajustementPwmMoteurs(90,90);
    _delay_ms(150);
	ajustementPwmMoteurs(53, 53);	//Débuter rotation vers la gauche du robot
}


void Capteurs::tournerGaucheTranquille(){
	Moteurs::tournerGauche();
    ajustementPwmMoteurs(60,60);
    _delay_ms(100);
	ajustementPwmMoteurs(35, 35);	//Débuter rotation vers la gauche du robot
}


void Capteurs::tournerDroite(){
	Moteurs::tournerDroit();
    ajustementPwmMoteurs(90,90);
    _delay_ms(150);
	ajustementPwmMoteurs(53,53);	//Débuter roation vers la droite du robot

}

void Capteurs::tourner180Gauche(){
    tournerGauche();

    do {
        lecture(); //Acquisition des données en provenance des capteurs
    }  while (!estPerdu());//Tourne tant que les capteurs sont actifs, afin de s'assurer que les capteurs quittent la ligne

    while (!sensors_[2]) {//Continuer de tourner tant que le capteur du milieu n'est pas actif, afin de retrouver la ligne
        lecture(); //Acquisition des données en provenance des capteurs
    }
    ajustementPwmMoteurs(0,0); //On arrête les moteurs lorsque la présente routine est terminée
}

void Capteurs::tourner180Droite(){
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



void Capteurs::intersectionGauche(){
    ajustementPwmMoteurs(60,65);
    _delay_ms(1000);//MODIFIER VALEUR					//la ligne perpendiculaire au robot croise son axe de rotation

    tournerGauche();			//debuter rotation
    lecture();
    while (!sensors_[2])
    {
        lecture();
    }
    ajustementPwmMoteurs(0,0);		//on arrete les moteurs lorsque le robot est parallele a la ligne


}
void Capteurs::intersectionDroite(){
    ajustementPwmMoteurs(60,65);
    _delay_ms(1100);            //la ligne perpendiculaire au robot croise son axe de rotation


    tournerDroite();            //debuter rotation
    while (!sensors_[2])
    {
        lecture();
    }
    ajustementPwmMoteurs(0,0);  //on arrete les moteurs lorsque le robot est parallele a la ligne
}

bool Capteurs::estIntersection(){
    return ((sensors_[0] && sensors_[1] && sensors_[2]) || (sensors_[2] && sensors_[3] && sensors_[4]));       //vrai si tous les capteurs de gauche ou/et droite sont actifs
}


bool Capteurs::estPerdu(){
    for(uint8_t i = 0; i < 200; i++){
        if (!(sensors_[0] || sensors_[1] || sensors_[2] || sensors_[3] || sensors_[4])) {
            _delay_ms(2);
            lecture();
        }
        else
            return false;
    }
    return true;
}
        
   

