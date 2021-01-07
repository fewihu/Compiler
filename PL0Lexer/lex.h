//Felix Müller 18-041-61
//Lexer PL0

#ifndef LEX_H
#define LEX_H

//Funktionpointer für Aktionen
typedef void (*funcPointer) (void);

//Symbolcodes für zusammengesetzte Sonderzeichen und Schluesselworte
typedef enum T_ZS{
	zNIL,
	zErg = 128, zLE, zGE, //:=   <=   >=
	zBGN, zCLL, zCST, zDO, zEND, zIF, zODD, zPRC, zTHN, zVAR, zWHL, zELS
}tZS;

//Morphemcodes
typedef enum T_MC {mcEmpty, mcSymb, mcNum, mcIdent}tMC;

//Morphem-Struktur
typedef struct{
	tMC MC;			//Morphemcode
	int posLine;	//Zeile
	int posCol;		//Spalte
	
	union VAL{		//Wert abh. von Code
		long Num;
		char* pStr;
		int Symb;
	}Val;
	
	int MLen;		//Morphemlänge	
}tMorph;


//Fkt-prototypen
int initLex();
tMorph * lex();

#endif
