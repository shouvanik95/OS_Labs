#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/time.h>

int fd[25], pagesize,a;
FILE* f_read[25];
char *data;


void one()
{
	for(a=0;a<25;a++)
	{
		char *filename;
		asprintf(&filename, "%d", a);
		fd[a] = open(filename, O_RDONLY);
		data = mmap((caddr_t)0, 10485760, PROT_READ, MAP_SHARED, fd[a],0);
		int b;
		for(b=0;b<10485760;b++)
		{
			char x = data[b];
		}
		free(filename);
	}
}
void five()
{
	for(a=0;a<25;a++)
	{
		char *filename;
		asprintf(&filename, "%d", a);
		fd[a] = open(filename, O_RDWR);
		data = mmap((caddr_t)0, 10485760, PROT_WRITE, MAP_SHARED, fd[a],0);
		int b;
		for(b=0;b<10485760;b++)
		{
			data[b]='b';
		}
		free(filename);
		munmap(data,10485760);
	}
}
void two()
{
	for(a=0;a<25;a++)
	{
		char *filename;
		asprintf(&filename, "%d", a);
		
		f_read[a] = fopen(filename, "r");
		char buf[512]; 
		
		while (fread(buf, 512, 1, f_read[a]) == 1) { 
		} 
		free(filename);
	}
}

void six()
{
	for(a=0;a<25;a++)
	{
		char *filename;
		asprintf(&filename, "%d", a);
		
		fd[a] = open(filename, O_RDWR);
		char buf[512]; 
		int i;
		for(i=0;i<512;i++){buf[i]='a';}
		for(i=0;i<1024*1024*10;i+=512)
		{
			write(fd[a],buf,(size_t)512);
		}
	}
}

int main()
{
struct timeval init, fin;
gettimeofday(&init,NULL); //initial time
pagesize = getpagesize();
two();
gettimeofday(&fin,NULL);
printf("%d\n",pagesize);
printf("%f\n",25.0*10485760/((fin.tv_sec-init.tv_sec)*1000000+(fin.tv_usec-init.tv_usec)));
return 0;
}
