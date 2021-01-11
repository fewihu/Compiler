//Augabefunktionen PL0

#include <string.h>

#include "write2Code.h"
#include "code.h"

int codeLen;

//putStrg und Zeichenkette -> in Codepuffer schreiben
void writePutStrg(char* string, FILE* codeBuf){
	
	int log = 0;
	char bCode = 27;
	fwrite(&bCode, sizeof(char), 1, codeBuf); 
	for(int i = 1; i < ((int)(strlen(string))-1); i++){
		bCode = *(string + i);
		fwrite(&bCode, sizeof(char), 1, codeBuf); 
		codeLen++;
		log++;
	}

	bCode = 0;	//schließt Zeichenkette ab
	fwrite(&bCode, sizeof(char), 1, codeBuf);
	codeLen += 2;
}

//Binärcodes ohne Parameter -> in Codepuffer geschrieben
void writeCode_0(tCode_0 opCode, FILE* codeBuf){
	
	char bCode = (char) opCode;
	fwrite(&bCode, sizeof(char), 1, codeBuf);  
	codeLen++;
}

//Binärcodes mit 1 Parameter -> in Codepuffer geschrieben
void writeCode_1(tCode_1 opCode, short arg1, FILE* codeBuf){

	char bCode = (char) opCode;
	char arg;

	fwrite(&bCode, sizeof(char), 1, codeBuf);
			
	arg = (unsigned char) (arg1 & 0xff);
	fwrite(&arg, sizeof(char), 1, codeBuf);
	arg = (unsigned char) (arg1 >> 8);
	fwrite(&arg, sizeof(char), 1, codeBuf);
			
	codeLen += 3;
}

//Binärcodes mit 2 Parametern -> in Codepuffer geschrieben
void writeCode_2(tCode_2 opCode, short arg1, short arg2, FILE* codeBuf){
	
	char bCode = (char) opCode;
	char arg;
	
	fwrite(&bCode, sizeof(char), 1, codeBuf);
			
	arg = (unsigned char) (arg1 & 0xff);
	fwrite(&arg, sizeof(char), 1, codeBuf);
	arg = (unsigned char) (arg1 >> 8);
	fwrite(&arg, sizeof(char), 1, codeBuf);
			
	arg = (unsigned char) (arg2 & 0xff);
	fwrite(&arg, sizeof(char), 1, codeBuf);
	arg = (unsigned char) (arg2 >> 8);
	fwrite(&arg, sizeof(char), 1, codeBuf);
			
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
