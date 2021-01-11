//Semantikroutinen PL0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "semRoutinen.h"
#include "identDescr.h"
#include "lex.h"
#include "simpleList.h"
#include "code.h"
#include "write2Code.h"

int procIdx 	= 1; //0 ist das Hauptprogramm

//aus lex.c (extern)
tMorph Morph;			//in lex.c als extern deklariert

//aus bogen.c (extern)
procDescr*		currProc;	
long* 			constBlock;		
int 			constBlockSize;
int				codeLen;
entryProcCode	actEPC;
listHead* 		lablList;
short 			condCode;

//Ausgabedatei und Codepuffer (extern aus bogen.c)
FILE* test;				//Ausgabedatei
FILE* codeBuf;			//Zwischendatei (Code der aktuellen Prozedur)

//lokale Suche nach Bezeichner
int searchLocal(procDescr* pProc, char* ident){

	listElement* act;
	listHead* head = pProc->localNameList;
	
	if(head->size > 0){
		act = getFirst(head); 
		while(act != NULL){
			if(strcmp(((identDescr*)act->data)->name,ident) == 0) return 0;
			act=getNext(head);
		}
		return 1;
	}else{	
		return 1;
	}
}

//lokale Suche nach Variablenbezeichner
int searchVarLocal(char* varIdent, int* displ){
	
	listHead* 		head;
	listElement*	act;
	identDescr*		actDescr;
	varDescr*		dataVar;
	
	head = currProc->localNameList;
	
	if(head->size > 0){
		act = getFirst(head);
		while(act != NULL){
		
			actDescr = (identDescr*) act->data;
			if(actDescr->identType == identVar){
				if(strcmp(actDescr->name, varIdent) == 0){
					printf("\tHEUREKA\n");
					dataVar = (varDescr*) actDescr->pObj;
					*displ = dataVar->displacement;
					return 1;
				}
			}
		
			act = getNext(head);
		}
	}
	return 0;
}

//lokale Suche nach globaler Variable
int searchVarGlobal(char* varIdent, int* displ, int* procIdx){

	procDescr*		actProc = currProc;
	listHead*		head;
	listElement*	act;
	identDescr*		actDescr;
	varDescr*		dataVar;
	
	while(1){
	
		head = actProc->localNameList;
		
		if(head->size > 0){
			act = getFirst(head);
			while(act != NULL){
				actDescr = (identDescr*) act->data;
				
				if(actDescr->identType == identVar){
					if(strcmp(actDescr->name, varIdent) == 0){
						printf("\tHEUREKA\n");
						dataVar = (varDescr*) actDescr->pObj;
						*displ		= dataVar->displacement;
						*procIdx	= actProc->idx;
						return 1;
					}
				}
				
				act = getNext(head);
			}
		}
		
		if(actProc->idx == 0) break;
		actProc = (procDescr*)actProc->prntProc;
	}
	
	return 0;
}

//(globale) Suche nach Konstantenbezeichner
int searchConstant(char* constIdent, int* index){
	
	procDescr*		actProc = currProc;
	listHead* 		head;
	listElement* 	act;
	identDescr* 	actDescr;
	constDescr* 	dataConst;
	
	while(1){
				
		head = actProc->localNameList;
		
		if(head->size > 0){
			act = getFirst(head); 
			while(act != NULL){
			
				actDescr = (identDescr*) act->data;
				if(actDescr->identType == identConst){
					if(strcmp(actDescr->name, constIdent) == 0 ){
						printf("\tHEUREKA\n");
						dataConst = (constDescr*) actDescr->pObj;
						*index = dataConst->idx;
						return 1;
					}
				}
			
				act=getNext(head);
			}
		}		
		
		if(actProc->idx == 0) break;
		actProc = (procDescr*)actProc->prntProc;
				
	}
	
	return 0;
}

//(globale) Suche nach Prozedurbezeichner
int searchProc(char* procIdent, int* index){
	
	procDescr* actProc = currProc;
	listHead* head;
	listElement* act;
	identDescr* actDescr;
	procDescr* dataProc;
	
	while(1){
		
		head = actProc->localNameList;
		
		if(head->size > 0){
			act = getFirst(head);
			while(act != NULL){
				actDescr = (identDescr*) act->data;
				if(actDescr->identType == identProc){
					if(strcmp(actDescr->name, procIdent) == 0){
						printf("\tHEUREKA\n");
						printf("1\n");
						dataProc = (procDescr*) actDescr->pObj;
						printf("1\n");
						*index = dataProc->idx;
						printf("1\n");
						return 1;
					}
				}
				act = getNext(head);
			}
		}
		if(actProc->idx == 0) break;
		actProc = (procDescr*)actProc->prntProc;
	}
	
	return 0;	
}

// === Condition ===
int co1(){ //odd
	
	printf("co1 erreicht\n\tschreibe odd in Buffer\n");
	writeCode_0(odd, codeBuf);
	return 1;
}

int co2(){ //=

	printf("co2 erreicht\n\tcmpEQ wird gepuffert\n");
	condCode = cmpEQ;
	return 1;
}

int co3(){ //#

	printf("co3 erreicht\n\tcmpNE wird gepuffert\n");
	condCode = cmpNE;
	return 1;
}

int co4(){ //<

	printf("co4 erreicht\n\tcmpLT wird gepuffert\n");
	condCode = cmpLT;
	return 1;
}

int co5(){ //>

	printf("co5 erreicht\n\tcmpGT wird gepuffert\n");
	condCode = cmpGT;
	return 1;
}

int co6(){ //<=

	printf("co6 erreicht\n\tcmpLE wird gepuffert\n");
	condCode = cmpLE;
	return 1;
}

int co7(){ //>=

	printf("co7 erreicht\n\tcmpGE wird gepuffert\n");
	condCode = cmpGE;
	return 1;
}

int co8(){
	
	printf("co8 erreicht\n\tgepufferter Code wird geschrieben\n");
	writeCode_0(condCode, codeBuf);
	return 1;
}

// === Block ===
int bl1(){ // --ident-->  (const)

	printf("bl1 erreicht\n\tsuche Konstantenbezeichner in lokaler Namensliste\n");

	//wenn prozedurlokale Suche nach Bezeichner ok, dann
	//lege Bezeichnerdaten an, Konstantenbeschreibung in bl2
	if(searchLocal(currProc, Morph.Val.pStr)){
			identDescr* newIdent	= createIdentDescr(identConst, Morph.Val.pStr);
			listElement* newElement	= createElement(newIdent);	
			insert(newElement, currProc->localNameList);
			printf("\tKonstante %s akzeptiert\n", Morph.Val.pStr);		
			return 1;
	}else{
		printf("Semantikfehler: Konstantenbezeichner %s bereits vergeben (Zeile %d, Spalte %d)\n", 
			Morph.Val.pStr, Morph.posLine, Morph.posCol);
		exit(1);			
	}
}

int bl2(){ // --mcNum--> (Wert für const)
	
	printf("bl2 erreicht\n\tsuche im Konstantenblock nach const Wert\n");
		
	int found = 0;
	constDescr* newConst;
	
	//Suche in Konstantenblock nach Wert der neuen Konstanten
	int i = 0;
	while(i < constBlockSize){
		if(*(constBlock + i * sizeof(long)) == Morph.Val.Num){
			//gefunden? -> setze Index
			newConst	= createConstDescr(Morph.Val.Num, i);
			found++;
			printf("\tKonstantenwert %ld schon vorhanden, setze Index\n", Morph.Val.Num);
			break;
		}
		i++;
	}
	
	if(!found){
		//nicht gefunden? -> Konstantenblock erweitern und Index setzen
		
		long* newConstBlock = (long*) realloc(constBlock, (constBlockSize + 1) * sizeof(long));
		
		if (newConstBlock) constBlock = newConstBlock;
		else{printf("kein Speicher :(\n"); exit(-1);}
		
		if(constBlock){
			
			*(constBlock + (constBlockSize * sizeof(long))) = Morph.Val.Num;
			newConst	= createConstDescr(Morph.Val.Num, constBlockSize);	
			
			constBlockSize++;
			printf("\tCBlock vergrößert: %d\n", constBlockSize);
			printf("\tKonstantenwert %ld in Konstantenblock eingefügt\n", Morph.Val.Num);
				
		}else{
			printf("Kein Speicher :(\n");
			exit(-1);
		}
	}
	
	listElement* lastElement= getCurr(currProc->localNameList);
	((identDescr*)lastElement->data)->pObj	= newConst;
	
	return 1;
}

int bl3(){ // --ident--> (var)
	
	printf("bl3 erreicht\n\tsuche Variablenbezeichner in lokaler Namensliste\n");
	
	//wenn prozedurlokale Suche nach Bezeichner ok, dann 
	//lege Bezeichnerdaten und Variablenbeschreibung an		
	if(searchLocal(currProc, Morph.Val.pStr)){
			identDescr* newIdent	= createIdentDescr(identVar, Morph.Val.pStr);
			
			varDescr* newVar		= createVarDescr(currProc->memAllocCount);
			currProc->memAllocCount += 4;
			
			newIdent->pObj = newVar;
			listElement* newElement	= createElement(newIdent);	
			insert(newElement, currProc->localNameList);
			printf("\tVariable %s akzeptiert\n", Morph.Val.pStr);
			return 1;
	}else{
		printf("Semantikfehler: Variablenbezeichner %s bereits vergeben (Zeile %d, Spalte %d)\n", 
			Morph.Val.pStr, Morph.posLine, Morph.posCol);
		exit(1);
	}
}


int bl4(){ // --ident--> (procedure)
	
	printf("bl4 erreicht\n\tsuche Prozedurbezeichner in lokaler Namensliste\n");
	
	//wenn prozedurlokale Suche nach Bezeichner ok, dann 
	//lege Bezeichnerdaten an, Prozedurbeschreibung in bl5
	if(searchLocal(currProc, Morph.Val.pStr)){
		identDescr* newIdent	= createIdentDescr(identProc, Morph.Val.pStr);
		//procIdx++;
		procDescr* newProc		= createProcDescr(procIdx, currProc);
		procIdx++;
		newIdent->pObj			= newProc;		
		listElement* newElement	= createElement(newIdent);	
		insert(newElement, currProc->localNameList);
		
		//globale Variablen setzen
		currProc				= newProc;	//umgebende Proc ist neue Proc
		
		printf("\tProzedurbezeichner %s akzeptiert\n", Morph.Val.pStr);
		return 1;
	}else{
		printf("Semantikfehler: Prozedurbezeichner %s bereits vergeben (Zeile %d, Spalte %d)\n", 
			Morph.Val.pStr, Morph.posLine, Morph.posCol);
		exit(1);
	}
}

int bl5(){ // --;--> (schließt Block von procedure ab)

	printf("bl5 erreicht\n");
	
	//=== DEBUG ===
	
	printf("------------------------\nProzedurindex: %d\nSpeicherplatzzuordnungszähler: %d\n", currProc->idx, currProc->memAllocCount);
	
	if(currProc->localNameList->size > 0){
		listElement* act = getFirst(currProc->localNameList); 

		while(act != NULL){
	
			switch(((identDescr*)act->data)->identType){
				case identVar: 
					printf("\tVariable: %s\n",((identDescr*)act->data)->name);
					break;
				case identConst:
					printf("\tConst: %s", ((identDescr*)act->data)->name );
					printf(", Wert:  %ld\n", *(constBlock + (((constDescr*)((identDescr*)act->data)->pObj)->idx) * sizeof(long)));
					break;
				default: break;
			}
			act=getNext(currProc->localNameList);
		}
		
		if((procDescr*)currProc->prntProc != NULL) printf("umgebende Prozedur: %d\n", ((procDescr*)currProc->prntProc)->idx);
		else printf("keine umgebende Prozedur, da Hauptprozedur\n");
	}
	
	//=== DEBUG ===
	

	printf("\tschreibe retProc in Puffer, schreibe EntryProc in Ausgabe\n");
	
	//schreibe Operation RETPROC in Puffer
	writeCode_0(retProc, codeBuf);
	
	//schreibe Operation ENTRYPROC und Parameter direkt in Ausgabedatei
	writeEPC(codeLen + 7, currProc->idx, currProc->memAllocCount, test);

	//öffne Puffer zum lesen
	int ret = fclose(codeBuf);
	codeBuf = fopen("codeBuf", "rb");
	printf("\tschließe Buffer und öffne ihn zum lesen %d\n",ret);
	
	if(codeBuf == NULL){
		printf("Kein Speicher (codeBuf) :(\n");
		exit(1);	
	}
	
	//lese Puffer und schreibe Inhalt in Augabedatei
	char* pCodeBuf = malloc(sizeof(char) * codeLen);
	printf("\tCodelänge: %d\n",codeLen);
	ret = fread(pCodeBuf, sizeof(char), codeLen, codeBuf);
	printf("\tLesen: %d Bytes\n", ret);
	ret = fwrite(pCodeBuf, sizeof(char), codeLen, test);
	printf("\tSchreiben: %d Bytes\n", ret);

	//schließe und lösche Puffer
	ret = fclose(codeBuf);
	remove("codeBuf");
	printf("\tschließe Buffer %d und lösche die Datei\n", ret);
	
	//lokale Namensliste nicht mehr gebraucht, übergeordnete wird umgebende Prozedur
	//deleteList(currProc->localNameList);
	struct procDescr* prntProc = currProc->prntProc;
	currProc = (procDescr*) prntProc;
	
	return 1;
}

int bl6(){ //Block für neue Prozedur beginnt
	
	printf("bl6 erreicht\n\töffne Datei codeBuf\n");
		
	//Codeausgabe leeren
	codeLen = 0;

	actEPC.IdxProc	= currProc->idx;
	actEPC.VarLen	= currProc->memAllocCount;
	
	codeBuf = fopen("codeBuf","wb");
	printf("\terrno: %d\n", errno);
	if(codeBuf == NULL)	printf("bl6 Dateifehler\n");
	
	return 1;
}

// === Faktor ===
int fa1(){ //mcNum (Direktkonstante)

	printf("fa1 erreicht\n\tsuche Konstantenwert in constBlock %d\n", constBlockSize);
	
	int found = 0;

	//Suche in Konstantenblock nach Wert der neuen Konstanten
	short i = 0;
	while(i < constBlockSize){
		
		printf("\tgesucht: %ld, gefunden: %ld\n", Morph.Val.Num, *(constBlock + i));

		if(*(constBlock + i ) == Morph.Val.Num){
			//gefunden? -> setze Index
			found++;
			printf("\tKonstantenwert %ld schon vorhanden, setze Index\n", Morph.Val.Num);
			break;
		}
		i++;
	}
	
	//nicht gefunden -> Konstantenblock erweitern und Index setzen
	if(!found){
		printf("\tKonstantenwert noch nicht in constBlock\n");
		long* newConstBlock = (long*) realloc(constBlock, (constBlockSize + 1) * sizeof(long));
		
		if(newConstBlock) constBlock = newConstBlock;
		else{ printf("Kein Speicher :(\n"); exit(-1);}
		
		if(constBlock != NULL){
			
			constBlock[constBlockSize] = Morph.Val.Num;		
			i = constBlockSize;
			constBlockSize++;
			printf("\tCBlock vergrößert: %d\n", constBlockSize);
			printf("\tKonstantenwert %ld in Konstantenblock eingeügt\n", Morph.Val.Num);
		}else{
			printf("Kein Speicher :(\n");
			exit(-1);
		}
	}
	
	//schreibe Operation PuConst und Parameter in Puffer
	writeCode_1(puConst, i, codeBuf);
	
	return 1;
}

int fa2(){ //mcIdent (Konstante oder Variable)

	printf("fa2 erreicht %d\n", Morph.MC);
	
	int index;
	int displ;
	int procIdx;
	
	int ret = searchConstant(Morph.Val.pStr, &index);
	if(ret){
		writeCode_1(puConst, index, codeBuf);
		printf("\tBezeichner ist Konstante, schreibe puConst\n");
		return 1;
	}
	
	ret = searchVarLocal(Morph.Val.pStr, &displ);
	if(ret){
		writeCode_1(puValVrLocl, displ, codeBuf);
		printf("\tBezeichner ist lokale Variable, schreibe puValVrLocl\n");
		return 1;
	}
	
	//neu
	ret = searchVarGlobal(Morph.Val.pStr, &displ, &procIdx);
	if(ret){
		writeCode_2(puValVrGlob, displ, procIdx, codeBuf);
		printf("\tBezeichner ist globale Variable, schreibe puValVrGlob\n");
		return 1;
	}
	
	printf("Bezeichner %s ist nicht definiert (Zeile %d, Spalte %d)\n", 
		Morph.Val.pStr, Morph.posLine, Morph.posCol);
	exit(1);
}

// === Statement ===
int st1(){ //Ident für Zuweisung (Adresse wird auf Stack gepusht)
	
	printf("st1 erreicht\n");
	
	int displ;
	int procIdx;
	int ret = searchVarLocal(Morph.Val.pStr, &displ);
	if(ret){
		printf("\tVariablenbezeichner gefunden, schreibe puAdrVrLocl\n");
		writeCode_1(puAdrVrLocl, displ, codeBuf);
		return 1;
	}
	
	ret = searchVarGlobal(Morph.Val.pStr, &displ, &procIdx);
	if(ret){
		printf("\tVariablenbezeichner gefunden, schreibe puAdrVrGlob\n");
		writeCode_2(puAdrVrGlob, displ, procIdx, codeBuf);
		return 1;	
	}
		
	printf("Semantikfehler: Variablenbezeichner %s nicht vereinbart (Zeile %d, Spalte %d)\n", 
		Morph.Val.pStr, Morph.posLine, Morph.posCol);
	exit(1);
}

int st2(){ //expr für Ident (Zuweisung)

	printf("st2 erreicht\n\tschreibe storeVal in Buffer\n");
		
	writeCode_0(storeVal, codeBuf);
	return 1;
}

int st3(){
	
	printf("st3 erreicht\n\tCodelänge vor jnot %d\n", codeLen);
	
	short* relAdr	= malloc(sizeof(short));
	*relAdr			= codeLen;
	
	listElement* newElement = malloc(sizeof(listElement));
	newElement->data		= relAdr;
	
	insertFrst(newElement, lablList);
	
	writeCode_1(jnot, 0, codeBuf); 
	return 1;
}

int st4(){

	printf("st4 erreicht\n");

	listElement* label = popFrst(lablList);
	short x = *((short*) label->data);
	
	if(label){
		short lenCondBlck = ((short)(codeLen)) - x - 3;
		
		printf("\tSpringe zu Byte %d\n", x);
		
		int ret = fseek(codeBuf, x+1, SEEK_SET);
		printf("SEEK: %d\n",ret);
		
		printf("\tRelativadresse beträgt %d\n", lenCondBlck);
		
		char arg;
		
		arg = (unsigned char) (lenCondBlck & 0xff);
		fwrite(&arg, sizeof(char), 1, codeBuf);
		arg = (unsigned char) (lenCondBlck >> 8);
		fwrite(&arg, sizeof(char), 1, codeBuf);
		
		fseek(codeBuf, 0, SEEK_END);
		return 1;
	}else return 0;
}

int st5(){
	
	printf("st5 erreicht\n\tsetze Label vor Schleifenbedingung\n");
	short* relAdr	= malloc(sizeof(short));
	*relAdr			= codeLen;
	
	listElement* newElement = malloc(sizeof(listElement));
	newElement->data		= relAdr;
	
	insertFrst(newElement, lablList);
	
	return 1;
}

int st6(){

	printf("st6 erreicht\n\tsetze Label auf jnot Befehl\n");
	short* relAdr	= malloc(sizeof(short));
	*relAdr			= codeLen;
	
	listElement* newElement = malloc(sizeof(listElement));
	newElement->data		= relAdr;
	
	insertFrst(newElement, lablList);
	
	writeCode_1(jnot, 0, codeBuf); 
	return 1;
}

int st7(){
	
	printf("st7 erreicht\n\tberechne aus Labeln Relativadressen für Sprungbefehle\n");
	
	listElement* label = popFrst(lablList);
	short x = *((short*) label->data);
	int ret;
	
	if(label){
		short lenCondBlck = ((short)(codeLen)) - x;
		
		printf("\tSpringe zu Byte %d\n", x);
		
		ret = fseek(codeBuf, x+1, SEEK_SET);
		if(ret != 0){ printf("fseek :(\n"); exit(-1); }
		
		printf("\tRelativadresse (Rücksprunglabel) beträgt %d\n", lenCondBlck);
		
		char arg;
		
		arg = (unsigned char) (lenCondBlck & 0xff);
		fwrite(&arg, sizeof(char), 1, codeBuf);
		arg = (unsigned char) (lenCondBlck >> 8);
		fwrite(&arg, sizeof(char), 1, codeBuf);
		
		ret = fseek(codeBuf, 0, SEEK_END);
		if(ret != 0){ printf("fseek :(\n"); exit(-1);}
		
		label = popFrst(lablList);
		if(!label){ printf("NULL-Error Labelliste\n"); exit(1);}
		
		x = *((short*) label->data);
		
		lenCondBlck = ((short) codeLen) - x + 3;		
		printf("\tRelativadresse (Conditionlable) beträgt %d\n", -lenCondBlck);
							
		writeCode_1(jmp, -lenCondBlck, codeBuf);
		
		return 1;
	}else return 0;
}

int st8(){ //procIdent für call

	printf("st8 erreicht\n");
	int procIdx;
	int ret = searchProc(Morph.Val.pStr, &procIdx);
	if(ret){
		printf("\tProzedur gefunden, schreibe call\n");
		writeCode_1(call, procIdx, codeBuf);
		return 1;
	}
	
	printf("Semantikfehler: Prozedurbezeichner %s nicht vereinbart (Zeile %d, Spalte %d)\n"
		, Morph.Val.pStr, Morph.posLine, Morph.posCol);
	exit(1);
}

int st9(){ // ident für ? (Adresse wird gepusht)
	
	printf("st9 erreicht\n");
	
	int displ;
	int procIdx;
	
	int ret = searchVarLocal(Morph.Val.pStr, &displ);
	if(ret){
		printf("\tVariablenbezeichner gefunden, schreibe puAdrVrLocl und getVal\n");
		writeCode_1(puAdrVrLocl, displ, codeBuf);
		writeCode_0(getVal, codeBuf);
		return 1;
	}
	
	ret = searchVarGlobal(Morph.Val.pStr, &displ, &procIdx);
	if(ret){
		printf("\tVariablenbezeichner gefunden, schreibe puAdrVrGlob\n");
		writeCode_2(puAdrVrGlob, displ, procIdx, codeBuf);
		writeCode_0(getVal, codeBuf);
		return 1;
	}
	
	printf("Semantikfehler: Variablenbezeichner %s nicht vereinbart (Zeile %d, Spalte %d)\n"
		, Morph.Val.pStr, Morph.posLine, Morph.posCol);
	exit(1);
}

int st10(){ //Expression für ! (Wert steht auf Stack) 

	printf("st10 erreicht\n\tschreibe putVal in Buffer\n");

	writeCode_0(putVal, codeBuf);

	return 1;
}

int pr1(){

	printf("pr1 erreicht\n\tschreibe Konstantenblock an das Ende der Ausgabe\n");

	//schreibe Konstantenblock an das Ende der Ausgabedatei
	int i = 0;
	char arg = 0;
	long Const = 0;
	while(i < constBlockSize){
		
		Const = constBlock[i];
		printf("\tschreibe Konstante %ld\n",Const);
		
		arg = Const & 0xffffffffffffff; 	fwrite(&arg, sizeof(char), 1, test);
		arg = (Const >> 8) & 0xffffffffffff;fwrite(&arg, sizeof(char), 1, test);		
		arg = (Const >> 16) & 0xffffffffff;	fwrite(&arg, sizeof(char), 1, test);
		arg = (Const >> 24) & 0xffffffff;	fwrite(&arg, sizeof(char), 1, test);
		i++;
	}
	
	printf("\trewind Ausgabe\n");
	rewind(test);
	if(test == NULL){ 
		printf("Kein Speicher (Ausgabedatei) :(\n");
		exit(1);	
	}
	
	//schreibe Anzahl Prozeduren in die ersten 4 Bytes der Ausgabe
	arg = procIdx & 0xffffffffffffff; 		fwrite(&arg, sizeof(char), 1, test);
	arg = (procIdx >> 8) & 0xffffffffffff;	fwrite(&arg, sizeof(char), 1, test);		
	arg = (procIdx >> 16) & 0xffffffffff;	fwrite(&arg, sizeof(char), 1, test);
	arg = (procIdx >> 24) & 0xffffffff;		fwrite(&arg, sizeof(char), 1, test);
	
	printf("ENDE erreicht\n");
	return 1;
}

int ex1(){
	
	printf("ex1 erreicht\n\tschreibe vzMinus in Buffer\n");

	writeCode_0(vzMinus, codeBuf);
	return 1;
}

int ex2(){

	printf("ex2 erreicht\n\tschreibe OpAdd in Buffer\n");

	writeCode_0(OpAdd, codeBuf);
	return 1;
}

int ex3(){

	printf("ex3 erreicht\n\tschreibe OpSub in Buffer\n");
	
	writeCode_0(OpSub, codeBuf);
	return 1;
}

int te1(){
	
	printf("te1 erreicht\n\t schreibe OpMult in Buffer\n");
	
	writeCode_0(OpMult, codeBuf);
	return 1;
}

int te2(){
	
	printf("te2 erreicht\n\t schreibe OpDiv in Buffer\n");
	
	writeCode_0(OpDiv, codeBuf);
	return 1;	
}
