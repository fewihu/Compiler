//Semantikroutinen PL0

#ifndef SEMR_H
#define SEMR_H

#include "identDescr.h"

//Suchfunktionen
int searchLocal(	procDescr*, char*); //Bezeichner in Prozedur
int searchVarLocal(	char*, int*);       //Variable in Prozedur -> liefert Displacement
int searchVarGlobal(char*, int*, int*); //Variable global -> liefert Prozedurindex + Displacement
int searchConstant(	char*, int*);		//Konstante -> liefert Index
int searchProc(		char*, int*);		//Prozedur -> leifert Index

//Block
int bl1();
int bl2();
int bl3();
int bl4();
int bl5();
int bl6();

//Factor
int fa1();
int fa2();

//Statement
int st1();
int st2();
int st3();
int st4();
int st5();
int st6();
int st7();
int st8();
int st9();
int st10();
int st11();
int st12();
int st13();

//Program
int pr1();

//Expression
int ex1();
int ex2();
int ex3();

//Term
int te1();
int te2();

//condition
int co1();
int co2();
int co3();
int co4();
int co5();
int co6();
int co7();
int co8();

#endif
