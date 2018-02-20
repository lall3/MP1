
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
#include <linux/types.h> 
#include <linux/unistd.h>

#include <linux/workqueue.h>
#include <linux/string.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/sched.h>

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



struct PID_list{
   struct list_head _head; 
   unsigned long cpu_time;
   long PID;
};

// my timer object
static struct timer_list _timer;


static struct PID_list pid_list;



static int lock =0; //acts as spin lock

char k_buffer[2048];

/*
* file write function
* param: file struct,  user buffer, count , data.
* writes to the file. Adds to the list
* return number of bytes written
*/
static ssize_t file_write(struct file *file,const  char *buffer, size_t count, loff_t * data)
  {

    printk(KERN_ALERT "WRITE FUNCTION REACHED");
    unsigned long curr_pid ;

    char * t_buffer;
    struct PID_list *temp;
    t_buffer = (char *)kmalloc(count +1, GFP_KERNEL);
    copy_from_user(t_buffer, buffer, count);
    t_buffer [count]= '\0';
    kstrtol(k_buffer, 0 , &curr_pid);
       
    temp = kmalloc(sizeof( struct PID_list ), GFP_KERNEL );
    (*temp).cpu_time=0;
    (*temp).PID= current -> pid;// getpid(); //task_pid_nr(current);

    while(lock);

    lock=1;
    list_add( &((*temp)._head) , &(pid_list._head) );
     lock=0;

     return count;

}

static ssize_t file_read(struct file *file, char * buf, size_t count, loff_t * data)
{
  int length, ctr;
  struct PID_list *temp;
  char * pid;  
  char read [256];
  int offset=0;
while(lock);
  lock =1;

  //int length, ctr;
  ctr = length = 0;
  //struct PID_list * temp;


  pid =(char *)( kmalloc(2048, GFP_KERNEL));

  list_for_each_entry(temp, &pid_list._head, _head)
  {
     length= sprintf(read, "PID= %lu, CPU Time= %lu\n", temp->PID, temp->cpu_time  );
     ctr += length;
     strcpy(pid+ offset, read);
     offset= strlen(pid);
  }
  ctr = strlen(pid)+1;
  copy_to_user(buf, pid,ctr);

  kfree((void*)pid);
  data += ctr;
  lock=0;

return ctr;
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
static void timer_function( unsigned long grbg)
{
  unsigned long unit_step = msecs_to_jiffies(500);
  schedule_work(&update);
  //queue_work(_workqueue, &mykmod_work_timer);
  mod_timer(&_timer, jiffies + unit_step);

}



//initializing the timer
void timer_init(void )
{
   setup_timer(&_timer , timer_function ,0);
   mod_timer(&_timer , jiffies + msecs_to_jiffies(500) ); //might need to add jiffies
}
//Timer event handler. Second half
void mykmod_work_handler(struct work_struct *pwork)
{
  
  unsigned long cpu_bucket;
  struct PID_list *process_entry, *temp;
while(lock);
lock =1;
//  printk(KERN_ALERT "Updating CPU times");
  list_for_each_entry_safe(process_entry, temp, &pid_list._head, _head){
    
//printk(KERN_ALERT "REAched here");

    if ( get_cpu_use( (process_entry->PID), &cpu_bucket ) == 0)
    {
      process_entry->cpu_time += cpu_bucket;
      printk(KERN_ALERT "Successfully updated cpu times");
    } 
    
    process_entry->cpu_time = jiffies_to_msecs(cputime_to_jiffies(process_entry->cpu_time));
    printk("PID: %d; CPU_TIME: %lu\n;", (int)(process_entry->PID), process_entry->cpu_time);
  }
  
lock=0;

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
   


   //printk(KERN_ALERT "Timer initialized");
   
   //list

   //time semantics
   //timer_init();
  // setup_timer(&_timer , &timer_function ,0);
  init_timer(&_timer);
  _timer.data =0;
  _timer.expires= jiffies + msecs_to_jiffies(1000);
  _timer.function = timer_function;
  add_timer(&_timer);

  mod_timer(&_timer , jiffies + msecs_to_jiffies(1000) ); 
  INIT_LIST_HEAD( &pid_list._head );
  _workqueue = create_workqueue("MP1_queue");
  //LIST_HEAD(_head);

   
   
   printk(KERN_ALERT "MP1 MODULE LOADED\n");
   return 0;   
}






// mp1_exit - Called when module is unloaded
void __exit mp1_exit(void)
{
   struct PID_list *temp1, *temp2;
   #ifdef DEBUG
   printk(KERN_ALERT "MP1 MODULE UNLOADING\n");
   #endif
   // Insert your code here ...

   //struct PID_list *temp1, * temp2;
   remove_proc_entry("status", proc_mp1);
   remove_proc_entry("MP1", NULL);

   del_timer(&_timer);
   //flush_workqueue(_workqueue);
   //destroy_workqueue(_workqueue);

  list_for_each_entry_safe(temp1, temp2, &pid_list._head, _head){
    list_del(&temp1->_head);
    kfree(temp1);

   }

   destroy_workqueue(_workqueue);

   printk(KERN_ALERT "MP1 MODULE UNLOADED\n");
}

// Register init and exit funtions
module_init(mp1_init);
module_exit(mp1_exit);
