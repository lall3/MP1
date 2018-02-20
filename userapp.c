#include "userapp.h"
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

void waste_time (unsigned long h)
{
	//if(h==0)
	//return;
	
	long fact=1;
	long i=0;
	long m,n,b,c;
	m=n=b=c=0;
	while(i< h)
	{
		fact=fact*i;
		i=i+1;
		//i-=1;
		while(m<h)
		{
			m++;
			while(c<h)
			{
				c++;
				n= m*c- n* i;
			}
		}
	}

}
int main(int argc, char* argv[])
{
	unsigned long i=15000;
	unsigned long j=0;
int err;	
FILE *proc_entry_fs; 
char str[300];
	//int pid= getpid();
	//printf("PID IS %d", pid);	
	while (j<i)
	{
		j++;
		waste_time(j);
	}


	proc_entry_fs = fopen("/proc/MP1/status", "w");
	pid_t pid= getpid();
	err=fprintf (proc_entry_fs, "%d",pid);
	

	char * msg = (char *) malloc (2048);

	ssize_t read_bytes = read(fileno(proc_entry_fs), (void *) msg , 2048);
 
	printf("pid=%d, read: %s",pid,msg);
	fclose(proc_entry_fs);

	return 0;
}

-- 
-Saurav Lall