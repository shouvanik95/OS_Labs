#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
int main()
{
int fd, pagesize,a;
char *data;
fd = open("myfile", O_RDONLY);
pagesize = getpagesize();
getchar();   
data = mmap((caddr_t)0, pagesize, PROT_READ, MAP_SHARED, fd,pagesize);
getchar();   
printf("%d",data[0]);
getchar();   
printf("%d",data[10000]);
getchar();
return 0;
}
