#include "userapp.h"
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

/*
* factorial function
* param: h.
* calculates h!
* void return type
*/
void faqctorial (unsigned long h)
{
	
	long fact=1;
	long i=0;
	
	while(i< h)
	{
		fact=fact*i;
		i=i+1;
	}

}


/*
* main function
* param: args from shell.
* returns 0 on success
*/
int main(int argc, char* argv[])
{
	unsigned long i=50000;
	unsigned long j=0;
	int err;	
	FILE *proc_entry_fs; 
	
	while (j<i)
	{
		j++;
		factorial(j);
	}

	//woeking the proc system
	proc_entry_fs = fopen("/proc/mp1/status", "w");
	pid_t pid= getpid();
	

	char * msg = (char *) malloc (2048);

	ssize_t read_bytes = read(fileno(proc_entry_fs), (void *) msg , 2048);
 
	printf("pid=%d, read: %s",pid,msg);
	fclose(proc_entry_fs);

	return 0;
}

