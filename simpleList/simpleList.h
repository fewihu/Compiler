//Felix Müller 18-041-61 
//Parser PL0

#ifndef LIST_H
#define LIST_H


typedef struct ELEMENT{
	void* nxtElement;
	void* data;	
}listElement;

typedef struct HEAD{
	listElement* frst;
	listElement* last;
	listElement* curr;
	int size;
}listHead;

int initList(listHead*);
int insert(listElement*, listHead*);
listElement* getFirst(listHead*);
listElement* getNext(listHead*);
listElement* createElement(void*);
listElement* getCurr(listHead*);
void deleteList(listHead*);	

#endif
