//übernommen von Prof A. Beck
//Binärcodes PL0

#ifndef CODE_H
#define CODE_H

typedef enum TCODE_0{
  
	/*--- Kellerbefehle ---*/
	storeVal=7 ,/*07 ()	[Speichern Wert -> Adresse, beides aus Keller]*/
  	putVal=8   ,/*08 ()	[Ausgabe eines Wertes aus Keller nach stdout]*/
  	getVal=9   ,/*09 ()	[Eingabe eines Wertes von stdin in Adresse aus Keller ]*/
    
  	/*--- arithmetische Befehle ---*/
  	vzMinus=10 ,/*0A ()	[Vorzeichen -]*/
  	odd=11     ,/*0B () [ungerade -> 0/1]*/    
    
  	/*--- binäre Operatoren kellern 2 Operanden aus und das Ergebnis ein ----*/
  	OpAdd=12   ,/*0C ()	[Addition]                                    */
  	OpSub=13   ,/*0D ()	[Subtraktion ]                                */
  	OpMult=14  ,/*0E ()	[Multiplikation ]                             */
  	OpDiv=15   ,/*0F ()	[Division ]                                   */
  	cmpEQ=16   ,/*10 ()	[Vergleich = -> 0/1]                          */
  	cmpNE=17   ,/*11 ()	[Vergleich # -> 0/1]                          */
  	cmpLT=18   ,/*12 ()	[Vergleich < -> 0/1]                          */
  	cmpGT=19   ,/*13 ()	[Vergleich > -> 0/1]                          */
  	cmpLE=20   ,/*14 ()	[Vergleich <=-> 0/1]                          */
  	cmpGE=21   ,/*15 ()	[Vergleich >=-> 0/1]                          */
  
  	/*--- Sprungbefehle ---*/
  	retProc=23  ,/*17 ()	[Rücksprung]*/

  	/* Code für Spracherweiterungen */                                                 
  	pop        ,/*1C  Entfernt einen Wert aus dem Stack - nicht implentiert     */
  	swap       ,/*1D  Austausch Adressse gegen Wert     - nicht implementiert   */

  	EndOfCode   /*1E               fuer interne Verwendung durch die VM         */
}tCode_0;

typedef enum TCODE_1{
	
	/*--- Kellerbefehle ---*/
 	puValVrLocl=0,/*00 (short Displ)	[Kellern Wert lokale  Variable]*/
 	puValVrMain=1,/*01 (short Displ)	[Kellern Wert Main    Variable]*/
  	puAdrVrLocl=3,/*03 (short Displ)	[Kellern Adresse lokale  Variable]*/
  	puAdrVrMain=4,/*04 (short Displ)	[Kellern Adresse Main    Variable]*/
  	puConst    =6,/*06 (short Index)	[Kellern einer Konstanten]*/
  	  	
  	/*--- Sprungbefehle ---*/
  	call       =22,/*16 (short ProzNr)	[Prozeduraufruf]*/
  	jmp        =24,/*18 (short RelAdr)	[SPZZ innerhalb der Funktion]*/
  	jnot       =25,/*19 (short RelAdr)	[SPZZ innerhalb der Funkt.,Beding.aus Keller]*/
  	
  	/* Code für Spracherweiterungen */
  	putStrg    =27,/*1B (char[]) */
}tCode_1;

typedef enum TCODE_2{

	/*--- Kellerbefehle ---*/
  	puValVrGlob = 2,/*02 (short Displ,short Proc)	[Kellern Wert globale Variable]*/
  	puAdrVrGlob = 5,/*05 (short Displ,short Proc)	[Kellern Adresse globale Variable]*/
}tCode_2;

typedef enum TCODE_3{

	/*--- Sprungbefehle ---*/
  	entryProc=26 ,/*1A (short lenCode,short ProcIdx,lenVar)*/
}tCode_3;

#endif
