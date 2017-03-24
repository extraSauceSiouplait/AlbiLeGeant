#define F_CPU 8000000
#include "../header.h"
#include <util/delay.h>

//Timer0 gère les PWM des moteurs;
//moteur droit: enable = D4  -  direction = D2	(avant = 0, arriere = 1)
//moteur gauche: enable = D5  - direction = D3  (avant = 0, arriere = 1)
//LED: 
//Son: (+) = B1  -  (-) = B0;

void decodeur(uint8_t,uint8_t,bool);


volatile uint16_t addresseActuelle = 0x0000;		//addresse de l'octet lu
volatile uint8_t compteurBoucle;					//nombre de repetitions de dbc restantes
volatile uint16_t addresseBoucle;					//addresse de dbc


int main()
{	
	DDRA = 0xFF;									//OUT
	DDRD = 0xFF;
	DDRB = 0xFF;									//OUT
	PORTD = 0x00;									
	PORTA = 0x00;
	uint8_t donnee[2];								//Tableau contenant instruction et operande
	
	Memoire24CXXX mem;
	mem.lecture(addresseActuelle,&donnee[0]);
	mem.lecture(0x0001+addresseActuelle,&donnee[1]);
	uint16_t nombreOctets = (donnee[0]<<8) + donnee[1];	//les deux premieres addresses representent le nb total d'octets de la routine
	bool estFini = false;								//si a atteint l'instruction fin
	for (addresseActuelle = 0x0002; addresseActuelle <= nombreOctets && !(estFini); addresseActuelle+=0x0002)		//boucle qui lit les instructions et operandes,puis les envoies au decodeur
	{
		mem.lecture(addresseActuelle, donnee, 2);		//Lecure de l'instruction
        _delay_ms(5);
		//mem.lecture(addresse,&donnee[1]);		//Lecture de l'operande
        _delay_ms(5);
		decodeur(donnee[0],donnee[1],estFini);
		//addresse+= 0x0001;
	}
	
	
} 
void decodeur(uint8_t instruction,uint8_t operande,bool estFini)
{
	switch(instruction)
	{
	
	case att: {  
             for(int i = 0; i < 25 * operande; i++){                           
                _delay_loop_2(F_CPU/(4*1000));           //on ne peut pas faire 25 ms * operande avec _delay_ms, car la valeur doit etre connu a la compilation. _delay_loop_2(parametre) attend 4*parametre cycles du cpu donc ici, il attend 1 ms.
             }
        } break;                             
        
	case dal: {                           
            PORTA = operande;                 			//si les pins de PORTA sont les entrées des dels
        } break;
        
	case det:{
            PORTA &= ~(operande);						//si les pins de PORTA sont les entrées des dels
        } break;
        
	case sgo:{
			TCCR0A |= (1 << COM0A0);  
            jouerSonorite(operande);
   } break;
        
	case sar:{
			TCCR0A &= ~(1 << COM0A0);
            PORTB = 0x00;							//Arret du son
        } break;
        
	case mar:{
										
            ajustementPwmTimer(0,0);					//Arret des moteurs
        } break;
        
	case 0x61:{
            ajustementPwmTimer(0,0);					//Arret des moteurs
        } break;
	
	case mav:{
            ecrire0('D', 2);							//Set direction = 0
            ecrire0('D', 3);
            ajustementPwmTimer(100 * operande/255, 100 * operande/255);			//ajustement de vitesse
	} break;
	
	case mre:{
            ecrire1('D', 2);											//Set direction = 1;
            ecrire1('D', 3);
            ajustementPwmTimer(100*operande/255, 100*operande/255);		//ajustement de vitesse
	} break;
	
	case trd:{
            ajustementPwmTimer(180, 180);									//debuter rotation droite du robot
            ecrire1('D', 3);
            ecrire0('D', 2);
            _delay_ms(1000);       										//ajuster pour que le robot vire de 90 degres
            ajustementPwmTimer(0,0);									//Set vitesse = 0
        } break;
	
	case trg:{
            ajustementPwmTimer(180, 180);	
              ecrire1('D', 2);		
              ecrire0('D', 3);									//debuter rotation gauche du robot
            _delay_ms(1000);        									//ajuster pour que le robot vire de 90 degres
            ajustementPwmTimer(0,0);									//Set vitesse =0
        } break;
        
        case dbc:{
         
            addresseBoucle = addresseActuelle;
            compteurBoucle = operande + 1;							//on enregistre l'addresse du debut de la boucle
        }
        case fbc:{
			if (compteurBoucle > 0)		
			{
				compteurBoucle -= 0x01;									//on est passer dans boucle donc compteurBoucle--
				addresseActuelle = addresseBoucle;						//on retourne au debut de la boucle
			}
			
			
	    }break;
        case fin:
			{
				
				estFini = true;
			}
		
    }
}



