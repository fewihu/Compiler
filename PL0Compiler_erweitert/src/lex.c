//Lexer PL0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lex.h"

//Index Funktionspointer beenden
#define e 4 

//Prototypen für Lexeraktionen
static void l(void);	//lesen
static void b(void);	//beenden
static void sl(void);	//schreiben + lesen
static void gsl(void);	//großschreiben + lesen
static void slb(void);	//schrieben + lesen + beenden

//################################################################
//Zeichenklassenvektor
//0... Sonderzeichen, 1... Buchstabe, 2... Ziffern, 3... :, 4... =, 5... <, 6... >, 7... sonstige Sonderzeichen
//8... ausschliesslich Anfangszeichen eines SW (B,P,V,W, E)
//9... Anfangszeichen oder sonstiges Zeichen eines SW (A,C,D,E,F,G,H,I,L,N,O,R,S,T,U)
//10...Anführungszeichen
static char zkv[128]=
{// 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, //0..
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, //1..
	7, 0,10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //2..
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 0, 5, 4, 6, 0, //3.. 
	0, 9, 8, 9, 9, 8, 9, 9, 9, 9, 1, 1, 9, 1, 9, 9, //4..
	8, 1, 9, 9, 9, 9, 8, 8, 1, 1, 1, 0, 0, 0, 0, 0, //5..
	0, 9, 8, 9, 9, 9, 9, 9, 9, 9, 1, 1, 9, 1, 9, 9, //6..
	8, 1, 9, 9, 9, 9, 8, 8, 1, 1, 1, 0, 0, 0, 0, 0,	//7..
};

//################################################################
//Hashtabelle Schlüsselworterkennung
//0... kein Schlüsselwort
//1... begin, 2... call, 3... const, 4... do, 5... end, 6... if, 7... odd,
//8... procedure, 9... then, 10... var, 11... while, 12 else

static char htkw[128]=
{// 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
	0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 3, 6, 0, 0, 0, //0...
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, //1...
    9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //2...
   12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //3...
    0, 0, 0,10, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, //4...
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, //5...
    0, 0, 8, 0, 0, 0, 0, 0,11, 0, 5, 0, 0, 0, 0, 0, //6...
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  //7...
};

//Schlüsselworte für strcmp
static char* cKw[12]=
{
	"BEGIN\0", "CALL\0", "CONST\0", "DO\0" , "END\0"  , "IF\0",
	"ODD\0"  , "PROCEDURE\0"      ,"THEN\0", "VAR\0"  , "WHILE\0", "ELSE\0"
};


//################################################################
//Automatentabellen

//aktueller Zustand, Zeichenklasse -> Folgezustand
//Folgezustand e == 4 beim beenden der Übersicht halber 
int aTable_fz[12][11]  =
{  //SZ B  Z  :  =  <  >  - ASw sSw  "
	{e, 1, 2, 3, 0, 4, 5, 0,  9, 9,  11},//Zustand 0 leer
	{e, 1, 1, e, e, e, e, e,  1, 1,  e}, //Zustand 1 Buchstabe
	{e, e, 2, e, e, e, e, e,  e, e,  e}, //Zustand 2 Ziffer
	{e, e, e, e, 6, e, e, e,  e, e,  e}, //Zustand 3 :
	{e, e, e, e, 7, e, e, e,  e, e,  e}, //Zustand 4 <
	{e, e, e, e, 8, e, e, e,  e, e,  e}, //Zustand 5 >  
	{e, e, e, e, e, e, e, e,  e, e,  e}, //Zustand 6 :=
	{e, e, e, e, e, e, e, e,  e, e,  e}, //Zustand 7 <=
	{e, e, e, e, e, e, e, e,  e, e,  e}, //Zustand 8 >=
	{1, 1, 1, e, e, e, e, e,  1,10,  e}, //Zustand 9 Anfang eines SW gelesen
	{10,1, 1, 10,10,10,10,10, 1,10,  e}, //Zustand 10 nur Zeichen eines SW gelesen
	{11,11,11,11,11,11,11,11,11,11,  e}  //Zustand 11 Zeichenkette
};


//zustand, zeichenklasse -> Funktion
int aTable_pi[12][11] =
{  //SZ B  Z  :  =  <  >  - ASw sSw "
	{3, 2, 1, 1, 3, 1, 1, 0, 2, 2,  1}, //Zustand 0 leer
	{e, 2, 1, e, e, e, e, e, 2, 2,  e}, //Zustand 1 Buchstabe
	{e, e, 1, e, e, e, e, e, 1, e,  e}, //Zustand 2 Ziffer
	{e, e, e, e, 1, e, e, e, e, e,  e}, //Zustand 3 :
	{e, e, e, e, 1, e, e, e, e, e,  e}, //Zustand 4 <
	{e, e, e, e, 1, e, e, e, e, e,  e}, //Zustand 5 >
	{e, e, e, e, e, e, e, e, e, e,  e}, //Zustand 6 :=
	{e, e, e, e, e, e, e, e, e, e,  e}, //Zustand 7 <=
	{e, e, e, e, e, e, e, e, e, e,  e}, //Zustand 8 >=
	{e, 2, 1, e, e, e, e, e, 2, 2,  e}, //Zustand 9 Anfang eines SW gelesen
	{e, 2, 1, e, e, e, e, e, 2, 2,  e}, //Zustand 10 nur Zeichen eines SW gelesen
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  3}  //Zustand 11 Zeichenkette
};

//globals
static FILE* pFile;				//Quelldatei
char c;							//aktuell gelesenes Zeichen
static int line;				//Zeile von c
static int col;					//Spalte von c
char* buf;						//Puffer zum bauen des Tokens
int bufIndex;					//Index für den Puffer
static int end;					//flag bei eof gesetzt
int zustand;					//aktueller Zustand
extern tMorph Morph;    		//aktuelles Morphem
static tMorph MorphInit; 		//static zum 0-initialisiert

//################################################################
//Lexer Initialisierung
int initLex(char* fileName){

	Morph = MorphInit;

	bufIndex = 0;

	pFile	= fopen(fileName, "r+t");
	if(pFile == NULL) { printf("Fehler bei Öffnen der Datei\n"); return 0;}
	
	buf		= malloc(128 +1);
	if(buf == NULL){ printf("Bekomme keinen Speicher... :(\n"); return 0;}
	
	c = fgetc(pFile);
		
	line 	= 1; 
	col		= 0;	
		
	return 1;
}

//################################################################
//berechnet Hash-Index eines Tokens zum Überprüfen auf Schlüsselwort
int getHIndex(char* token){
	
	if(strlen(token) < 2 || strlen(token) > 9) return 0;
	
	int ret = (*(token+1))  * strlen(token);
	return ret%128;
}

//################################################################
//Funktionen für die Aktionen des Lexers
//lesen
static void l(void){

		c = fgetc(pFile);
		if(c == EOF) { end = 1; b();}
		if(c == '\n'){ line++; col = 0;} else {col++;}
}

//schreiben + lesen
static void sl(void){

	*(buf + bufIndex) = c;
	bufIndex++;
	*(buf + bufIndex) = '\0';

	l(); 
}

//als Großbuchstabe schreiben + lesen
static void gsl(void){
	
	*(buf + bufIndex) = (char) toupper(c);
	bufIndex++;
	*(buf + bufIndex) = '\0';
	
	l();
}

//beenden
static void b(void){
	//Abbruchbedingung von lextest
	
	char hIndex = 0;	
	if(end == 1){ Morph.Val.Symb = -1; Morph.MC = mcSymb; return; }
	
	switch(zustand){
		case 0: // beliebiges SZ (Klasse 0) oder =
			Morph.Val.Symb = (int)*buf;
			Morph.MC = mcSymb;
			break;
		case 1: //Bezeichner			
			Morph.Val.pStr = buf;
			Morph.MC = mcIdent;
			break;
		case 2: //Zahl
			Morph.Val.Num	= atol(buf);
			Morph.MC		= mcNum;
			break;
		case 3: // :
			//printf("\t\t\t LEXER: :\n");
			break;
		case 4: // <
			Morph.Val.Symb	= '<';
			Morph.MC		= mcSymb;
			break;
		case 5: // >
			//printf("\t\t\t LEXER: >\n");
			Morph.Val.Symb	= '>';
			Morph.MC		= mcSymb;
			break;
		case 6: //ergibt
			//printf("\t\t\t LEXER: :=\n");
			Morph.Val.Symb	= (long) zErg;
			Morph.MC		= mcSymb;
			break;
		case 7: //lower or equal
			Morph.Val.Symb	= (long) zLE;
			Morph.MC		= mcSymb;
			break;
		case 8: //greater or equal
			Morph.Val.Symb	= (long) zGE;
			Morph.MC		= mcSymb;
			break;
		case 9: //Bezeichner		
			Morph.Val.pStr = buf;
			Morph.MC = mcIdent;
			break;
		case 10: //Schlüsselwort -> genau finden			
			hIndex = getHIndex(buf);			
			if(htkw[(int)hIndex] == 0) {	
				Morph.MC = mcIdent;
				Morph.Val.pStr = buf;
				break;
			}
			else{
				if(strcmp(cKw[htkw[(int)hIndex]-1], buf) == 0){
					Morph.MC = mcSymb;
					Morph.Val.Symb = 130 + htkw[(int)hIndex];
					break;
				}else{
					Morph.MC = mcIdent;
					Morph.Val.pStr = buf;
					break;
				}
			}
			break;
		case 11: //Zeichenkette
			Morph.Val.pStr = buf;
			Morph.MC = mcString;
			break;
		default: break;
	}		
	end++;
}

//schreiben + lesen + beenden
static void slb(void){
	sl();
	b();
}

//Funktionenarray für Aktionsfunktionen (l, sl, gsl, slb, b)
static funcPointer fp[5] = { l, sl, gsl, slb, b };


//################################################################
//Funktion lex
tMorph* lex(void){
		
	zustand = 0; 
	int fZustand;	
	end = 0;
	bufIndex=0;
	strcpy(buf,"\0");
	Morph = MorphInit;
	Morph.posLine = line;
	Morph.posCol  = col;
		
	do{

		fZustand = aTable_fz[zustand]
							[((int)zkv[((int)c)])];
							
		fp[aTable_pi[zustand]
					[((int)zkv[((int)c)])]]();
		zustand = fZustand;
	}while(end == 0);
				
	return &Morph;
}
