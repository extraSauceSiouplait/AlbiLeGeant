/*
 * Titre:          TP3 - probleme1.cpp
 * Date :          27 janvier 2017
 * Modification :  27 janvier 2017
 * Auteurs :       Camille Bergeron-Miron et Samuel Meilleur
 *  
 *                              TABLEAU D'ETATS 
 *      ___________________________________________________________________    
 *      |  etat present  |   bouton   |     etat suivant     |    sortie  |
 *      |    INITIAL     |      0     |        INITIAL       |      0     |
 *      |    INITIAL     | 1 (5 fois) |        ALLUME        |      0     |
 *      |    ALLUME      |     X      | INITIAL(apres 1 sec) |      1     |
 *      |________________|____________|______________________|____________|
 * 
 */


#define F_CPU 8000000
#include <avr/io.h> 
#include <util/delay.h>

bool verifierContact();

int main()
{
    DDRD = 0x00;       //PORT D en mode entree
    DDRA = 0xff;       //PORT A en mode sortie
    enum Etats {
        INITIAL = 1, ALLUME = 2
    };
    int etatPresent = INITIAL;
    int compteur = 0;
    
    for(;;){
        switch(etatPresent){
            case INITIAL: {
                if(verifierContact()){
                    while (verifierContact()){              //Permet d'assurer que le bouton est relache avant d'incrementer le compteur.                          
                    }                                       
                    compteur ++;
                    if(compteur == 5){
                        etatPresent = ALLUME;
                    }
                }
            } break;
            case ALLUME: {
                PORTA = 0x01;       //0x01 = rouge
                _delay_ms(1000);
                PORTA = 0x00;       //0x00 = eteint
                etatPresent = INITIAL;
                compteur = 0;
            } break;
        }
    
    }

}


bool verifierContact(){
    if(PIND == 0x04){                    //Si le processeur detecte un signal du bouton-poussoir, il s'assure qu'il celui-ci
        _delay_ms(10);                   //est encore present 10ms plus tard pour etre sur que le contact soit vrai.
       if (PIND == 0x04)
            return true;
    }
    return false;
}
