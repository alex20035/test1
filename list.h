#ifndef LIST_H
#define LIST_H

//#include<string.h>
//#include<stdlib.h>
typedef struct listelmt_
{
	void *data;
	struct listelmt_ *next;
}listelmt;

typedef struct list_
{
	int size;
	int (*match)(const void *key1,const void *key2);
	void (*destroy)(void *data);
	listelmt *head;
	listelmt *tail;
}list;

 extern void list_init(list *l,void (*destroy)(void *data));
 extern void list_destroy(list *l);
 extern int list_ins_next(list *l,listelmt *element,const void *data);
 extern int list_rem_next(list *l,listelmt *element,void **data);
#define list_size(l) ((l)->size)
#define list_head(l) ((l)->head)
#define list_tail(l) ((l)->tail)
#define list_is_head(l,element) ((element)==(l)->head?1:0)
#define list_is_tail(element) ((element)->next==NULL?1:0)
#define list_data(element) ((element)->data)
#define list_next(element) ((element)->next)

#endif

