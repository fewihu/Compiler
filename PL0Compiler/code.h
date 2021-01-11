//Binärcodes PL0
//übernommen von Prof A. Beck

#ifndef CODE_H
#define CODE_H

typedef enum TCODE_0{
  
	/*--- Kellerbefehle ---*/
	storeVal   ,/*07 ()	[Speichern Wert -> Adresse, beides aus Keller]*/
  	putVal     ,/*08 ()	[Ausgabe eines Wertes aus Keller nach stdout]*/
  	getVal     ,/*09 ()	[Eingabe eines Wertes von stdin in Adresse aus Keller ]*/
    
  	/*--- arithmetische Befehle ---*/
  	vzMinus    ,/*0A ()	[Vorzeichen -]*/
  	odd        ,/*0B () [ungerade -> 0/1]*/    
    
  	/*--- binäre Operatoren kellern 2 Operanden aus und das Ergebnis ein ----*/
  	OpAdd      ,/*0C ()	[Addition]                                    */
  	OpSub      ,/*0D ()	[Subtraktion ]                                */
  	OpMult     ,/*0E ()	[Multiplikation ]                             */
  	OpDiv      ,/*0F ()	[Division ]                                   */
  	cmpEQ      ,/*10 ()	[Vergleich = -> 0/1]                          */
  	cmpNE      ,/*11 ()	[Vergleich # -> 0/1]                          */
  	cmpLT      ,/*12 ()	[Vergleich < -> 0/1]                          */
  	cmpGT      ,/*13 ()	[Vergleich > -> 0/1]                          */
  	cmpLE      ,/*14 ()	[Vergleich <=-> 0/1]                          */
  	cmpGE      ,/*15 ()	[Vergleich >=-> 0/1]                          */
  
  	/*--- Sprungbefehle ---*/
  	retProc    ,/*17 ()	[Rücksprung]*/

  	/* Code für Spracherweiterungen */                                                 
  	pop        ,/*1C  Entfernt einen Wert aus dem Stack - nicht implentiert     */
  	swap       ,/*1D  Austausch Adressse gegen Wert     - nicht implementiert   */

  	EndOfCode   /*1E               fuer interne Verwendung durch die VM         */
}tCode_0;

typedef enum TCODE_1{
	
	/*--- Kellerbefehle ---*/
 	puValVrLocl,/*00 (short Displ)	[Kellern Wert lokale  Variable]*/
 	puValVrMain,/*01 (short Displ)	[Kellern Wert Main    Variable]*/
  	puAdrVrLocl,/*03 (short Displ)	[Kellern Adresse lokale  Variable]*/
  	puAdrVrMain,/*04 (short Displ)	[Kellern Adresse Main    Variable]*/
  	puConst    ,/*06 (short Index)	[Kellern einer Konstanten]*/
  	  	
  	/*--- Sprungbefehle ---*/
  	call       ,/*16 (short ProzNr)	[Prozeduraufruf]*/
  	jmp        ,/*18 (short RelAdr)	[SPZZ innerhalb der Funktion]*/
  	jnot       ,/*19 (short RelAdr)	[SPZZ innerhalb der Funkt.,Beding.aus Keller]*/
  	
  	/* Code für Spracherweiterungen */
  	putStrg    ,/*1B (char[]) */
}tCode_1;

typedef enum TCODE_2{

	/*--- Kellerbefehle ---*/
  	puValVrGlob,/*02 (short Displ,short Proc)	[Kellern Wert globale Variable]*/
  	puAdrVrGlob,/*05 (short Displ,short Proc)	[Kellern Adresse globale Variable]*/
}tCode_2;

typedef enum TCODE_3{

	/*--- Sprungbefehle ---*/
  	entryProc  ,/*1A (short lenCode,short ProcIdx,lenVar)*/
}tCode_3;

typedef struct EPC{
	char opCode;
	short CodeLen;
	short IdxProc;
	short VarLen;
}entryProcCode;

#endif
