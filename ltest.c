#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<error.h>
#include<fcntl.h>
#include<sys/types.h>
#include<string.h>
#include"list.h"
int main()
{
	//int data[10]={0,1,2,3,4,5,6,7,8,9};
	int *data;
	int *sdata;
	int i;
	list *l;
	l=(list *)malloc(sizeof(list));
	
	//data=(int *)malloc(10*sizeof(int))
	
	list_init(l,free);
	for(i=0;i<10;i++){
		data=(int *)malloc(sizeof(int));
		*data=i;
		//data++;
		list_ins_next(l,NULL,data);
	}
	for(i=0;i<10;i++){
		list_rem_next(l,NULL,(int **)&sdata);
		printf("%d",*sdata);
		free(sdata);
	}
	list_destroy(l);
	free(l);

}
