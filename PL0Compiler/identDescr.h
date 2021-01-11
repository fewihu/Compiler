//Idents PL0

#ifndef DESCR_H
#define DESCR_H

#include "simpleList.h"

typedef enum IDENT_TYPE{
	identVar, identConst, identProc
}tIdentType;

//Daten eines Bezeichners
typedef struct DATA_DESCR{
	tIdentType identType;
	char* name;
	void* pObj;	//zeigt auf varDescr/constDescr/procDescr
}identDescr;

//Beschreibung einer Variablen
typedef struct DATA_VAR{
	int displacement;
}varDescr;

//Beschreibung einer Konstanten
typedef struct DATA_CONST{
	long val;
	int idx;
}constDescr;

//Beschreibung einer Prozedur
typedef struct DATA_PROC{
	int idx;
	struct procDescr* prntProc;	//übergeordnete Prozedur
	listHead* localNameList;	//Namesliste der prozedurlokalen Bezeichner
	int	memAllocCount;
}procDescr;


identDescr* createIdentDescr(tIdentType, char*);
varDescr*	createVarDescr(int);
constDescr* createConstDescr(long, int);
procDescr*	createProcDescr(int, procDescr*);
void		deleteIdentDescr(identDescr*);

#endif
