#ifndef _SCULL_H_
#define _SCULL_H_
#include<linux/ioctl.h>
#ifndef SCULL_MAJOR
#define SCULL_MAJOR 0
#endif

#ifndef SCULL_NR_DEVS
#define SCULL_NR_DEVS 4 //SCULL0~SCULL3
#endif



struct scull_dev{
	struct list_ *l;
	unsigned long size;
	unsigned int access_key;
	struct semaphore sem;
	struct cdev cdev;
};
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

#define list_size(l) ((l)->size)
#define list_head(l) ((l)->head)
#define list_tail(l) ((l)->tail)
#define list_is_head(l,element) ((element)==(l)->head?1:0)
#define list_is_tail(element) ((element)->next==NULL?1:0)
#define list_data(element) ((element)->data)
#define list_next(element) ((element)->next)

#define TYPE(minor) ((minor)>>4)&&0xff)
#define NUM(minor) ((minor)&0xf)

extern int scull_major; 
extern int scull_nr_devs;


ssize_t scull_read(struct file *filep,char __user *buf,size_t count,loff_t *offp);
ssize_t scull_write(struct file *filep,const char __user *buff,size_t count,loff_t *offp);
loff_t scull_llseek(struct file *filep,loff_t off,int whence);
extern void list_init(struct list_ *l,void (*destroy)(void *data));
 extern void list_destroy(struct list_ *l);
 extern int list_ins_next(struct list_ *l,listelmt *element,const void *data);
 extern int list_rem_next(struct list_ *l,listelmt *element,void **data);

#define SCULL_IOC_MAXNR 14
#endif


