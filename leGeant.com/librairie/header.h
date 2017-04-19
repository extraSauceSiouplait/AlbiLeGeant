/*! \file header.h
 *  \brief Fichier d'en-tête du projet
 * 
 *  Contient tous les #define et les #include nécessaire pour le projet. Les fichiers *.cpp n'ont qu'à inclure ce fichier.
 */

#ifndef HEADER_H
#define HEADER_H

#include "librairieFonctions.h"
#include "capteurs.h"
#include "can.h"
#include "Moteurs.h"
#include <avr/io.h> 
#include <util/delay.h>
#include <util/delay_basic.h>
#include <avr/interrupt.h>

#define VERT 'v'    //lors du choix de la couleur
#define ROUGE 'r'   //lors du choix de la couleur
#define GAUCHE 'g'  //lors du choix du coté avec la photorésistance
#define DROIT 'd'   //lors du choix du coté avec la photorésistance

#endif //HEADER_H

