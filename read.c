#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_BUF 1024

int main()
{
    int FP;
    char * MYPIPE = "/tmp/MYPIPE";
    char buffer[MAX_BUF];
    FP = open(MYPIPE, O_RDONLY);
    read(FP, buffer, MAX_BUF);
    printf("DATE & TIME IS: %s\n", buffer);
    close(FP);
    return 0;
}
