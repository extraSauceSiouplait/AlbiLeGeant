/*
 * Titre:          TP3 - probleme2.cpp
 * Date :          27 janvier 2017
 * Modification :  27 janvier 2017
 * Auteurs :       Camille Bergeron-Miron et Samuel Meilleur 
 *  
 *                              TABLEAU D'ETATS 
 *       ___________________________________________________________________
 *      |  etat present  |   bouton    |     etat suivant     |    sortie   |
 *      |    INITIAL     |      0      |        INITIAL       |  1 (rouge)  |
 *      |    INITIAL     |      1      |         AMBRE        |  1 (rouge)  | 
 *      |     AMBRE      |      0      |         VERT1        | 1-2 (ambre) |
 *      |     AMBRE      |      1      |         AMBRE        | 1-2 (ambre) | 
 *      |     VERT1      |      0      |         VERT1        |  2 (vert)   |
 *      |     VERT1      |      1      |         ROUGE        |  2 (vert)   |
 *      |     ROUGE      |      0      |         ETEINT       |  1 (rouge)  |
 *      |     ROUGE      |      1      |         ROUGE        |  1 (rouge)  | 
 *      |     ETEINT     |      0      |         ETEINT       |      0      |
 *      |     ETEINT     |      1      |         VERT2        |      0      |
 *      |     VERT2      |      0      |        INITIAL       |   2 (vert)  |
 *      |     VERT2      |      1      |         VERT2        |   2 (vert)  |
 *      |________________|_____________|______________________|_____________|
 * 
 */

#define F_CPU 8000000
#include <avr/io.h> 
#include <util/delay.h>

bool verifierContact();

int main()
{
    DDRA = 0xff;     //PORT A en mode sortie
    DDRD = 0x00;    //PORT D en mode entree
    enum Etats{
        INITIAL = 0, AMBRE = 1, VERT1 = 2, ROUGE = 3, ETEINT = 4, VERT2 = 5  
    };
    int etatPresent = INITIAL;
    
    for(;;){
        switch(etatPresent){
            case INITIAL:{
                PORTA = 0x01;       //0x01 = rouge
                if (verifierContact()){
                    etatPresent = AMBRE;
                }
            } break;
            case AMBRE: {
               while(verifierContact()){
                    PORTA= 0x01;
                    _delay_ms(3);       //Delai plus court pour le rouge que le vert afin que la LED paraisse plus ambre (presque rouge sinon).
                    PORTA = 0x02;       //0x02 = vert
                    _delay_ms(10);
            }
            etatPresent = VERT1;
            } break;
            case VERT1: {
                PORTA = 0x02;
                if (verifierContact()){
                    etatPresent = ROUGE;
                }
            } break;
            case ROUGE: {
                while(verifierContact()){
                    PORTA = 0x01;
                }
                etatPresent = ETEINT;
            } break;
            case ETEINT: {
                PORTA = 0x00;
                if (verifierContact()){
                    etatPresent = VERT2;
                }
            } break;
            case VERT2: {
                while (verifierContact()){
                    PORTA = 0x02;
                }
                etatPresent = INITIAL;
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
