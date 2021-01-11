//Parser PL0

#ifndef PARS_H
#define PARS_H

#define OK 1
#define FAIL 0
#include "lex.h"

//Typen von Bögen
typedef enum BOGEN_TYPE{

	BogenN = 0, //leer
	BogenS = 1, //Symbol
	BogenM = 2, //Morphem
	BogenG = 4, //Graph
	BogenE = 8  //Ende
}tBogenType;

//Funktionspointer für Semantikroutinen
typedef int (*funcPointer_int) (void);

//Bogen des Syntaxgraphen
typedef struct BOGEN{

	tBogenType bt;			//Bogentyp
	
	union BGX{				//genauere Beschreibung
		unsigned long	X;	//Initialisierung
		int 			S;	//Symbol
		tMC				M;	//Morphemcode
		struct BOGEN* 	G;	//Graph
	}BogenX;
	
	int (*funcPointer)(void);		//Aktion oder NULL
	int nxtBogen;			//Index Folgebogen
	int altBogen;			//Index Alternativbogen
	 
}tBogen;

#endif
