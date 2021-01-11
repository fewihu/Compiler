//Augabefunktionen PL0

#include "write2Code.h"
#include "code.h"

int codeLen;

//Binärcodes ohne Parameter -> in Codepuffer geschrieben
void writeCode_0(tCode_0 opCode, FILE* codeBuf){
	
	char bCode;
	switch(opCode){
		case retProc:
			bCode = 23;
			fwrite(&bCode, sizeof(char), 1, codeBuf);  
			break;
			
		case putVal:
			bCode = 8;
			fwrite(&bCode, sizeof(char), 1, codeBuf);  
			break;
		
		case vzMinus:
			bCode = 10;
			fwrite(&bCode, sizeof(char), 1, codeBuf);  
			break;
		
		case OpAdd:
			bCode = 12;
			fwrite(&bCode, sizeof(char), 1, codeBuf);  
			break;
			
		case OpSub:
			bCode = 13;
			fwrite(&bCode, sizeof(char), 1, codeBuf);  
			break;
			
		case OpMult:
			bCode = 14;
			fwrite(&bCode, sizeof(char), 1, codeBuf);
			break;
		
		case OpDiv:
			bCode = 15;
			fwrite(&bCode, sizeof(char), 1, codeBuf);
			break;
			
		case storeVal:
			bCode = 7;
			fwrite(&bCode, sizeof(char), 1, codeBuf);
			break;
			
		case getVal:
			bCode = 9;
			fwrite(&bCode, sizeof(char), 1, codeBuf);
			break;
		
		case odd: 
			bCode = 11;
			fwrite(&bCode, sizeof(char), 1, codeBuf);
			break;	
			
		case cmpEQ:
			bCode = 16;
			fwrite(&bCode, sizeof(char), 1, codeBuf);
			break;
		
		case cmpNE:
			bCode = 17;
			fwrite(&bCode, sizeof(char), 1, codeBuf);
			break;
			
		case cmpLT:
			bCode = 18;
			fwrite(&bCode, sizeof(char), 1, codeBuf);
			break;
			
		case cmpGT:
			bCode = 19;
			fwrite(&bCode, sizeof(char), 1, codeBuf);
			break;
			
		case cmpLE:
			bCode = 20;
			fwrite(&bCode, sizeof(char), 1, codeBuf);
			break;
			
		case cmpGE:
			bCode = 21;
			fwrite(&bCode, sizeof(char), 1, codeBuf);
			break;		
			
		default: break;
	}
	codeLen++;
}

//Binärcodes mit 1 Parameter -> in Codepuffer geschrieben
void writeCode_1(tCode_1 opCode, short arg1, FILE* codeBuf){

	char bCode;
	char arg;
	switch(opCode){
		case puConst:
			bCode = 6;
			fwrite(&bCode, sizeof(char), 1, codeBuf);
			
			arg = (unsigned char) (arg1 & 0xff);
			fwrite(&arg, sizeof(char), 1, codeBuf);
			arg = (unsigned char) (arg1 >> 8);
			fwrite(&arg, sizeof(char), 1, codeBuf);
			
			break;
			
		case puValVrLocl:
			bCode = 0;
			fwrite(&bCode, sizeof(char), 1, codeBuf);
			
			arg = (unsigned char) (arg1 & 0xff);
			fwrite(&arg, sizeof(char), 1, codeBuf);
			arg = (unsigned char) (arg1 >> 8);
			fwrite(&arg, sizeof(char), 1, codeBuf);
			
			break;
			
		case puAdrVrLocl:
			bCode = 3;
			fwrite(&bCode, sizeof(char), 1, codeBuf);
			
			arg = (unsigned char) (arg1 & 0xff);
			fwrite(&arg, sizeof(char), 1, codeBuf);
			arg = (unsigned char) (arg1 >> 8);
			fwrite(&arg, sizeof(char), 1, codeBuf);
			
			break;
		case call:
			bCode = 22;
			fwrite(&bCode, sizeof(char), 1, codeBuf);
			
			arg = (unsigned char) (arg1 & 0xff);
			fwrite(&arg, sizeof(char), 1, codeBuf);
			arg = (unsigned char) (arg1 >> 8);
			fwrite(&arg, sizeof(char), 1, codeBuf);
			break;
			
		case jnot:
			bCode = 25;
			fwrite(&bCode, sizeof(char), 1, codeBuf);
			
			arg = (unsigned char) (arg1 & 0xff);
			fwrite(&arg, sizeof(char), 1, codeBuf);
			arg = (unsigned char) (arg1 >> 8);
			fwrite(&arg, sizeof(char), 1, codeBuf);
			
			break;
			
		case jmp:
			bCode = 24;
			fwrite(&bCode, sizeof(char), 1, codeBuf);
			
			arg = (unsigned char) (arg1 & 0xff);
			fwrite(&arg, sizeof(char), 1, codeBuf);
			arg = (unsigned char) (arg1 >> 8);
			fwrite(&arg, sizeof(char), 1, codeBuf);
			
			break;
						
		default: break;
	}
	codeLen += 3;
}

//Binärcodes mit 2 Parametern -> in Codepuffer geschrieben
void writeCode_2(tCode_2 opCode, short arg1, short arg2, FILE* codeBuf){
	
	char bCode;
	char arg;
	
	switch(opCode){
		case puValVrGlob:
			bCode = 2;
			fwrite(&bCode, sizeof(char), 1, codeBuf);
			
			arg = (unsigned char) (arg1 & 0xff);
			fwrite(&arg, sizeof(char), 1, codeBuf);
			arg = (unsigned char) (arg1 >> 8);
			fwrite(&arg, sizeof(char), 1, codeBuf);
			
			arg = (unsigned char) (arg2 & 0xff);
			fwrite(&arg, sizeof(char), 1, codeBuf);
			arg = (unsigned char) (arg2 >> 8);
			fwrite(&arg, sizeof(char), 1, codeBuf);
			
			break;
		
		case puAdrVrGlob: 
			bCode = 5;
			
			fwrite(&bCode, sizeof(char), 1, codeBuf);
			
			arg = (unsigned char) (arg1 & 0xff);
			fwrite(&arg, sizeof(char), 1, codeBuf);
			arg = (unsigned char) (arg1 >> 8);
			fwrite(&arg, sizeof(char), 1, codeBuf);
			
			arg = (unsigned char) (arg2 & 0xff);
			fwrite(&arg, sizeof(char), 1, codeBuf);
			arg = (unsigned char) (arg2 >> 8);
			fwrite(&arg, sizeof(char), 1, codeBuf);
			
			break;
			
		
		default: break;
	}
	codeLen += 5;
}

//EntryProc und Parameter wird direkt in Ausgabedatei geschrieben
void writeEPC(short arg1, short arg2, short arg3, FILE* test){
	
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
