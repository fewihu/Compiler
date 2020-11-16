//Felix Müller 18-041-61 
//Parser PL0

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "lex.h"
#include "bogen.h"

//in lex.c als extern definiert
tMorph Morph={0};

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
	{BogenM, {(unsigned long) mcNum},   NULL, 5, 1},   	//0 mcIdent	(0-X) 
	{BogenM, {(unsigned long) mcIdent}, NULL, 5, 2},	//1 mcNum   (0-X)
	{BogenS, {(unsigned long) '('    }, NULL, 3, 0},	//2 '('     (0-1)
	{BogenG, {(unsigned long) gExpr  }, NULL, 4, 0},	//3 expr    (1-2)
	{BogenS, {(unsigned long) ')'    }, NULL, 5, 0},	//4 ')'     (2-X)
	{BogenE, {(unsigned long) 0      }, NULL, 0, 0}		//X ENDE
};

//==================================================
//Term
//	 
//    factor      Nil
// 0 --------> 1 -----> X
//            / \
// '*' or '/'/	 \ factor
//			2----^
		
tBogen gTerm[]={
	{BogenG, {(unsigned long) gFact}, NULL, 1, 0},	//0 Factor	(0-1)	
	{BogenS, {(unsigned long) '*'},   NULL, 4, 2},	//1 *		(1-2)
	{BogenS, {(unsigned long) '/'},   NULL, 4, 3},	//2 /		(1-2)
	{BogenN, {(unsigned long) 0},     NULL, 5, 0},	//3	Nil		(1-X)
	{BogenG, {(unsigned long) gFact}, NULL, 1, 0},	//4 Factor	(2-1)
	{BogenE, {(unsigned long) 0 },    NULL, 0, 0}	//X ENDE
};

//==================================================
//Expression
//							   term
//	  				       --------------
//   ---------term-----   /	     		 \
//  /      \           \ / '+' or '-'    /
// 0 -Nil-> 1 --term--> 2 ------------> 3
//  \      /			 \ Nil
//   -----^				  \
//	  '-'				   X

tBogen gExpr[]={
	{BogenS, {(unsigned long) '-'},   NULL, 2, 1}, //0 -	(0-1)
	{BogenG, {(unsigned long) gTerm}, NULL, 3, 0}, //1 TERM (0-2)
	{BogenG, {(unsigned long) gTerm}, NULL, 3, 0}, //2 TERM (1-2)
	{BogenS, {(unsigned long) '+'},   NULL, 6, 4}, //3 +	(2-3)
	{BogenS, {(unsigned long) '-'},   NULL, 7, 5}, //4 - 	(2-3)
	{BogenN, {(unsigned long) 0},     NULL, 8, 0}, //5 Nil	(2-X)
	{BogenG, {(unsigned long) gTerm}, NULL, 3, 0}, //6 TERM	(3-2)
	{BogenG, {(unsigned long) gTerm}, NULL, 3, 0}, //7 TERM	(3-2)
	{BogenE, {(unsigned long) 0},     NULL, 0, 0}  //8 X ENDE
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
	{BogenG, {(unsigned long) gExpr}, NULL, 10,0},	//2 Expr	(1-X)
	{BogenS, {(unsigned long) '='},   NULL, 9, 4},	//3 =		(2-3)
	{BogenS, {(unsigned long) '#'},   NULL, 9, 5},	//4 #		(2-3)
	{BogenS, {(unsigned long) '<'},   NULL, 9, 6},	//5 <		(2-3)
	{BogenS, {(unsigned long) '>'},   NULL, 9, 7},	//6 >		(2-3)
	{BogenS, {(unsigned long) zLE},   NULL, 9, 8},	//7 <=		(2-3)
	{BogenS, {(unsigned long) zGE},   NULL, 9, 0},	//8 >=		(2-3)
	{BogenG, {(unsigned long) gExpr}, NULL,10, 0},	//9 Expr	(3-X)
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
	{BogenM, {(unsigned long) mcIdent}, NULL,  7, 1},	// 0  IDENT	(0-1)
	{BogenS, {(unsigned long) zCLL},    NULL,  9, 2},	// 1  CALL	(0-4)
	{BogenS, {(unsigned long) zBGN},    NULL, 10, 3},	// 2  BEGIN	(0-7)
	{BogenS, {(unsigned long) zIF},     NULL, 13, 4},	// 3  IF	(0-2)
	{BogenS, {(unsigned long) zWHL},    NULL, 16, 5},	// 4  WHILE	(0-3)
	{BogenS, {(unsigned long) '?'},     NULL, 19, 6},	// 5  ?		(0-5)
	{BogenS, {(unsigned long) '!'},     NULL, 20, 0},	// 6  !		(0-5)
	//=============================================================================
	{BogenS, {(unsigned long) zErg},    NULL,  8, 0},	// 7  := für IDENT		(1-8)
	{BogenG, {(unsigned long) gExpr},   NULL, 21, 0},	// 8  EXPR für IDENT	(8-X)
	//=============================================================================						
	{BogenM, {(unsigned long) mcIdent}, NULL, 21, 0},	// 9  IDENT für CALL	(4-X)
	//=============================================================================
	{BogenG, {(unsigned long) gStmt},   NULL, 11, 0},	// 10 statement für BEGIN	(7-14)
	{BogenS, {(unsigned long) ';'},     NULL, 10,12},	// 11 ';' trennt statements (14-7)
	{BogenS, {(unsigned long) zEND},    NULL, 21, 0},	// 12 END zu BEGIN			(14-X)
	//=============================================================================
	{BogenG, {(unsigned long) gCond},   NULL, 14, 0},	// 13 condition für IF	(2-9)
	{BogenS, {(unsigned long) zTHN},    NULL, 15, 0},	// 14 THEN für IF		(9-15)
	{BogenG, {(unsigned long) gStmt},   NULL, 21, 0},	// 15 statement für IF	(15-X)
	//=============================================================================
	{BogenG, {(unsigned long) gCond},   NULL, 17, 0},	// 16 condition für WHILE	(3-10)
	{BogenS, {(unsigned long) zDO},     NULL, 18, 0},	// 17 DO für WHILE			(10-16)
	{BogenG, {(unsigned long) gStmt},   NULL, 21, 0},	// 18 statement für WHILE	(16-X)
	//=============================================================================
	{BogenM, {(unsigned long) mcIdent}, NULL, 21, 0},	// 19 IDENT für ?		(5-X)	
	//=============================================================================
	{BogenG, {(unsigned long) gExpr},   NULL, 21, 0},	// 20 expression für !	(6-X)
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
// 8 <----------';'------------
// |\
// | ---PROCEDURE--> 9 ---IDENT--> 10 ---';'---> 11 ---BLOCK---> 12
// |															 |
// 13 <----------------------------------';'---------------------                                                             
// \
//  --STATEMENT--> X
//

tBogen gBlck[]={
	{BogenS, {(unsigned long) zCST},    NULL, 2, 1},	// 0  CONST 				(0-1)
	{BogenN, {(unsigned long) 0},       NULL, 7, 0},	// 1  Nil zu VAR 			(0-5)
	{BogenM, {(unsigned long) mcIdent}, NULL, 3, 0},	// 2  IDENT 				(1-2)
	{BogenS, {(unsigned long) '='},     NULL, 4, 0},	// 3  =						(2-3)
	{BogenM, {(unsigned long) mcNum},   NULL, 5, 0},	// 4  mcNum					(3-4)
	{BogenS, {(unsigned long) ','},     NULL, 2, 6},	// 5  , zwischen CONST IDENT(4-1)
	{BogenS, {(unsigned long) ';'},     NULL, 7, 0},	// 6  ; schließt CONST ab	(4-5)
	// Ende vom  CONST Teil -> VAR Teil
	{BogenS, {(unsigned long) zVAR},    NULL, 9, 8},	// 7  VAR					(5-6)
	{BogenN, {(unsigned long) 0},       NULL,12, 0},	// 8  Nil zu PROCEDURE		(5-8)
	{BogenM, {(unsigned long) mcIdent}, NULL,10, 0},	// 9  IDENT					(6-7)
	{BogenS, {(unsigned long) ','},     NULL, 9,11},	// 10 , zwischen IDENT		(7-6)
	{BogenS, {(unsigned long) ';'},     NULL,12, 0},	// 11 ; VAR -> PROCEDURE	(7-8)
	// Ende vom VAR Teil -> PROCEDURE Teil
	{BogenS, {(unsigned long) zPRC},    NULL,14,13},	// 12 PROCEDURE 			(8-9)
	{BogenN, {(unsigned long) 0},       NULL,18, 0},	// 13 Nil zu STATEMENT		(8-13)
	{BogenM, {(unsigned long) mcIdent}, NULL,15, 0},	// 14 IDENT für PROCEDURE	(9-10)
	{BogenS, {(unsigned long) ';'},     NULL,16, 0},	// 15 ; schleißt PROC ab	(10-11)
	{BogenG, {(unsigned long) gBlck},   NULL,17, 0},	// 16 block für PROCEDURE	(11-12)
	{BogenS, {(unsigned long) ';'}, 	NULL,12, 0},	// 17 ; schließt block ab	(12-13)
	// Ende vom PROCEDURE Teil -> statement Teil
	{BogenG, {(unsigned long) gStmt},   NULL, 19, 0},	// 18 statement 			(13-X)
	{BogenE, {(unsigned long) 0},       NULL,  0, 0}	// 19 X ENDE
};

tBogen gProg[]={
	{BogenG, {(unsigned long) gBlck}, NULL, 1, 0}, // 0 BLOCK	(0-1)
	{BogenS, {(unsigned long) '.'},	  NULL, 2, 0}, // 1 .		(1-2)
	{BogenE, {(unsigned long) 0},     NULL, 0, 0}  // 2 X ENDE 
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
				ret = (Morph.Val.Symb == pBogenBuf->BogenX.S); 
				break; 
			case BogenM: 
				ret = (Morph.MC == (tMC) pBogenBuf->BogenX.M); 
				break;
			case BogenG:
				ret = parse(pBogenBuf->BogenX.G);
				break;
			case BogenE:
				printf("BogenEnde\n");
				return OK;		
		}
		
		//Semantikroutinen hier ausführen (Funktionspointer)
		
		//keine Erfolg --> Alternative?
		if(!ret){
			
			if(pBogenBuf->altBogen != 0){
				//setze Alternativbogen	
				pBogenBuf = pGraph + pBogenBuf->altBogen;
			}else{
				//kein Alternativbogen verfügbar
				return FAIL;
			}
		//Erfolg --> neu lexen?	
		}else{
			
			if(pBogenBuf->bt & BogenS || pBogenBuf->bt & BogenM){
				
				lex();
			}
			
			//setze Folgebogen 
			pBogenBuf = pGraph+pBogenBuf->nxtBogen;			
		}
	}
}

int main(int argc, char* argv[]){
	
	if(argc > 1){
		if(initLex(argv[1])){			
			if(parse(gProg) == 1){
				printf("korrekt geparst\n");
			}else{
				printf("Syntaxfehler\n");
			}
		}
	}
}
