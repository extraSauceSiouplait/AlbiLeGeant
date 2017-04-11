#include "header.h"
#define VERT 'v'     //lors du choix de la couleur
#define ROUGE 'r'   //lors du choix de la couleur
#define GAUCHE 'g'  //lors du choix du coté avec la photorésistance
#define DROIT 'd'   //lors du choix du coté avec la photorésistance


//Énumération des états de la machine à état
enum Etats {COULEUR = 0, UTURN = 1, TOGA = 2, TOGAB = 3, COMPTEURLIGNE_1 = 4, PARKING_1 = 5, TOABC = 6, COMPTEURLIGNE_2 = 7, ALLERETOUR = 8, CINQ40 = 9, PHOTORESISTANCE = 10, INTERMITTENCE = 11, TOAGC = 12, TOGAH = 13, PARKING_2 = 14};

//Variables globales
volatile char    couleurChoisie = VERT; //NUL (pas encore choisi)
volatile uint8_t etat = 0; //Initialisation de la variable etat permettant de décrire l'état présent de la machine à états
volatile uint8_t compteurIntersection = 0;
volatile uint8_t triggerBonneIntersection;


int main() {
                Capteurs capteur;  //Création d'un objet de classe Capteur
                int etat = 0;
                DDRD = 0xFF;
                initialisationPwmMoteurs();
                for(;;)
                {
                switch (etat){
                
                    case 0:
                    {
                    
                    //FIN DE ETAT PRECEDENT
                   
                
                    if(couleurChoisie == VERT)
                        triggerBonneIntersection = 2;
                    else
                        triggerBonneIntersection = 4;
                    
                    etat++;
                    break;
                    }
                    
                    case 1:
                    {
               
                    while(!capteur.estIntersection()){

                        capteur.lecture();
                        capteur.lineTracking();
                        }
                        compteurIntersection++;                     //les capteurs detectent une intersection
                
                    if(compteurIntersection == triggerBonneIntersection+1) {        //l'axe de rotation du robot est sur la ligne de sa couleur
                    
                        capteur.tournerGauche();
                    
                        while (capteur.getSensor(2))        //le capteur du centre erst encore sur le segment
                        {
                            capteur.lecture();
                        }
                        while (!capteur.getSensor(2))       //le capteur du centre est entre les deux lignes
                        {
                            capteur.lecture();
                        }
                        ajustementPwmMoteurs(0,0); //le capteur du centre est sur la nouvelle ligne
                        etat++;
                    }

                        while(capteur.estIntersection()) {      //le robot est sur l'intersection
                        capteur.lecture();                    
                        ajustementPwmMoteurs(50,50); 
                        
                            }
                   break;
                    }
                    case 2:
                    {
                        while (!capteur.estPerdu())
                        {
                            capteur.lecture();
                            capteur.lineTracking();
                        }                               //les capteurs detectent la fin de la ligne
                        ajustementPwmMoteurs(60,60);
                        _delay_ms(1000);               //a ajuster pour que axe de rotation du robot soit au bout de la ligne
                        
                        if (couleurChoisie == VERT)
                        {
                            capteur.tourner180Gauche();
                        }
                        else
                        {
                            capteur.tourner180Droite();     
                        }                               //le robot a fait un 180 sur la ligne
                        while (!capteur.estIntersection())
                        {
                            capteur.lecture();
                            capteur.lineTracking();
                        }
                        capteur.intersectionGauche();
                        etat++;
                        break;
                    }
                    
                    case 3:
                    {
                        
                        capteur.lecture();
                        capteur.lineTracking();
                        break;
                    }
                    
                }     
            }//fin intersection

    
     //Machine à état décrivant le parcours du robot
   

    }

