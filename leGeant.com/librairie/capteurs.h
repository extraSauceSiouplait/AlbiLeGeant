#ifndef CAPTEURS_H
#define CAPTEURS_H

#define F_CPU 8000000


class Capteurs {
    
public:
    Capteurs();
    
    /*
     * Stocke les valeurs des capteurs IR.
     * --> sensors_[]
     */
    static void lecture();		  // Méthode permettant l'acquisition des données en provenance du capteur de ligne
    
    /*
     * Retourne la valeur du capteur IR indiqué en paramètre.
     */
    static bool getSensor(uint8_t indice);
    
    /*
     * Vérifie si les capteurs voient une intersection
     */
    static bool estIntersection();
    
    /*
     * Vérifie si les capteurs on réellement perdu la ligne
     */
    static bool estPerdu();
    
    /*
     * Pour la partie ALLERETOUR
     * Version plus précise de estPerdu
     */
    static bool estPerduLong();
    
    
private:
    static bool sensors_[5];
    
};


#endif
