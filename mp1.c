#define LINUX

#include <linux/module.h>
#include <linux/kernel.h>
#include "mp1_given.h"

//added inclusions
#include <linux/timer.h>
#include <linux/proc_fs.h>
#include <linux/list.h>
#include <linux/gfp.h> // flags 
#include <linux/jiffies.h> 


#include <linux/workqueue.h>
#include <linux/string.h>
#include <asm/uaccess.h>
#include <linux/slab.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("lall3");
MODULE_DESCRIPTION("CS-423 MP1");

static int list_mutex =0;

void mykmod_work_handler(struct work_struct *pwork);

static struct proc_dir_entry* proc_mp1;
static struct proc_dir_entry* proc_status;
static struct workqueue_struct * _workqueue;

static DECLARE_WORK(update, mykmod_work_handler);
static struct work_struct update;

//static list_t list_pid;
typedef struct list_t{
   struct list_head node;
   char * data;
   void * voidP;
} list_t;

struct PID_list{
   struct list_head _head; 
   long cpu_time;
   long PID;
};

// my timer object
static struct timer_list _timer;

static list_t pidList;

static struct PID_list pid_list;

char * msg;


static int lock =0; //acts as spin lock

char k_buffer[2048];

//file functions
ssize_t file_write(struct file *file, char *buffer, size_t count, loff_t * data)
{
    long curr_pid=0;

    copy_from_user(k_buffer, buffer, count);
    kstrtol(k_buffer, 0 , &curr_pid);

    struct PID_list *temp;
    temp = kmalloc(sizeof( struct PID_list ), GFP_KERNEL );
    (*temp).cpu_time= (*temp).PID=0;

    while(lock);

    lock=1;
    list_add( &((*temp)._head) , &(pid_list._head) );
    lock=0;

    return count;

}

ssize_t file_read(struct file *file, char * buf, size_t count, loff_t * data)
{
  while(lock);
  lock =1;

  int length, ctr;
  ctr = length = 0;
  struct PID_list * temp;

  char * pid = kmalloc(count, GFP_KERNEL);

  list_for_each_entry(temp, &pid_list._head, _head)
  {
     length= sprintf(pid + ctr, "PID= %lu, CPU Time= %lu\n", temp->PID, temp->cpu_time  );
     ctr += length;
  }
  copy_to_user(buf, pid,ctr);

  kfree((void*)pid);
  data += ctr;
  lock=0;

}



//file system struct
//from linux channel
static const struct file_operations mp1_file_ops = {
   .owner = THIS_MODULE,
   .read = file_read,
   .write = file_write,

};


#define DEBUG 1

//functions for timer interupt handling
void timer_function(void)
{
   unsigned long unit_step = msecs_to_jiffies(5000);
   mod_timer(&_timer, jiffies + unit_step);
  //schedule_work();

schedule_work(&update);
/*
   if(!_workqueue)
      create_workqueue("_workqueue");

   struct work_struct temp;
   INIT_WORK(& temp, _worker_);
   queue_work(_workqueue, &temp);
*/
}



//initializing the timer
void timer_init(void )
{
   setup_timer(&_timer , timer_function ,0);
   mod_timer(&_timer , jiffies + msecs_to_jiffies(5000) ); //might need to add jiffies
}
//Timer event handler. Second half
void mykmod_work_handler(struct work_struct *pwork)
{
   struct PID_list* temp_Node=NULL;
    while(list_mutex);//wait if mutex=1
    list_mutex=1;//lock
    list_for_each_entry(temp_Node, &pid_list._head, _head)
    {   
       if (get_cpu_use((int)(temp_Node->PID), &(temp_Node->cpu_time)) == 0){
        printk(KERN_INFO "Successfully updated cpu times");
       } else {
        printk(KERN_INFO "Failed to update new times");
      }
    }
    list_mutex=0;//lock
}



// mp1_init - Called when module is loaded
int __init mp1_init(void)
{
   #ifdef DEBUG
   printk(KERN_ALERT "MP1 MODULE LOADING\n");
   #endif
   // Insert your code here ...

   //making directory
   //proc systems
   proc_mp1 = proc_mkdir( "MP1" ,NULL);
   proc_status = proc_create("status", 0666, proc_mp1, &mp1_file_ops);

   //error check
   if (!proc_status)
      return -ENOMEM;
   


   printk(KERN_ALERT "Timer initialized");
   
   //list
   INIT_LIST_HEAD( &(pid_list)._head );

   //time semantics
   //timer_init();
   setup_timer(&_timer , timer_function ,0);
   mod_timer(&_timer , jiffies + msecs_to_jiffies(5000) ); 

   
   
   printk(KERN_ALERT "MP1 MODULE LOADED\n");
   return 0;   
}






// mp1_exit - Called when module is unloaded
void __exit mp1_exit(void)
{
   #ifdef DEBUG
   printk(KERN_ALERT "MP1 MODULE UNLOADING\n");
   #endif
   // Insert your code here ...

   remove_proc_entry("status", proc_mp1);
   remove_proc_entry("MP1", NULL);

   del_timer(&_timer);
   //cleanup_list();
   //flush_workqueue(_workqueue);
   //destroy_workqueue(_workqueue);
   printk(KERN_ALERT "MP1 MODULE UNLOADED\n");
}

// Register init and exit funtions
module_init(mp1_init);
module_exit(mp1_exit);
