//Idents PL0

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "identDescr.h"

//erzeugt neue Identbeschreibung
identDescr* createIdentDescr(tIdentType identType, char* name){
	
	identDescr* new	= malloc(sizeof(identDescr));
	if(new){
		new->name = malloc(sizeof(char*));
		if(new->name){
			strcpy(new->name,name);
			new->identType = identType;
			return new;
		}else{
			printf("Kein Speicher :(\n");
			exit(-1);
		}
	}else{
		printf("Kein Speicher :(\n");
		exit(-1);
	}
}

//erzeugt neue Variablenbeschreibung
varDescr* createVarDescr(int displacement){
	
	varDescr * new = malloc(sizeof(varDescr));
	if(new){
		new->displacement = displacement;
		return new;
	}else{
		printf("Kein Speicher :(\n");
		exit(-1);
	}
}

//erzeugt neue Konstantenbeschreibung
constDescr* createConstDescr(long val, int idx){
	
	constDescr* new = malloc(sizeof(constDescr));
	if(new){
		new->val = val;
		new->idx = idx;
		return new;
	}else{
		printf("Kein Speicher :(\n");
		exit(-1);
	}
}

//erzeugt neue Prozedurbeschreibung mit lokaler Namensliste
procDescr* createProcDescr(int idx, procDescr* prntProc){
	
	procDescr* new = malloc(sizeof(procDescr));
	if(new){
		new->idx			= idx;
		new->prntProc		= (struct procDescr*)prntProc;
		new->memAllocCount	= 0;
		new->localNameList	= malloc(sizeof(listHead));
		initList(new->localNameList);
		return new;
	}else{
		printf("Kein Speicher :(\n");
		exit(-1);
	}
}

//entfernt eine Identbeschreibung aus dem Speicher
void deleteIdentDescr(identDescr* pDescr){

	free(pDescr->pObj);
	free(pDescr->name);
	free(pDescr);
}
