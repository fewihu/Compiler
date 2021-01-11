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

tMorph Morph;		//in lex.c als extern deklariert

//aus bogen.c (extern)
procDescr*		currProc;		//aktuelle Prozedur	
long* 			constBlock;		//Pointer auf Beginn des Konstantenblocks		
int 			constBlockSize;	//Größe des Konstantenblocks (long)
int				codeLen;		//Codelänge
listHead* 		lablList;		//Labelliste für Sprünge
short 			condCode;		//gepufferte Conditioncode

FILE* codeFile;				//Ausgabedatei
FILE* codeBuf;			//Zwischendatei (Code der aktuellen Prozedur)

//lokale Suche nach Bezeichner
int searchLocal(procDescr* pProc, char* ident){

	listElement* act;
	listHead* head = pProc->localNameList;
	int size;
	
	if(head && head->size > 0){
		act = getFirst(head);
		size = head->size; 
		
		while(act != NULL && size > 0){
			if(strcmp(((identDescr*)act->data)->name,ident) == 0) return 0;
			act=getNext(head);
			size--;
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
	int size;
	
	if(head && head->size > 0){
		act = getFirst(head);
		size = head->size;
		
		while(act != NULL && size > 0){
		
			actDescr = (identDescr*) act->data;
			if(actDescr && actDescr->name && actDescr->identType == identVar){
				if(strcmp(actDescr->name, varIdent) == 0){
					dataVar = (varDescr*) actDescr->pObj;
					*displ = dataVar->displacement;
					return 1;
				}
			}
		
			act = getNext(head);
			size--;
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
	int size;
	
	while(1){
	
		head = actProc->localNameList;
		
		if(head && head->size > 0){
			act = getFirst(head);
			size = head->size;
			
			while(act != NULL && size > 0){
				actDescr = (identDescr*) act->data;
				
				if(actDescr->identType == identVar){
					if(strcmp(actDescr->name, varIdent) == 0){
						dataVar = (varDescr*) actDescr->pObj;
						*displ		= dataVar->displacement;
						*procIdx	= actProc->idx;
						return 1;
					}
				}
				
				act = getNext(head);
				size--;
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
		
		if(head && head->size > 0){
			act = getFirst(head); 
			int size = head->size; 
			while(act != NULL && size){
			
				actDescr = (identDescr*) act->data;
				if(actDescr->identType == identConst){
					if(strcmp(actDescr->name, constIdent) == 0 ){
						dataConst = (constDescr*) actDescr->pObj;
						*index = dataConst->idx;
						return 1;
					}
				}
			
				act=getNext(head); size--;
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
	int size;
	
	while(1){
		
		head = actProc->localNameList;
		
		if(head && head->size > 0){
			act = getFirst(head);
			size = head->size;
			
			while(act != NULL && size > 0){
				actDescr = (identDescr*) act->data;
				if(actDescr->identType == identProc){
					if(strcmp(actDescr->name, procIdent) == 0){
						dataProc = (procDescr*) actDescr->pObj;
						*index = dataProc->idx;
						return 1;
					}
				}
				act = getNext(head);
				size--;
			}
		}
		if(actProc->idx == 0) break;
		actProc = (procDescr*)actProc->prntProc;
	}
	
	return 0;	
}

// === Condition ===
int co1(){ //odd
	
	writeCode_0(odd, codeBuf);
	return 1;
}

int co2(){ //=

	condCode = cmpEQ;
	return 1;
}

int co3(){ //#

	condCode = cmpNE;
	return 1;
}

int co4(){ //<

	condCode = cmpLT;
	return 1;
}

int co5(){ //>

	condCode = cmpGT;
	return 1;
}

int co6(){ //<=

	condCode = cmpLE;
	return 1;
}

int co7(){ //>=

	condCode = cmpGE;
	return 1;
}

int co8(){
	
	writeCode_0(condCode, codeBuf);
	return 1;
}

// === Block ===
int bl1(){ // --ident-->  (const)


	//wenn prozedurlokale Suche nach Bezeichner ok, dann
	//lege Bezeichnerdaten an, Konstantenbeschreibung in bl2
	if(searchLocal(currProc, Morph.Val.pStr)){
			identDescr* newIdent	= createIdentDescr(identConst, Morph.Val.pStr);
			listElement* newElement	= createElement(newIdent);	
			insert(newElement, currProc->localNameList);		
			return 1;
	}else{
		printf("Semantikfehler: Konstantenbezeichner %s bereits vergeben (Zeile %d, Spalte %d)\n", 
			Morph.Val.pStr, Morph.posLine, Morph.posCol);
		fclose(codeBuf); fclose(codeFile); remove("out.cl0"); remove("codeBuf");
		exit(1);			
	}
}

int bl2(){ // --mcNum--> (Wert für const)
			
	int found = 0;
	constDescr* newConst;
	
	//Suche in Konstantenblock nach Wert der neuen Konstanten
	int i = 0;
	while(i < constBlockSize){
		if(*(constBlock + i * sizeof(long)) == Morph.Val.Num){
			//gefunden? -> setze Index
			newConst	= createConstDescr(Morph.Val.Num, i);
			found++;
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
		}else{
			fclose(codeFile); fclose(codeBuf); remove("out.cl0"); remove("codeBuf");
			printf("Kein Speicher :(\n");
			exit(-1);
		}
	}
	
	listElement* lastElement= getCurr(currProc->localNameList);
	((identDescr*)lastElement->data)->pObj	= newConst;
	
	return 1;
}

int bl3(){ // --ident--> (var)
		
	//wenn prozedurlokale Suche nach Bezeichner ok, dann 
	//lege Bezeichnerdaten und Variablenbeschreibung an		
	if(searchLocal(currProc, Morph.Val.pStr)){
			identDescr* newIdent	= createIdentDescr(identVar, Morph.Val.pStr);
			
			varDescr* newVar		= createVarDescr(currProc->memAllocCount);
			currProc->memAllocCount += 4;
			
			newIdent->pObj = newVar;
			listElement* newElement	= createElement(newIdent);	
			insert(newElement, currProc->localNameList);
			return 1;
	}else{
		printf("Semantikfehler: Variablenbezeichner %s bereits vergeben (Zeile %d, Spalte %d)\n", 
			Morph.Val.pStr, Morph.posLine, Morph.posCol);
		fclose(codeFile); fclose(codeBuf); remove("out.cl0"); remove("codeBuf");
		exit(1);
	}
}


int bl4(){ // --ident--> (procedure)
	
	//wenn prozedurlokale Suche nach Bezeichner ok, dann 
	//lege Bezeichnerdaten an, Prozedurbeschreibung in bl5
	if(searchLocal(currProc, Morph.Val.pStr)){
		identDescr* newIdent	= createIdentDescr(identProc, Morph.Val.pStr);
		procDescr* newProc		= createProcDescr(procIdx, currProc);
		procIdx++;
		newIdent->pObj			= newProc;		
		listElement* newElement	= createElement(newIdent);	
		insert(newElement, currProc->localNameList);
		
		//globale Variablen setzen
		currProc				= newProc;	//umgebende Proc ist neue Proc
		return 1;
	}else{
		printf("Semantikfehler: Prozedurbezeichner %s bereits vergeben (Zeile %d, Spalte %d)\n", 
			Morph.Val.pStr, Morph.posLine, Morph.posCol);
		fclose(codeFile); fclose(codeBuf); remove("out.cl0"); remove("codeBuf");
		exit(1);
	}
}

int bl5(){ // --;--> (schließt Block von procedure ab)
	
	//schreibe Operation RETPROC in Puffer
	writeCode_0(retProc, codeBuf);
	
	//schreibe Operation ENTRYPROC und Parameter direkt in Ausgabedatei
	writeEPC(codeLen + 7, currProc->idx, currProc->memAllocCount, codeFile);

	//öffne Puffer zum lesen
	int ret = fclose(codeBuf);
	if(ret != 0){
		printf("fclose bl5 :(\n");
		fclose(codeFile); fclose(codeBuf); remove("out.cl0"); remove("codeBuf");
		exit(1);
	}
	
	codeBuf = fopen("codeBuf", "rb");
	if(codeBuf == NULL){
		fclose(codeFile); fclose(codeBuf); remove("out.cl0"); remove("codeBuf");
		printf("Kein Speicher (codeBuf) :(\n");
		exit(1);	
	}
	
	//lese Puffer und schreibe Inhalt in Augabedatei
	char* pCodeBuf = malloc(sizeof(char) * codeLen);
	ret = fread(pCodeBuf, sizeof(char), codeLen, codeBuf);
	ret = fwrite(pCodeBuf, sizeof(char), codeLen, codeFile);

	//schließe und lösche Puffer
	ret = fclose(codeBuf);
	remove("codeBuf");
	
	struct procDescr* prntProc = currProc->prntProc;
	if(prntProc != NULL) currProc = (procDescr*) prntProc;
	
	return 1;
}

int bl6(){ //Block für neue Prozedur beginnt
		
	//Codeausgabe leeren
	codeLen = 0;
	codeBuf = fopen("codeBuf","wb");

	if(codeBuf == NULL)	{
		printf("bl6 Dateifehler\n");
		fclose(codeFile); fclose(codeBuf); remove("out.cl0"); remove("codeBuf");	
		exit(1);
	}
	
	return 1;
}

// === Faktor ===
int fa1(){ //mcNum (Direktkonstante)
	
	int found = 0;

	//Suche in Konstantenblock nach Wert der neuen Konstanten
	short i = 0;
	while(i < constBlockSize){
		
		if(*(constBlock + i ) == Morph.Val.Num){

			found++; //gefunden? -> setze Index
			break;
		}
		i++;
	}
	
	//nicht gefunden -> Konstantenblock erweitern und Index setzen
	if(!found){
		long* newConstBlock = (long*) realloc(constBlock, (constBlockSize + 1) * sizeof(long));
		
		if(newConstBlock) constBlock = newConstBlock;
		else{ 
			printf("Kein Speicher :(\n");
			fclose(codeFile); fclose(codeBuf); remove("out.cl0"); remove("codeBuf"); 
			exit(-1);
		}
		
		if(constBlock != NULL){
			
			constBlock[constBlockSize] = Morph.Val.Num;		
			i = constBlockSize;
			constBlockSize++;
		}else{
			printf("Kein Speicher :(\n");
			fclose(codeFile); fclose(codeBuf); remove("out.cl0"); remove("codeBuf");
			exit(-1);
		}
	}
	
	//schreibe Operation PuConst und Parameter in Puffer
	writeCode_1(puConst, i, codeBuf);
	
	return 1;
}

int fa2(){ //mcIdent (Konstante oder Variable)

	int index;
	int displ;
	int procIdx;
		
	int ret = searchConstant(Morph.Val.pStr, &index);
	if(ret){
		writeCode_1(puConst, index, codeBuf);
		return 1;
	}
	
	ret = searchVarLocal(Morph.Val.pStr, &displ);
	if(ret){
		writeCode_1(puValVrLocl, displ, codeBuf);
		return 1;
	}
	
	//neu
	ret = searchVarGlobal(Morph.Val.pStr, &displ, &procIdx);
	if(ret){
		writeCode_2(puValVrGlob, displ, procIdx, codeBuf);
		return 1;
	}
	
	printf("Bezeichner %s ist nicht definiert (Zeile %d, Spalte %d)\n", 
		Morph.Val.pStr, Morph.posLine, Morph.posCol);
	fclose(codeFile); fclose(codeBuf); remove("out.cl0"); remove("codeBuf");
	exit(1);
}

// === Statement ===
int st1(){ //Ident für Zuweisung (Adresse wird auf Stack gepusht)
	
	int displ;
	int procIdx;
	int ret = searchVarLocal(Morph.Val.pStr, &displ);
	if(ret){
		writeCode_1(puAdrVrLocl, displ, codeBuf);
		return 1;
	}
	
	ret = searchVarGlobal(Morph.Val.pStr, &displ, &procIdx);
	if(ret){
		writeCode_2(puAdrVrGlob, displ, procIdx, codeBuf);
		return 1;	
	}
		
	printf("Semantikfehler: Variablenbezeichner %s nicht vereinbart (Zeile %d, Spalte %d)\n", 
		Morph.Val.pStr, Morph.posLine, Morph.posCol);
	fclose(codeFile); fclose(codeBuf); remove("out.cl0"); remove("codeBuf");
	exit(1);
}

int st2(){ //expr für Ident (Zuweisung)
		
	writeCode_0(storeVal, codeBuf);
	return 1;
}

int st3(){
	
	short* relAdr	= malloc(sizeof(short));
	*relAdr			= codeLen;
	
	listElement* newElement = malloc(sizeof(listElement));
	newElement->data		= relAdr;
	
	insertFrst(newElement, lablList);
	
	writeCode_1(jnot, 0, codeBuf); 
	return 1;
}

int st4(){

	listElement* label = popFrst(lablList);
	
	if(label){
		short x = *((short*) label->data);
		short lenCondBlck = ((short)(codeLen)) - x - 3;
		
		int ret = fseek(codeBuf, x+1, SEEK_SET);
		if(ret != 0){
			printf("fseek st4 :(\n");
			fclose(codeFile); fclose(codeBuf); remove("out.cl0"); remove("codeBuf");
			exit(1);
		}
		
		char arg;		
		arg = (unsigned char) (lenCondBlck & 0xff);
		fwrite(&arg, sizeof(char), 1, codeBuf);
		arg = (unsigned char) (lenCondBlck >> 8);
		fwrite(&arg, sizeof(char), 1, codeBuf);
		
		fseek(codeBuf, 0, SEEK_END);
		return 1;
	}else{
		printf("NULL-Pointer popLabel st13\n");
		fclose(codeFile); fclose(codeBuf); remove("out.cl0"); remove("codeBuf");
		exit(1);
	}
}

int st12(){
		
	/*short* relAdr	= malloc(sizeof(short));
	*relAdr			= codeLen;
	listElement* newElement = malloc(sizeof(listElement));
	newElement->data		= relAdr;
	insertFrst(newElement, lablList);*/
	
	writeCode_1(jmp, 0, codeBuf);
	
	listElement* label = popFrst(lablList);
	
	if(label){
		short x = *((short*) label->data);
		short lenCondBlck = ((short)(codeLen)) - x - 3;
		
		int ret = fseek(codeBuf, x+1, SEEK_SET);
		if(ret != 0){
			printf("fseek st12 :(\n");
			fclose(codeFile); fclose(codeBuf); remove("out.cl0"); remove("codeBuf");
			exit(1);
		}
		
		char arg;		
		arg = (unsigned char) (lenCondBlck & 0xff);
		fwrite(&arg, sizeof(char), 1, codeBuf);
		arg = (unsigned char) (lenCondBlck >> 8);
		fwrite(&arg, sizeof(char), 1, codeBuf);
		
		fseek(codeBuf, 0, SEEK_END);
		
		//neu einkellern
		short* relAdr	= malloc(sizeof(short));
		*relAdr			= codeLen;
	
		listElement* newElement = malloc(sizeof(listElement));
		newElement->data		= relAdr;
	
		insertFrst(newElement, lablList);
		
		return 1;
	}else{
		printf("NULL-Pointer popLabel st12\n");
		fclose(codeFile); fclose(codeBuf); remove("out.cl0"); remove("codeBuf");
		exit(1);
	}
	
}

int st13(){
	
	listElement* label = popFrst(lablList);
		
	if(label){
		short x = *((short*) label->data);
		short lenCondBlck = ((short)(codeLen)) - x - 0;
		
		int ret = fseek(codeBuf, x-2, SEEK_SET);
		if(ret != 0){
			printf("fseek st13 1 :(\n");
			fclose(codeFile); fclose(codeBuf); remove("out.cl0"); remove("codeBuf");
			exit(1);
		}


		char arg;
		arg = (unsigned char) (lenCondBlck & 0xff);
		fwrite(&arg, sizeof(char), 1, codeBuf);
		arg = (unsigned char) (lenCondBlck >> 8);
		fwrite(&arg, sizeof(char), 1, codeBuf);
		
		ret = fseek(codeBuf, 0, SEEK_END);
		if(ret != 0){
			printf("fseek st13 2 :(\n");
			fclose(codeFile); fclose(codeBuf); remove("out.cl0"); remove("codeBuf");
			exit(1);
		}
		
		return 1;
			
	}else{
		printf("NULL-Pointer popLabel st13\n");
		fclose(codeFile); fclose(codeBuf); remove("out.cl0"); remove("codeBuf");
		exit(1);
	}
}

int st5(){

	short* relAdr	= malloc(sizeof(short));
	listElement* newElement = malloc(sizeof(listElement));
	
	if(relAdr && newElement){
		
		*relAdr			= codeLen;
		newElement->data		= relAdr;
		insertFrst(newElement, lablList);
		return 1;
	}else{
		printf("Kein Speicher :(\n");
		fclose(codeFile); fclose(codeBuf); remove("out.cl0"); remove("codeBuf");
		exit(1);
	}
}

int st6(){

	short* relAdr	= malloc(sizeof(short));
	listElement* newElement = malloc(sizeof(listElement));
	
	if(relAdr && newElement){
		
		*relAdr			= codeLen;
		newElement->data		= relAdr;
		insertFrst(newElement, lablList);
		writeCode_1(jnot, 0, codeBuf); 
		return 1;
	}else{
		printf("Kein Speicher :(\n");
		fclose(codeFile); fclose(codeBuf); remove("out.cl0"); remove("codeBuf");
		exit(1);
	}
}

int st7(){
	
	listElement* label = popFrst(lablList);
	short x = *((short*) label->data);
	int ret;
	
	if(label){
		short lenCondBlck = ((short)(codeLen)) - x;
				
		ret = fseek(codeBuf, x+1, SEEK_SET);
		if(ret != 0){ 
			printf("fseek :(\n"); 
			fclose(codeFile); fclose(codeBuf); remove("out.cl0"); remove("codeBuf");
			exit(-1); 
		}
		
		char arg;
		arg = (unsigned char) (lenCondBlck & 0xff);
		fwrite(&arg, sizeof(char), 1, codeBuf);
		arg = (unsigned char) (lenCondBlck >> 8);
		fwrite(&arg, sizeof(char), 1, codeBuf);
		
		ret = fseek(codeBuf, 0, SEEK_END);
		if(ret != 0){ 
			printf("fseek :(\n"); 
			fclose(codeFile); fclose(codeBuf); remove("out.cl0"); remove("codeBuf");
			exit(-1);	
		}
		
		label = popFrst(lablList);
		if(!label){ 
			printf("NULL-Error Labelliste\n"); 
			fclose(codeFile); fclose(codeBuf); remove("out.cl0"); remove("codeBuf");
			exit(1);
		}
		
		x = *((short*) label->data);
		lenCondBlck = ((short) codeLen) - x + 3;									
		writeCode_1(jmp, -lenCondBlck, codeBuf);
		
		return 1;
	}else return 0;
}

int st8(){ //procIdent für call

	int procIdx;
	int ret = searchProc(Morph.Val.pStr, &procIdx);
	if(ret){
		writeCode_1(call, procIdx, codeBuf);
		return 1;
	}
	
	printf("Semantikfehler: Prozedurbezeichner %s nicht vereinbart (Zeile %d, Spalte %d)\n"
		, Morph.Val.pStr, Morph.posLine, Morph.posCol);
	fclose(codeFile); fclose(codeBuf); remove("out.cl0"); remove("codeBuf");
	exit(1);
}

int st9(){ // ident für ? (Adresse wird gepusht)
	
	int displ;
	int procIdx;
	
	int ret = searchVarLocal(Morph.Val.pStr, &displ);
	if(ret){
		writeCode_1(puAdrVrLocl, displ, codeBuf);
		writeCode_0(getVal, codeBuf);
		return 1;
	}
	
	ret = searchVarGlobal(Morph.Val.pStr, &displ, &procIdx);
	if(ret){
		writeCode_2(puAdrVrGlob, displ, procIdx, codeBuf);
		writeCode_0(getVal, codeBuf);
		return 1;
	}
	
	printf("Semantikfehler: Variablenbezeichner %s nicht vereinbart (Zeile %d, Spalte %d)\n"
		, Morph.Val.pStr, Morph.posLine, Morph.posCol);
	fclose(codeFile); fclose(codeBuf); remove("out.cl0"); remove("codeBuf");
	exit(1);
}

int st10(){ //Expression für ! (Wert steht auf Stack) 

	writeCode_0(putVal, codeBuf);
	return 1;
}

int st11(){ // String für !
	
	writePutStrg(Morph.Val.pStr, codeBuf);
	return 1;
}

int pr1(){

	//schreibe Konstantenblock an das Ende der Ausgabedatei
	int i = 0;
	char arg = 0;
	long Const = 0;
	while(i < constBlockSize){
		
		Const = constBlock[i];
		arg = Const & 0xffffffffffffff; 	fwrite(&arg, sizeof(char), 1, codeFile);
		arg = (Const >> 8) & 0xffffffffffff;fwrite(&arg, sizeof(char), 1, codeFile);		
		arg = (Const >> 16) & 0xffffffffff;	fwrite(&arg, sizeof(char), 1, codeFile);
		arg = (Const >> 24) & 0xffffffff;	fwrite(&arg, sizeof(char), 1, codeFile);
		i++;
	}
	
	rewind(codeFile);
	if(codeFile == NULL){ 
		printf("Kein Speicher (Ausgabedatei) :(\n");
		fclose(codeFile); fclose(codeBuf); remove("out.cl0"); remove("codeBuf");
		exit(1);	
	}
	
	//schreibe Anzahl Prozeduren in die ersten 4 Bytes der Ausgabe
	arg = procIdx & 0xffffffffffffff; 		fwrite(&arg, sizeof(char), 1, codeFile);
	arg = (procIdx >> 8) & 0xffffffffffff;	fwrite(&arg, sizeof(char), 1, codeFile);		
	arg = (procIdx >> 16) & 0xffffffffff;	fwrite(&arg, sizeof(char), 1, codeFile);
	arg = (procIdx >> 24) & 0xffffffff;		fwrite(&arg, sizeof(char), 1, codeFile);

	return 1;
}

int ex1(){

	writeCode_0(vzMinus, codeBuf);
	return 1;
}

int ex2(){

	writeCode_0(OpAdd, codeBuf);
	return 1;
}

int ex3(){
	
	writeCode_0(OpSub, codeBuf);
	return 1;
}

int te1(){

	writeCode_0(OpMult, codeBuf);
	return 1;
}

int te2(){
	
	writeCode_0(OpDiv, codeBuf);
	return 1;	
}
