#ifndef CAPTEURS_H
#define CAPTEURS_H

#define F_CPU 8000000

#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>


class Capteurs {
    
public:
    Capteurs();
    
    /*
     * Stocke les valeurs des capteurs IR.
     * --> sensors_[]
     */
	void lecture();		  // Méthode permettant l'acquisition des données en provenance du capteur de ligne
    
    /*
     * Effectue une lecture()
     * Ajuste la direction en fonction de la pos. du robot sur la ligne
     */
	void lineTracking();
    
    /*
     * Pour le guidage avant la partie Photorésistance
     * Version plus précise du lineTracking() général
     */
    void lineTrackingTranquille();
    
    /*
     * Ajuste la direction pour tourner à Gauche indéfiniment
     */
	void tournerGauche();
    
    /*
     * Ajuste la direction des moteurs pour tourner à Droite indéfiniment
     */    
	void tournerDroite();
    
    
    /*
     * Ajuste la direction des moteurs afin de réaliser un virage d'intersection à Droite
     */
    void intersectionDroite();
    
    /*
     * Dirige les moteurs afin de réaliser un virage d'intersection à Gauche
     */
    void intersectionGauche();	
    
    /*
     * Vérifie si les capteurs voient une intersection
     */
    bool estIntersection();
    
    /*
     * Retourne la valeur du capteur IR indiqué en paramètre.
     */
    bool getSensor(uint8_t indice);
    
    /*
     * Vérifie si les capteurs on réellement perdu la ligne
     */
    bool estPerdu();
    
    /*
     * Pour la partie ALLERETOUR
     * Version plus précise de estPerdu
     */
    bool estPerduLong();
     
    /*
     * Sur une ligne droite, le robot tourne a Gauche jusqu'à ce qu'il retrouve la ligne.
     */
    void tourner180Gauche();
    
    /*
     * Sur une ligne droite, le robot tourne a Droite jusqu'à ce qu'il retrouve la ligne.
     */
    void tourner180Droite();
    
    /*
     * Pour le parking Final.
     * Sur une ligne droite, tourne a Droite jusqu'à ce que tous les capteurs redépassent la ligne
     */
    void tourner180DroiteFinal();
    
    /*
     * Pour le parking Final.
     * Sur une ligne droite, tourne a Gauche jusqu'à ce que tous les capteurs redépassent la ligne
     */
    void tourner180GaucheFinal();
    
    
    /*
     * Pour le CINQ40
     * Version plus précise de tournerGauche()
     */
    void tournerGaucheTranquille();
    
    /*
     * Fait du line tracking tant que les capteurs ne détectent pas une intersetction.
     */
    void attendreIntersection();
    
private:
    bool sensors_[5];
    
};


#endif
