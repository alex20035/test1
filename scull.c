#include<linux/module.h>
#include<linux/moduleparam.h>
#include<linux/init.h>

#include<linux/kernel.h>
#include<linux/slab.h>
#include<linux/fs.h>
#include<linux/errno.h>
#include<linux/types.h>
#include<linux/proc_fs.h>
#include<linux/fcntl.h>
#include<linux/seq_file.h>
#include<linux/cdev.h>

//#include<asm/system.h>
#include<asm/uaccess.h>

#include "scull.h"

int scull_major=SCULL_MAJOR;
int scull_minor=0;
int scull_nr_devs=SCULL_NR_DEVS;


module_param(scull_major,int,S_IRUGO);
module_param(scull_minor,int,S_IRUGO);
module_param(scull_nr_devs,int,S_IRUGO);

MODULE_LICENSE("Dual BSD/GPL");

struct scull_dev *scull_devices;



void list_init(struct list_ *l,void (*destroy)(void *data))
{
	l->size=0;
	l->destroy=destroy;
	l->head=NULL;
	l->tail=NULL;
	return;
}

void list_destroy(struct list_ *l)
{
	void *data;
	while(list_size(l)){
		if(list_rem_next(l,NULL,(void **)&data
)==0 && l->destroy!=NULL){
			l->destroy(data);
			}
	}

	memset(l,0,sizeof(l));
	return ;
}

int list_ins_next(struct list_ *l,listelmt *element,const void *data)
{
	listelmt *new_element;
	if((new_element=(listelmt *)kmalloc(sizeof(listelmt),GFP_KERNEL))==NULL)
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

int list_rem_next(struct list_ *l,listelmt *element,void **data)
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

	kfree(old_element);
	l->size--;
	return 0;
}




int scull_open(struct inode *inode,struct file *filep)
{
	struct scull_dev *dev;
	dev=container_of(inode->i_cdev,struct scull_dev,cdev);
	filep->private_data=dev;
	if((filep->f_flags&O_ACCMODE)==O_WRONLY){
		list_init(dev->l,kfree);		
	}
	return 0;
}

int scull_release(struct inode *inode,struct file *filep)
{
	//list_destroy(l);
	return 0;
}



ssize_t scull_read(struct file *filep,char __user *buf,size_t count,loff_t *f_pos)
{
	struct scull_dev *dev=filep->private_data;
	int *sdata,i;	
	ssize_t retval=0;
	
	//if(down_interruptible(&dev->sem))
	//	return -ERESTARTSYS;
	if(*f_pos>=dev->size)
		goto out;
	if(*f_pos+count>dev->size)
		count=dev->size-*f_pos;
	for(i=0;i<count;i++){
		list_rem_next(dev->l,NULL,(int **)&sdata);
		printk(KERN_ALERT "read someting in kernel");
		kfree(sdata);
	}

	
	*f_pos+=count;
	retval=count;
	out:
	//up(&dev->sem);
	return retval;
}
ssize_t scull_write(struct file *filep,const char __user *buf,size_t count,loff_t *f_pos)
{
	struct scull_dev *dev=filep->private_data;
	
	int *data,i;
	
	ssize_t retval=-ENOMEM;
	/*	
	if(down_interruptible(&dev->sem))
		return -ERESTARTSYS;
	*/
	for(i=0;i<count;i++){
		data=(int *)kmalloc(sizeof(int),GFP_KERNEL);
		
		*data=i;
		list_ins_next(dev->l,NULL,data);	
		printk(KERN_ALERT "write someting in kernel");
	}
	*f_pos+=count;
	retval=count;
	if(dev->size<*f_pos)
		dev->size=*f_pos;
	out:
	//up(&dev->sem);
	return retval;	
}

loff_t scull_llseek(struct file *filep,loff_t off,int whence)
{
	struct scull_dev *dev=filep->private_data;
	loff_t newpos;

	switch(whence){
	case 0:
	newpos=off;
	break;

	case 1:
	newpos=filep->f_pos-off;
	break;
	
	case 2:
	newpos=dev->size+off;
	break;
	
	default:
	return -EINVAL;	
	}
	if(newpos<0) return -EINVAL;
	filep->f_pos=newpos;
	return newpos;

}

struct file_operations scull_fops={
.owner=	THIS_MODULE,
.llseek=	scull_llseek,
.read=	scull_read,
.write=	scull_write,
.open=	scull_open,
.release=	scull_release,
};

void scull_cleanup_module(void)
{
	int i;
	dev_t devno=MKDEV(scull_major,scull_minor);
	if(scull_devices){
		for(i=0;i<scull_nr_devs;i++){
			//scull_trim(scull_devices+1);
			list_destroy(scull_devices[i].l);
			kfree(scull_devices[i].l);
			cdev_del(&scull_devices[i].cdev);
		}
	kfree(scull_devices);
	}	
	unregister_chrdev_region(devno,scull_nr_devs);
}

static void scull_setup_cdev(struct scull_dev *dev,int index)
{
	
	int err,devno=MKDEV(scull_major,scull_minor+index);
	cdev_init(&dev->cdev,&scull_fops);
	dev->cdev.owner=THIS_MODULE;
	dev->cdev.ops=&scull_fops;
	err=cdev_add(&dev->cdev,devno,1);
	dev->l=(list *)kmalloc(sizeof(list),GFP_KERNEL);
			if(dev->l==NULL)
				printk(KERN_NOTICE "Error adding scull ");
	//device_create(module_class,NULL,dev,NULL,"scull")
	memset(dev->l,0,sizeof(struct list_));
	if(err)
		printk(KERN_NOTICE "Error %d adding scull %d",err,index);
}

int scull_init_module(void)
{
	int result,i;
	dev_t dev=0;

	//module_class=class_create(THIS_MODULE,"scull");//auto create nod

	if(scull_major){
		dev=MKDEV(scull_major,scull_minor);
		result=register_chrdev_region(dev,scull_nr_devs,"scull");

	}else{
		result=alloc_chrdev_region(&dev,scull_minor,scull_nr_devs,"scull");
		scull_major=MAJOR(dev);
	}
	if(result<0){
		printk(KERN_WARNING "scull:can't get major %d /n",scull_major);
		return result;
	}
	scull_devices=kmalloc(scull_nr_devs*sizeof(struct scull_dev),GFP_KERNEL);
	if(!scull_devices){
		result=-ENOMEM;
		goto fail;
	}
	memset(scull_devices,0,scull_nr_devs*sizeof(struct scull_dev));
	printk(KERN_ALERT "The length of scull_dev is %d",sizeof(struct scull_dev));
	for(i=0;i<scull_nr_devs;i++){
		//init_MUTEX(&scull_devices[i].sem);
		scull_setup_cdev(&scull_devices[i],i);
		
		
	}
	
	dev=MKDEV(scull_major,scull_minor+scull_nr_devs);
	return 0;
	fail:
	scull_cleanup_module();
	return result;
	
}

module_init(scull_init_module);
module_exit(scull_cleanup_module);

