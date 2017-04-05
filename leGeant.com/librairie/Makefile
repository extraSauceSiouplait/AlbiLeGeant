########       AVR Project Makefile   	  ########
#####                                        #####
#####	   Makefile produit et ecrit par     #####
#####	Simon Barrette & Jerome Collin pour  #####
#####		 INF1995 - 2016		     #####
#####					     #####
#####	      Inspire de Pat Deegan -        #####
#####  Psychogenic Inc (www.psychogenic.com) #####
##################################################

# Ce Makefile vous permet de compiler des projets
# pour les microcontroleurs Atmel AVR sur 
# Linux ou Unix, en utilisant l'outil AVR-GCC. 
# Ce Makefile supporte C & C++

include Makefile_commun.txt

#####      Details specifique a la cible       #####
#####  Vous devez les adapter a votre projet   #####

# Nom de votre projet
# (utilisez un seul mot, exemple: 'monprojet')
PROJECTNAME=libA

# Fichiers sources
# (Listez tous les fichiers a compiler, ex: 'tp1.c, tp2.cpp'):
# Utilisez le suffixe .cpp pour les fichiers C++
PRJSRC= $(wildcard *.cpp)



####################################################
#####         Configuration terminee           #####
#####                                          #####
#####  Le reste de cette section contient les  #####
##### details d'implementation vous permettant #####
##### de mieux comprendre le fonctionnement de ##### 
#####   ce Makefile en vue de sa modification  #####
####################################################



####### variables #######


#compilateur librairies
AVRC=avr-ar

####### Options de compilation #######                        
#Flags pour compilation de librairies

LIBFLAGS =crs

####### Cible (Target) #######

#Nom des cibles par defaut
TRG=$(PROJECTNAME).a



####### Creation des commandes du Makefile ####### 

# Creation des cibles Phony (Phony Target)
# En plus de la commande make qui permet de compiler
# votre projet, vous pouvez utilisez les commandes
# make all, make install et make clean

# Implementation de la cible
$(TRG): $(OBJDEPS)
	$(AVRC) $(LIBFLAGS) $(TRG) $(OBJDEPS)



# Verification des dependances (header dependencies)
-include *.d

# Pour plus d'information sur cette section, consulter:
# http://bit.ly/2580FU8

# Make all permet simplement de compiler le projet
#
all: $(TRG)

# Make install permet de compiler le projet puis
# d'ecrire le programme en memoire flash dans votre
# microcontroleur. Celui-ci doit etre branche par cable USB
install: $(HEXROMTRG)				
	$(AVRDUDE) -c $(AVRDUDE_PROGRAMMERID)   \
	-p $(MCU) -P -e -U flash:w:$(HEXROMTRG)

# Make clean permet d'effacer tout les fichiers generes
# lors de la compilation
clean:
	$(REMOVE) $(TRG) $(TRG).map *.o $(HEXTRG) *.d

# Pour plus d'information sur les phony target, consulter:
# http://bit.ly/1WBQe61

# De plus, pour mieux comprendre les makefiles et 
# leur fonctionnement, consulter la documentation de GNU Make:
# http://bit.ly/23Vpk8s

# Finalement, ce tutoriel en ligne constitut une bonne 
# introduction au Makefile:
# http://bit.ly/1XvxsN3

#####                    EOF                   #####
