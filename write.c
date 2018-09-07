#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
int main()
{
    int FP;
    char * MYPIPE = "/tmp/MYPIPE";

    /* CREATING NAMED PIPE */
    mkfifo(MYPIPE, 0666);

    FP = open(MYPIPE, O_WRONLY);

	time_t current_time;
	struct tm *time_info;
	char timeString[100]; 

	time(&current_time);
	time_info = localtime(&current_time);
	strftime(timeString, sizeof(timeString), "%Y/%m/%d and %H:%M:%S", time_info);
 
	write(FP, timeString , sizeof(timeString));
    close(FP);

    unlink(MYPIPE);

    return 0;
}

