//Praktikum 2 Compiler/Interpreter


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "intrprtr.h"

static MORPHEM m;

void lex(char* pX){

	static char* px;
	
	/*Initialisierung*/
	if(pX != NULL){
		m.mc=mempty;
		px=pX;
	}
	
	/*lexikalische Analyse*/
	if(*px == '\0'){
		m.mc = mempty;
	}else{
		for(; *px == ' ' || *px == '\t'; px++);
		
		if(isdigit(*px) || *px == '.'){
			//numeral
			m.dval	= strtod (px, &px);
			m.mc	= mdbl;
		}else{
			switch(*px){
				case '+':
				case '*':
				case '(':
				case ')':
				case '-':
				case '/': m.cval = *px++; m.mc = mop;
					break;
				//end or wrong char
				default: m.mc = mempty;							
			}
		}	
	}
}

double fac(void){
	
	double tmp;
	if(m.mc == mop){
		if(m.cval == '('){
			lex(NULL);
			tmp = expr();
			if(m.mc != mop || m.cval != ')') exit(1);
			lex(NULL);
		} else exit(1);
	}else{
		if(m.mc == mdbl) {tmp = m.dval; lex(NULL);}
		else exit(1);
	}
	return tmp;
}

double term(void){
	
	double tmp = fac();
	
	while(m.mc == mop && (m.cval == '*' || m.cval == '/')){
		if(m.cval == '*'){lex(NULL); tmp *= fac();}
		else if(m.cval == '/'){lex(NULL); tmp /= fac();}
	}
	
	return tmp;
}

double expr(void){
	
	double tmp = term();
	
	while(m.mc==mop && (m.cval=='+' || m.cval=='-')){
		if(m.cval=='-'){lex(NULL); tmp -=term();}
		else if(m.cval=='+'){lex(NULL); tmp +=term();}
	}
	
	return tmp;
}



int main(int argc, char* argv[]){
	
	if(argc != 2){ printf("./a.out <Ausdruck>\n"); exit(-1); }
	lex(argv[1]);
	printf("Ergebnis: %f\n", expr());
}
