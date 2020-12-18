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



int procIdx 	= 1; //0 ist das Hauptprogramm

tMorph Morph;			//in lex.c als extern deklariert
procDescr* currProc;	//in bogen.c als extern deklariert	
long* constBlock;		// -"-		
int constBlockSize;		// -"-
int		codeLen;		// -"-
entryProcCode actEPC;	// -"-

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
}

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

void writeCode_0(tCode_0 opCode){
	
	char bCode = 0;
	switch(opCode){
		case retProc:
			bCode = 23;
			fwrite(&bCode, sizeof(char), 1, codeBuf);  
			codeLen++;
			break;
			
		case putVal:
			bCode = 8;
			fwrite(&bCode, sizeof(char), 1, codeBuf);  
			codeLen++;
			break;
		
		case vzMinus:
			bCode = 10;
			fwrite(&bCode, sizeof(char), 1, codeBuf);  
			codeLen++;
			break;
		
		case OpAdd:
			bCode = 12;
			fwrite(&bCode, sizeof(char), 1, codeBuf);  
			codeLen++;
			break;
			
		case OpSub:
			bCode = 13;
			fwrite(&bCode, sizeof(char), 1, codeBuf);  
			codeLen++;
			break;
			
		case OpMult:
			bCode = 14;
			fwrite(&bCode, sizeof(char), 1, codeBuf);
			codeLen++;
			break;
		
		case OpDiv:
			bCode = 15;
			fwrite(&bCode, sizeof(char), 1, codeBuf);
			codeLen++;
			break;
			
		case storeVal:
			bCode = 7;
			fwrite(&bCode, sizeof(char), 1, codeBuf);
			codeLen++;
			break;
			
		case getVal:
			bCode = 9;
			fwrite(&bCode, sizeof(char), 1, codeBuf);
			codeLen++;
			break;	
		
		default: break;
	}
}

void writeCode_1(tCode_1 opCode, short arg1){

	char bCode = 0;
	char arg = 0;
	switch(opCode){
		case puConst:
			bCode = 6;
			fwrite(&bCode, sizeof(char), 1, codeBuf);
			
			arg = (unsigned char) (arg1 & 0xff);
			fwrite(&arg, sizeof(char), 1, codeBuf);
			arg = (unsigned char) (arg1 >> 8);
			fwrite(&arg, sizeof(char), 1, codeBuf);
			
			codeLen += 3;
			break;
			
		case puValVrLocl:
			bCode = 0;
			fwrite(&bCode, sizeof(char), 1, codeBuf);
			
			arg = (unsigned char) (arg1 & 0xff);
			fwrite(&arg, sizeof(char), 1, codeBuf);
			arg = (unsigned char) (arg1 >> 8);
			fwrite(&arg, sizeof(char), 1, codeBuf);
			
			codeLen += 3;
			break;
			
		case puAdrVrLocl:
			bCode = 3;
			fwrite(&bCode, sizeof(char), 1, codeBuf);
			
			arg = (unsigned char) (arg1 & 0xff);
			fwrite(&arg, sizeof(char), 1, codeBuf);
			arg = (unsigned char) (arg1 >> 8);
			fwrite(&arg, sizeof(char), 1, codeBuf);
			
			codeLen += 3;
			break;
		case call:
			bCode = 22;
			fwrite(&bCode, sizeof(char), 1, codeBuf);
			
			arg = (unsigned char) (arg1 & 0xff);
			fwrite(&arg, sizeof(char), 1, codeBuf);
			arg = (unsigned char) (arg1 >> 8);
			fwrite(&arg, sizeof(char), 1, codeBuf);
			
			codeLen += 3;
			break;
						
		default: break;
	}
}

void writeEPC(short arg1, short arg2, short arg3){
	
	char entryProc = 26;
	char arg = 0;

	fwrite(&entryProc, sizeof(char), 1, test);
	
	arg = (unsigned char) (arg1 & 0xff);
	fwrite(&arg, sizeof(char), 1, test);
	arg = (unsigned char) (arg1 >> 8);
	fwrite(&arg, sizeof(char), 1, test);
	
	arg = (unsigned char) (arg2 & 0xff);
	fwrite(&arg, sizeof(char), 1, test);
	arg = (unsigned char) (arg2 >> 8);
	fwrite(&arg, sizeof(char), 1, test);
	
	arg = (unsigned char) (arg3 & 0xff);
	fwrite(&arg, sizeof(char), 1, test);
	arg = (unsigned char) (arg3 >> 8);
	fwrite(&arg, sizeof(char), 1, test);
}


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
		printf("Semantikfehler: Konstantenbezeichner %s bereits vergeben", Morph.Val.pStr);
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
		else{printf("kein Speicher :("); exit(-1);}
		
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
	
	printf("bl3 erreicht\n\tsuche Variablenbezeichner in lokaler Namensliste");
	
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
		printf("Semantikfehler: Variablenbezeichner %s bereits vergeben", Morph.Val.pStr);
		exit(1);
	}
}


int bl4(){ // --ident--> (procedure)
	
	printf("bl4 erreicht\n\tsuche Prozedurbezeichner in lokaler Namensliste");
	
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
		//procIdx++;	
		
		printf("\tProzedurbezeichner %s akzeptiert\n", Morph.Val.pStr);
		return 1;
	}else{
		printf("Semantikfehler: Prozedurbezeichner %s bereits vergeben", Morph.Val.pStr);
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
					printf("    Variable: %s\n",((identDescr*)act->data)->name);
					break;
				case identConst:
					printf("    Const: %s", ((identDescr*)act->data)->name );
					printf(", Wert:  %ld\n", *(constBlock + (((constDescr*)((identDescr*)act->data)->pObj)->idx) * sizeof(long)));
					break;
			}
			act=getNext(currProc->localNameList);
		}
		
		if((procDescr*)currProc->prntProc != NULL) printf("umgebende Prozedur: %d\n", ((procDescr*)currProc->prntProc)->idx);
		else printf("keine umgebende Prozedur, da Hauptprozedur\n");
	}
	
	//=== DEBUG ===
	

	printf("\tschreibe retProc in Puffer, schreibe EntryProc in Ausgabe\n");
	
	//schreibe Operation RETPROC in Puffer
	writeCode_0(retProc);
	
	//schreibe Operation ENTRYPROC und Parameter direkt in Ausgabedatei
	writeEPC(codeLen + 7, currProc->idx, currProc->memAllocCount);

	//öffne Puffer zum lesen
	int ret = fclose(codeBuf);
	codeBuf = fopen("codeBuf", "rb");
	printf("\tschließe Buffer und öffne ihn zum lesen %d\n",ret);
	
	if(codeBuf == NULL){
		printf("Kein Speicher (codeBuf) :(");
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



int bl6(){
	
	printf("bl6 erreicht\n\töffne Datei codeBuf\n");
		
	//Codeausgabe leeren
	codeLen = 0;

	actEPC.IdxProc	= currProc->idx;
	actEPC.VarLen	= currProc->memAllocCount;
	
	codeBuf = fopen("codeBuf","ab");
	printf("\terrno: %d\n", errno);
	if(codeBuf == NULL)	printf("bl6 Dateifehler\n");
	
	return 1;
}

int fa1(){

	printf("fa1 erreicht\n\tsuche Konstantenwert in constBlock %d\n", constBlockSize);
	
	int found = 0;

	//Suche in Konstantenblock nach Wert der neuen Konstanten
	short i = 0;
	while(i < constBlockSize){
		
		printf("\tgesucht: %ld, gefunden: %ld\n", Morph.Val.Num, *(constBlock + i * sizeof(long)));

		if(*(constBlock + i * sizeof(long)) == Morph.Val.Num){
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
	writeCode_1(puConst, i);
	
	return 1;
}

int fa2(){

	printf("fa2 erreicht %d\n", Morph.MC);
	
	int index;
	int displ;
	
	int ret = searchConstant(Morph.Val.pStr, &index);
	if(ret){
		writeCode_1(puConst, index);
		printf("\tBezeichner ist Konstante, schreibe puConst\n");
		return 1;
	}
	
	ret = searchVarLocal(Morph.Val.pStr, &displ);
	if(ret){
		writeCode_1(puValVrLocl, displ);
		printf("\tBezeichner ist lokale Variable, schreibe puValVrLocl\n");
		return 1;
	}
	
	return 0;
}

int st1(){
	printf("st1 erreicht\n");
	
	int displ;
	int ret = searchVarLocal(Morph.Val.pStr, &displ);
	if(ret){
		printf("\tVariablenbezeichner gefunden, schreibe puAdrVrLocl\n");
		writeCode_1(puAdrVrLocl, displ);
		return 1;
	}else{
		printf("Semantikfehler: Variablenbezeichner %s nicht vereinbart\n", Morph.Val.pStr);
		exit(1);
	}

}

int st2(){
	printf("st2 erreicht, schreibe storeVal in Buffer\n");
		
	writeCode_0(storeVal);
	return 1;
}

int st8(){
	printf("st8 erreicht\n");
	int procIdx;
	int ret = searchProc(Morph.Val.pStr, &procIdx);
	if(ret){
		printf("\tProzedur gefunden, schreibe call\n");
		writeCode_1(call, procIdx);
		return 1;
	}else{
		printf("Semantikfehler: Prozedurbezeichner %s nicht vereinbart\n", Morph.Val.pStr);
		exit(1);
	}
}

int st9(){
	printf("st9 erreicht\n");
	int displ;
	int ret = searchVarLocal(Morph.Val.pStr, &displ);
	if(ret){
		printf("\tVariablenbezeichner gefunden, schreibe puAdrVrLocl und getVal\n");
		writeCode_1(puAdrVrLocl, displ);
		writeCode_0(getVal);
		return 1;
	}else{
		printf("Semantikfehler: Variablenbezeichner %s nicht vereinbart\n", Morph.Val.pStr);
		exit(1);
	}
}

int st10(){

	printf("st10 erreicht\n\tschreibe putVal in Buffer\n");

	writeCode_0(putVal);

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
	
	//schreibe die Anzahl Proceduren in die ersten 4 Bytes
	//fseek(test, 0, SEEK_SET);
	//int ret = fclose(test);
	printf("\trewind Ausgabe\n");
	//test = fopen("test.cl0","wb+");
	rewind(test);
	if(test == NULL){ 
		printf("Kein Speicher (Ausgabedatei) :(");
		exit(1);	
	}
	
	arg = procIdx & 0xffffffffffffff; 		fwrite(&arg, sizeof(char), 1, test);
	arg = (procIdx >> 8) & 0xffffffffffff;	fwrite(&arg, sizeof(char), 1, test);		
	arg = (procIdx >> 16) & 0xffffffffff;	fwrite(&arg, sizeof(char), 1, test);
	arg = (procIdx >> 24) & 0xffffffff;		fwrite(&arg, sizeof(char), 1, test);
	
	printf("ENDE erreicht\n");
	return 1;
}

int ex1(){
	
	printf("ex1 erreicht\n\tschreibe vzMinus in Buffer\n");

	writeCode_0(vzMinus);
	return 1;
}

int ex2(){

	printf("ex2 erreicht\n\tschreibe OpAdd in Buffer\n");

	writeCode_0(OpAdd);
	return 1;
}

int ex3(){

	printf("ex3 erreicht\n\tschreibe OpSub in Buffer\n");
	
	writeCode_0(OpSub);
	return 1;
}

int te1(){
	
	printf("te1 erreicht\n\t schreibe OpMult in Buffer\n");
	
	writeCode_0(OpMult);
	return 1;
}

int te2(){
	
	printf("te2 erreicht\n\t schreibe OpDiv in Buffer\n");
	
	writeCode_0(OpDiv);
	return 1;	
}

int kawup(){
	printf("steige in Ausdruck ab\n");
	return 1;
}
