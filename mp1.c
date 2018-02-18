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


MODULE_LICENSE("GPL");
MODULE_AUTHOR("lall3");
MODULE_DESCRIPTION("CS-423 MP1");



static struct proc_dir_entry* proc_mp1;
static struct proc_dir_entry* proc_status;
static struct workqueue_struct * _workqueue;


static list_t list_pid;

struct PID_list{
   struct list_head _head; 
   long cpu_time;
   long PID;
};

// my timer object
static struct timer_list _timer;



static struct PID_list pid_list;

//added functions for read and write
ssize_t file_write(struct file *filp, char *buff, size_t len, loff_t *data) {
   list_t *insert_node = (list_t*)kmalloc(sizeof(list_t), GFP_KERNEL);
   char * buf = (char*)kmalloc(count, GFP_KERNEL);
   ssize_t ret = copy_from_user(buf, buffer, count);
   insert_node->data = buf;
   list_add_tail(&(insert_node->node), &(pidList.node));
   return ret;
   
}



ssize_t mp1_read(struct file *file, char __user * buffer, size_t count, loff_t * data)
{
   int copied;
   char * buf;
   list_t *tmp = NULL;
   buf = (char*) kcalloc( 1, count, GFP_KERNEL);
   tmp = list_entry(&(pidList.node), list_t, node);
   memcpy(buf, tmp->data, strlen(tmp->data));
   copied = copy_to_user(buffer, buf, count);
   kfree(buf);
   return copied;


}

unsigned long unit_step = msecs_to_jiffies(5000);
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
   mod_timer(&_timer, jiffies + unit_step);
  //schedule_work();

   if(!_workqueue)
      create_workqueue("_workqueue");

   struct work_struct temp;
   INIT_WORK(& temp, _worker_ );
   queue_work(_workqueue, temp);
}



//initializing the timer
void timer_init(void )
{
   setup_timer(&_timer , timer_function ,0);
   mod_timer(&_timer , jiffies + msecs_to_jiffies(10000) ); //might need to add jiffies
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
   timer_init();

   
   
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
   flush_workqueue(_workqueue);
   destroy_workqueue(_workqueue);
   printk(KERN_ALERT "MP1 MODULE UNLOADED\n");
}

// Register init and exit funtions
module_init(mp1_init);
module_exit(mp1_exit);