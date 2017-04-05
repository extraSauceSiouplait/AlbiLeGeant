#define F_CPU 8000000
#include "header.h"

#define VERT 'v'
#define ROUGE 'r'


//Montage:
    //INT0 sur D2
    //INT1 sur D3
    //DEL sur C0(+) - C1(-)     //valeur temporaire (a changer)
    
ISR(INT2_vect);


volatile char couleurChoisie = '\0'; //null (pas encore choisi)
bool estChoisi = false;


int main(){
    DDRB = 0x00;        //PORT B en lecture pour lire les interruptions.
    DDRC = 0xff;        //PORT C en écriture pour la DEL.
    ecrire0('C', 0);    //Assure que la DEL est eteinte avant le choix de la couleur.
    ecrire0('C', 1);    //Assure que la DEL est eteinte avant le choix de la couleur.
    initialisationINT2(1,0);    //falling edge activates interrupt.
   // initialisationINT1(1,0);    //falling edge activates interrupt.

    for(;;){
        if (!(PINB & (1<<3) && !estChoisi) ){
             if(couleurChoisie){  //ne fait rien si la couleur n'a pas encore été choisie.
                //ETAT = ETAT_SUIVANT (IMPORTANT!!!!)
                 estChoisi = true;
                EIMSK &= ~(1 << INT2);   //interruptions désactivées pour INT0 et INT1, le choix de couleur ne peut plus être changé.
            }
        }
        //Reste du code du robot.
    }
  
}


ISR(INT2_vect){
    _delay_ms(30);
    if(! (PINB & (1 << 2)) ){
        switch(couleurChoisie){
            case VERT:
                couleurChoisie = ROUGE;
                ecrire1('C', 0);
                ecrire0('C', 1);
                break;  
            case ROUGE: 
                couleurChoisie = VERT;
                ecrire1('C', 1);
                ecrire0('C', 0);
                break;
            default: //quand couleurChoisie est null.
                couleurChoisie = VERT;
                ecrire1('C', 1);
                ecrire0('C', 0);
                break;
        }
   } 
}
/*
ISR(INT1_vect){ 
    if(couleurChoisie){  //ne fait rien si la couleur n'a pas encore été choisie.
        //ETAT = ETAT_SUIVANT (IMPORTANT!!!!)
         EIMSK &= ~(1 << INT0) & ~(1 << INT1);   //interruptions désactivées pour INT0 et INT1, le choix de couleur ne peut plus être changé.
    }
}
*/
