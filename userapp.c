#include "userapp.h"

void waste_time (unsigned long h)
{
	if(h==0)
	return;
	
	unsigned long k=0;
	unsigned long m=0;
	while(k< h)
	{
		while(m<h)
			m++;
		m=0;
		while(m<h)
			m++;
		m=0;
		unsigned long gf=1;
		while(m< h)
			gf=gf*(m--);
		
		k++;
	}
	k=0;
	while (k<h)
		k++;
}
int main(int argc, char* argv[])
{
	unsigned long i=9999999999;
	unsigned long j=0;
	//int pid= getpid();
	//printf("PID IS %d", pid);	
	while (j<i)
	{
		j++;
		waste_time(j);
	}
	return 0;
}
