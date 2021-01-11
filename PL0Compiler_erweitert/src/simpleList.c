//Liste PL0

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "simpleList.h"
#include "identDescr.h"

int initList(listHead* head){
	if(head != NULL){
		head->size = 0;
		return 1;
	}else return 0;
}

int insert(listElement* newElement, listHead* head){
	if((newElement != NULL) && (head != NULL)){
		if(head->size == 0){
			head->frst = newElement;
			head->curr = newElement;
			head->last = newElement;
			head->size++;
		}else{
			head->curr->nxtElement	= newElement;
			head->last				= newElement;
			head->curr				= newElement;
			head->size++;
		}
		return 1;
	}else return 0;
}

int insertFrst(listElement* newElement, listHead* head){
	
	if((newElement != NULL) && (head != NULL)){
		if(head->size == 0){
			head->frst = newElement;
			head->curr = newElement;
			head->last = newElement;
			head->size++;
		}else{
			newElement->nxtElement = head->frst;
			head->frst = newElement;
			head->curr = newElement;
			head->size++;
		}
		return 1;
	}else return 0;
}

listElement* popFrst(listHead* head){
	
	if((head != NULL) && (head->size > 0)){
		listElement* toPop = head->frst;
		head->frst = head->frst->nxtElement;
		head->curr = head->frst;
		head->size--;
		return toPop;
	}else return NULL;
}

listElement* getFirst(listHead* head){
	if(head->size > 0){
		head->curr = head->frst;
		return head->curr;
	}else return NULL;
}

listElement* getNext(listHead* head){
	
	listElement* nxt = (listElement*)head->curr->nxtElement;
	
	if(nxt != NULL){
		head->curr = nxt;
		return nxt;
	}else{
		return NULL;
	}
}

listElement* createElement(void* data){
	
	listElement* new = malloc(sizeof(listElement));
	if(new) new->data = data;
	else{ 
		printf("Kein Speicher :(\n"); 
		exit(-1); 
	}
	return new;	
}

listElement* getCurr(listHead* head){
	return head->curr;
} 



void deleteList(listHead* head){

	if(head->size > 0){
		listElement* act = getFirst(head);
		listElement* nxt;
		while(act != NULL){
			nxt = (listElement*)act->nxtElement;
			deleteIdentDescr((identDescr*)act->data);
			act = nxt;
		}
	}
	
	free(head);
}
