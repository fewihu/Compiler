#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "semRoutinen.h"
#include "identDescr.h"
#include "lex.h"
#include "simpleList.h"

int procIdx 	= 1; //0 ist das Hauptprogramm

tMorph Morph;			//in lex.c als extern deklariert

procDescr* currProc;	//in bogen.c als extern deklariert	

long* constBlock;		// -"-		
int constBlockSize;		// -"-

//TODO varCount durch int MemAlloc in procDescr ersetzen

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

int bl1(){ // --ident-->  (const)

	//wenn prozedurlokale Suche nach Bezeichner ok, dann
	//lege Bezeichnerdaten an, Konstantenbeschreibung in bl2
	if(searchLocal(currProc, Morph.Val.pStr)){
			identDescr* newIdent	= createIdentDescr(identConst, Morph.Val.pStr);
			listElement* newElement	= createElement(newIdent);	
			insert(newElement, currProc->localNameList);
			printf("Konstante %s akzeptiert", Morph.Val.pStr);		
			return 1;
	}else return 0;			
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
			printf(", Konstantenwert %ld schon vorhanden, setze Index\n", Morph.Val.Num);
			break;
		}
		i++;
	}
	
	if(!found){
		//nicht gefunden? -> Konstantenblock erweitern und Index setzen
		constBlock = realloc(constBlock, (constBlockSize + 1) * sizeof(long));
		if(constBlock){
			*(constBlock + (constBlockSize * sizeof(long))) = Morph.Val.Num;
			newConst	= createConstDescr(Morph.Val.Num, constBlockSize);	
			constBlockSize++;
			printf(", Konstantenwert %ld in Konstantenblock eingeügt\n", Morph.Val.Num);	
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
	
	//wenn prozedurlokale Suche nach Bezeichner ok, dann 
	//lege Bezeichnerdaten und Variablenbeschreibung an		
	if(searchLocal(currProc, Morph.Val.pStr)){
			identDescr* newIdent	= createIdentDescr(identVar, Morph.Val.pStr);
			
			varDescr* newVar		= createVarDescr(currProc->memAllocCount);
			currProc->memAllocCount += 4;
			
			newIdent->pObj = newVar;
			listElement* newElement	= createElement(newIdent);	
			insert(newElement, currProc->localNameList);
			printf("Variable %s akzeptiert\n", Morph.Val.pStr);
			return 1;
	}else return 0;
}


int bl4(){ // --ident--> (procedure)
	
	//wenn prozedurlokale Suche nach Bezeichner ok, dann 
	//lege Bezeichnerdaten an, Prozedurbeschreibung in bl5
	if(searchLocal(currProc, Morph.Val.pStr)){
		identDescr* newIdent	= createIdentDescr(identProc, Morph.Val.pStr);
		procDescr* newProc		= createProcDescr(procIdx,currProc);		
		listElement* newElement	= createElement(newIdent);	
		insert(newElement, currProc->localNameList);
		
		//globale Variablen setzen
		currProc				= newProc;	//umgebende Proc ist neue Proc
		procIdx++;	
		
		printf("Prozedurbezeichner %s akzeptiert\n", Morph.Val.pStr);
		return 1;
	}else return 0;
}

int bl5(){ // --;--> (schließt Block von procedure ab)

	//=== DEBUG ===
	
	printf("------------------------\nProzedurindex: %d\nSpeicherplatzzuordnungszähler: %d\n", currProc->idx, currProc->memAllocCount);
	
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
		
	printf("umgebende Prozedur: %d\n", ((procDescr*)currProc->prntProc)->idx);
	
	//=== DEBUG ===
	
	//lokale Namensliste nicht mehr gebraucht, übergeordnete wird umgebende Prozedur
	deleteList(currProc->localNameList);
	struct procDescr* prntProc = currProc->prntProc;
	currProc = (procDescr*) prntProc;
	
	//Hier muss später der erzeugt Code der Prozedur rausgeschrieben werden
	
	return 1;
}
