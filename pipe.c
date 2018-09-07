#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void main(void){
	int pipe_fd[2];
	int ret = pipe(pipe_fd);
	pid_t pid;
	char buf[20];

	if (ret == -1){
		perror("pipe failed");
	}

	pid = fork();

	if (pid == 0){// child
		close(pipe_fd[1]);

		// sleep(2);
		// read(pipe_fd[0], buf, 5);
		// printf("read from char array :%s\n", buf);


		char b;
		printf("\nread one byte each time\n");
		while(read(pipe_fd[0], &b, 1)){
			printf("%c", b);
		}
			printf("\nChild exiting\n");
			exit(1);

	}else{// parent
		close(pipe_fd[0]);
		write(pipe_fd[1], "ahbar", 5);
	}

	printf("Parent exiting\n");
	exit(1);

}