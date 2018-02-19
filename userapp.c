#include "userapp.h"
#include <stdlib.h> 
#include <stdio.h>

void waste_time (unsigned long h)
{
	if(h==0)
	return;
	
	long fact=1;
	long i=0;
	while(i< h)
	{
		fact=fact*i;
		i=i+2;
		i-=1;
	}
}
int main(int argc, char* argv[])
{
	unsigned long i=300000;
	unsigned long j=0;
	FILE *proc_entry_fs; 
	//int pid= getpid();
	//printf("PID IS %d", pid);	
	while (j<i)
	{
		j++;
		waste_time(j);
	}


	proc_entry_fs = fopen("/proc/MP1/status", "w");
	err=fprintf (proc_entry_fs, "%s",str);
	printf("pid=%s err=%d \n",str,err);
	fclose(proc_entry_fs);

	return 0;
}
