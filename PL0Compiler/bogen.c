//Parser PL0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lex.h"
#include "bogen.h"
#include "simpleList.h"
#include "identDescr.h"
#include "semRoutinen.h"
#include "code.h"

//in lex.c als extern definiert
tMorph Morph={0};

extern procDescr* currProc;	//aktuell umgebende Prozedur
extern long* constBlock;	//Konstantenblock
extern int constBlockSize;	//Größe des Konstantenblocks
extern int		codeLen;	//Codelänge

extern entryProcCode actEPC;//struct das Informationen zum aktuellen EntryProc Code hält

extern listHead* lablList;	//Liste mit Labeln für Sprünge

extern short condCode;

extern FILE* test;			//Ausgabedatei
extern FILE* codeBuf;		//Zwischendatei für Code der akt. Proc

tBogen gExpr[]; //muss in factor bekannt sein

//==================================================
//Faktor
// 
//		 numeral
//	--------------------->
// /		              \
// 0 -----> ident -------> X
// \				      /
//  -----> 1 ------> 2 ---^ 
//	 '('      expr     '('

tBogen gFact[]={
	{BogenM, {(unsigned long) mcNum},	fa1 , 5, 1},   	//0 mcNum	(0-X) 
	{BogenM, {(unsigned long) mcIdent}, fa2 , 5, 2},	//1 mcIdent (0-X)
	{BogenS, {(unsigned long) '('    }, NULL, 3, 0},	//2 '('     (0-1)
	{BogenG, {(unsigned long) gExpr  }, NULL, 4, 0},	//3 expr    (1-2)
	{BogenS, {(unsigned long) ')'    }, NULL, 5, 0},	//4 ')'     (2-X)
	{BogenE, {(unsigned long) 0      }, NULL, 0, 0}		//X ENDE
};

//==================================================
//Term
//					 factor			'/'				
//	              ------------- 4 <----
//     factor    /                    |
// 0 ---------> 1 ------------------> 2 --------> X
//               \                    |
//				  ^------------ 3 <----
//					 factor	        '*'
		
tBogen gTerm[]={
	{BogenG, {(unsigned long) gFact}, NULL, 1, 0},	//0 Factor	(0-1)	
	{BogenN, {(unsigned long) 0},     NULL, 2, 0},	//1 Nil     (1-2)
	{BogenS, {(unsigned long) '*'},   NULL, 3, 4},  //2 '*'     (2-3)
	{BogenG, {(unsigned long) gFact}, te1 , 1, 0},	//3 Factor	(3-1)
	{BogenS, {(unsigned long) '/'},   NULL, 5, 6},	//4 '/'		(2-4)
	{BogenG, {(unsigned long) gFact}, te2 , 1, 0},  //5 Factor  (4-1)
	{BogenN, {(unsigned long) 0},     NULL, 7, 0},	//6 Nil		(2-X)
	{BogenE, {(unsigned long) 0},     NULL, 0, 0}	//X ENDE
};

//==================================================
//Expression
//
//                             -------------------> 4 ---> X
//				   term       /                    /|
// 0 ---------> 1 ---------> 3 <-- term 5 <--'+'--/ |
//  \	                    / \		                |
//	 \--------> 2 ---------/   \-- term 6 <--'-'-----
//		 '-'       term

tBogen gExpr[]={
	{BogenN, {(unsigned long) 0},		NULL, 1, 2},	//0 Nil	 (0-1)
	{BogenG, {(unsigned long) gTerm}, 	NULL, 4, 0},	//1 TERM (1-3)
	{BogenS, {(unsigned long) '-'}, 	NULL, 3, 0},	//2 -	 (0-2)
	{BogenG, {(unsigned long) gTerm},	ex1 , 4, 0},	//3 TERM (2-3)
	{BogenN, {(unsigned long) 0},		NULL, 5, 0},	//4 Nil	 (3-4)
	{BogenS, {(unsigned long) '+'},		NULL, 8, 6},	//5 +    (4-5)
	{BogenS, {(unsigned long) '-'},		NULL, 9, 7},	//6 -	 (4-6)
	{BogenN, {(unsigned long) 0}, 		NULL,10, 0},	//7 Nil	 (4-X)
	{BogenG, {(unsigned long) gTerm}, 	ex2 , 4, 0},	//8 Term (5-3)
	{BogenG, {(unsigned long) gTerm}, 	ex3 , 4, 0},	//9 Term (6-3)
	{BogenE, {(unsigned long) 0},		NULL, 0, 0}		//10 X ENDE	
};

//==================================================
//Condition
//	
//    ODD         expr
//   -----> 1 -----------> X
//  /                       \  expr
// 0-----> 2 --------------> 3
//  expr      =,#,<,>,<=,>=

tBogen gCond[]={
	{BogenS, {(unsigned long) zODD},  NULL, 2, 1},	//0 ODD		(0-1)	
	{BogenG, {(unsigned long) gExpr}, NULL, 3, 0},	//1 Expr	(0-2)
	{BogenG, {(unsigned long) gExpr}, co1 ,10, 0},	//2 Expr	(1-X)
	{BogenS, {(unsigned long) '='},   co2 , 9, 4},	//3 =		(2-3)
	{BogenS, {(unsigned long) '#'},   co3 , 9, 5},	//4 #		(2-3)
	{BogenS, {(unsigned long) '<'},   co4 , 9, 6},	//5 <		(2-3)
	{BogenS, {(unsigned long) '>'},   co5 , 9, 7},	//6 >		(2-3)
	{BogenS, {(unsigned long) zLE},   co6 , 9, 8},	//7 <=		(2-3)
	{BogenS, {(unsigned long) zGE},   co7 , 9, 0},	//8 >=		(2-3)
	{BogenG, {(unsigned long) gExpr}, co8 ,10, 0},	//9 Expr	(3-X)
	{BogenE, {(unsigned long) 0},     NULL, 0, 0}	//X ENDE
};

//==================================================
//Statement
//
//   --ident--> 1 ----':='---> 8 -----expr-------------------
//  /                                                        \
// |   ---IF--> 2 -condition-> 9 -----THEN----> 15 -statement-
// |  /                                                       \
// | | -WHILE-> 3 -condition-> 10 ----DO------> 16 -statement--
//  \|/                                                        \
//   0---CALL-> 4 ------------ident----------------------------> X
//  /|\						                                   /
// | | --'?'--> 5 ------------ident----------------------------
// | \                                                       /
// |  --'!'---> 6 ------------expr--------------------------
//  \                                                      /
//   --BEGIN--> 7 --statemnt-> 14 -----END-----------------
//               \              /  
//                ^----';'-----    

tBogen gStmt[]={
	{BogenM, {(unsigned long) mcIdent}, st1,  7, 1},	// 0  IDENT	(0-1)
	{BogenS, {(unsigned long) zCLL},    NULL,  9, 2},	// 1  CALL	(0-4)
	{BogenS, {(unsigned long) zBGN},    NULL, 10, 3},	// 2  BEGIN	(0-7)
	{BogenS, {(unsigned long) zIF},     NULL, 13, 4},	// 3  IF	(0-2)
	{BogenS, {(unsigned long) zWHL},    st5 , 16, 5},	// 4  WHILE	(0-3)
	{BogenS, {(unsigned long) '?'},     NULL, 19, 6},	// 5  ?		(0-5)
	{BogenS, {(unsigned long) '!'},     NULL, 20, 0},	// 6  !		(0-5)
	//=============================================================================
	{BogenS, {(unsigned long) zErg},    NULL,  8, 0},	// 7  := für IDENT		(1-8)
	{BogenG, {(unsigned long) gExpr},   st2,  21, 0},	// 8  EXPR für IDENT	(8-X)
	//=============================================================================						
	{BogenM, {(unsigned long) mcIdent}, st8 , 21, 0},	// 9  IDENT für CALL	(4-X)
	//=============================================================================
	{BogenG, {(unsigned long) gStmt},   NULL, 11, 0},	// 10 statement für BEGIN	(7-14)
	{BogenS, {(unsigned long) ';'},     NULL, 10,12},	// 11 ';' trennt statements (14-7)
	{BogenS, {(unsigned long) zEND},    NULL, 21, 0},	// 12 END zu BEGIN			(14-X)
	//=============================================================================
	{BogenG, {(unsigned long) gCond},   st3 , 14, 0},	// 13 condition für IF	(2-9)
	{BogenS, {(unsigned long) zTHN},    NULL, 15, 0},	// 14 THEN für IF		(9-15)
	{BogenG, {(unsigned long) gStmt},   st4 , 21, 0},	// 15 statement für IF	(15-X)
	//=============================================================================
	{BogenG, {(unsigned long) gCond},   st6 , 17, 0},	// 16 condition für WHILE	(3-10)
	{BogenS, {(unsigned long) zDO},     NULL, 18, 0},	// 17 DO für WHILE			(10-16)
	{BogenG, {(unsigned long) gStmt},   st7 , 21, 0},	// 18 statement für WHILE	(16-X)
	//=============================================================================
	{BogenM, {(unsigned long) mcIdent}, st9, 21, 0},	// 19 IDENT für ?		(5-X)
	//=============================================================================
	{BogenG, {(unsigned long) gExpr},   st10, 21, 0},	// 20 expression für !	(6-X)
	//=============================================================================
	{BogenE, {(unsigned long) 0},		NULL,  0, 0}	// 21 X ENDE	
};	


//==================================================
//Block
//
// 0 --CONST--> 1 --IDENT--> 2 --'='--> 3 --numeral--> 4
// |             \                                    /|
// |              ^--------------','-------<---------  |
// |                                                   |
// 5 <---------------------------';'-------<-----------
// |\
// | ---VAR---> 6 ---IDENT---> 7 
// |            \             /|
// |             ^----','----- |
// |                           |
// 8<-  <-------- ';'-----------
// |\ \
// | | \<-------------------------------------------------';'--------
// | |																 \
// |  --PROCEDURE--> 9 ---IDENT--> 10 ---';'---> 11 ---BLOCK---> 12--/
// |															 
// 13                                                             
// \
//  --STATEMENT--> X
//

tBogen gBlck[]={
	{BogenS, {(unsigned long) zCST},    NULL, 2, 1},	// 0  CONST 				(0-1)
	{BogenN, {(unsigned long) 0},       NULL, 7, 0},	// 1  Nil zu VAR 			(0-5)
	{BogenM, {(unsigned long) mcIdent}, bl1 , 3, 0},	// 2  IDENT 				(1-2)
	{BogenS, {(unsigned long) '='},     NULL, 4, 0},	// 3  =						(2-3)
	{BogenM, {(unsigned long) mcNum},   bl2 , 5, 0},	// 4  mcNum					(3-4)
	{BogenS, {(unsigned long) ','},     NULL, 2, 6},	// 5  , zwischen CONST IDENT(4-1)
	{BogenS, {(unsigned long) ';'},     NULL, 7, 0},	// 6  ; schließt CONST ab	(4-5)
	// Ende vom  CONST Teil -> VAR Teil
	{BogenS, {(unsigned long) zVAR},    NULL, 9, 8},	// 7  VAR					(5-6)
	{BogenN, {(unsigned long) 0},       NULL,12, 0},	// 8  Nil zu PROCEDURE		(5-8)
	{BogenM, {(unsigned long) mcIdent}, bl3 ,10, 0},	// 9  IDENT					(6-7)
	{BogenS, {(unsigned long) ','},     NULL, 9,11},	// 10 , zwischen IDENT		(7-6)
	{BogenS, {(unsigned long) ';'},     NULL,12, 0},	// 11 ; VAR -> PROCEDURE	(7-8)
	// Ende vom VAR Teil -> PROCEDURE Teil
	{BogenS, {(unsigned long) zPRC},    NULL,14,13},	// 12 PROCEDURE 			(8-9)
	{BogenN, {(unsigned long) 0},       bl6 ,18, 0},	// 13 Nil zu STATEMENT		(8-13)
	{BogenM, {(unsigned long) mcIdent}, bl4 ,15, 0},	// 14 IDENT für PROCEDURE	(9-10)
	{BogenS, {(unsigned long) ';'},     NULL,16, 0},	// 15 ; schleißt PROC ab	(10-11)
	{BogenG, {(unsigned long) gBlck},   NULL,17, 0},	// 16 block für PROCEDURE	(11-12)
	{BogenS, {(unsigned long) ';'}, 	NULL,12, 0},	// 17 ; schließt block ab	(12-8)
	// Ende vom PROCEDURE Teil -> statement Teil
	{BogenG, {(unsigned long) gStmt},   bl5, 19, 0},	// 18 statement 			(13-X)
	{BogenE, {(unsigned long) 0},       NULL,  0, 0}	// 19 X ENDE
};

tBogen gProg[]={
	{BogenG, {(unsigned long) gBlck}, NULL, 1, 0}, // 0 BLOCK	(0-1)
	{BogenS, {(unsigned long) '.'},	  pr1 , 2, 0}, // 1 .		(1-2)
	{BogenE, {(unsigned long) 0},     NULL, 0, 0}  // 2 X ENDE 
};

// Fehlercodes für "erwartetes Symbol nicht gelesen"
static char* errormsg;
static char* expectedS[]={
	":=\n\0","<=\n\0",">=\n\0","begin\n\0","call\n\0","const\n\0","do\n\0",
	"end\n\0","if\n\0","odd\n\0","procedure\n\0", "then\n\0","var\n\0","while\n\0"
};


int parse(tBogen* pGraph){
	
	tBogen* pBogenBuf	= pGraph;
	int ret 			= FAIL;
	
	if(Morph.MC == mcEmpty) lex();

	while(1){
						
		//passt gelesenes Symbol zu erwarteten Symbol?
		switch(pBogenBuf->bt & (BogenN+BogenS+BogenM+BogenG+BogenE)){
			
			case BogenN: 
				ret = 1; 
				break;
			case BogenS: 				
				//vorsorglich erwartetes Symbol an Fehlermeldung anhängen
				strcat(errormsg, " | Symbol oder Schlüsselwort: ");
				if(pBogenBuf->BogenX.S > 127){
					strcat(errormsg, expectedS[pBogenBuf->BogenX.S - 128]);
				}else{
					char append[3] = {pBogenBuf->BogenX.S, '\n', '\0'};
					strcat(errormsg, append);
				}
			
				ret = (Morph.Val.Symb == pBogenBuf->BogenX.S); 
				break; 
			case BogenM: 
				//vorsorglich erwartetes Token an Fehlermeldung anhängen
				strcat(errormsg, " | <Bezeichner>\n");
				ret = (Morph.MC == pBogenBuf->BogenX.M);
				break;
			case BogenG:
				//neuer Bogen -> Fehlermeldung hinfällig				
				strcpy(errormsg, "\0");
				ret = parse(pBogenBuf->BogenX.G);
				break;
			case BogenE:
				
				//alles okay -> Fehlermeldung hinfällig
				strcpy(errormsg, "\0");
				return OK;		
		}
		
		//Semantikroutinen hier ausführen (Funktionspointer)
		if(ret && (pBogenBuf->funcPointer != NULL)) ret = pBogenBuf->funcPointer();
		
		//kein Erfolg --> Alternative?
		if(!ret){
			
			if(pBogenBuf->altBogen != 0){
				//setze Alternativbogen	
				//printf("alternative: %d\n",pBogenBuf->altBogen);
				pBogenBuf = pGraph + pBogenBuf->altBogen;
				
			
			}else{
				//kein Alternativbogen verfügbar
				return FAIL;
			}
		//Erfolg --> neu lexen?	
		}else{
			
			//Symbol oder Morphem -> darauf folgt nächstes Zeichen
			if(pBogenBuf->bt & BogenS || pBogenBuf->bt & BogenM) lex();
			
			//setze Folgebogen 
			pBogenBuf = pGraph+pBogenBuf->nxtBogen;		
		}
	}
}

int main(int argc, char* argv[]){
		
	//Hauptprogramm als aktuelle Prozedur festlegen
	currProc = malloc(sizeof(procDescr));
	currProc->idx 				= 0;
	currProc->prntProc			= NULL;
	currProc->localNameList		= malloc(sizeof(listHead));
	currProc->memAllocCount		= 0;
	
	//Lableliste einrichten
	lablList = malloc(sizeof(listHead));
	
	//Größe des Konstantenblocks und Codelänge(akt. Proc) festlegen -> wächst dynamisch
	constBlockSize	= 0;
	codeLen 		= 0;
	
	//ersten 4 Bytes in Codeausgabe überspringen -> später mit Anzahl der Prozeduren überschreiben
	test = fopen("test.cl0", "wb");
	int dummy = 0;
	fwrite(&dummy, sizeof(int), 1, test);
		
	errormsg = malloc(1024);
	
	if(argc > 1 && errormsg){
		if(initLex(argv[1])){			
			if(parse(gProg) == 1){
				printf("korrekt geparst\n");
			}else{
				printf("Syntaxfehler Zeile: %d, Spalte: %d\n",Morph.posLine, Morph.posCol);
				printf("%s erwartet, aber \n",errormsg);
				if(Morph.MC == mcSymb){
					if(Morph.Val.Symb > 127)	printf("Schlüsselwort: %s gefunden\n", expectedS[Morph.Val.Symb - 128]);
					else if(Morph.Val.Symb==-1)	printf("EOF gefunden\n");
					else 						printf("Symbol: %c gefunden\n", (char)Morph.Val.Symb);
				}
				else if(Morph.MC == mcIdent) 	printf("Bezeichner: %s gefunden\n",Morph.Val.pStr);
				else if(Morph.MC == mcNum)		printf("Zahl: %ld gefunden\n", Morph.Val.Num);
				else 							printf("nichts gefunden\n");
			}
		}
	}else return 1;
	
	free(errormsg);	
	return 0;
}
