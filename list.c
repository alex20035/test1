#include "list.h"

void list_init(list *l,void (*destroy)(void *data))
{
	l->size=0;
	l->destroy=destroy;
	l->head=NULL;
	l->tail=NULL;
	return;
}

void list_destroy(list *l)
{
	void *data;
	while(list_size(l)){
		if(list_rem_next(l,NULL,(void **)&data
)==0 && l->destroy!=NULL){
			l->destroy(data);
			}
	}

	memset(l,0,sizeof(data));
	return ;
}

int list_ins_next(list *l,listelmt *element,const void *data)
{
	listelmt *new_element;
	if((new_element=(listelmt *)malloc(sizeof(listelmt)))==NULL)
		return -1;
	new_element->data=(void *)data;
	if(element==NULL){
		if(list_size(l)==0)
			l->tail=new_element;
		new_element->next=l->head;
		l->head=new_element;
		
	}
	else{
		if(element->next==NULL)
		l->tail=new_element;
		new_element->next=element->next;
		element->next=new_element;
	}
	l->size++;
	return 0;
	
}

int list_rem_next(list *l,listelmt *element,void **data)
{
	listelmt *old_element;
	if(list_size(l)==0)
		return -1;
	if(element==NULL){
			*data=l->head->data;
			old_element=l->head;
			l->head=l->head->next;
			if(list_size(l)==1)
				l->tail=NULL;
	}
	else{
		if(element->next==NULL)
			return -1;
		*data=element->next->data;
		old_element=element->next;
		element->next=element->next->next;
		if(element->next==NULL)
			l->tail=element;
	}

	free(old_element);
	l->size--;
	return 0;
}

