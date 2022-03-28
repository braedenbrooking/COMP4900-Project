#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/neutrino.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>


#define MAX_STRING_LEN		256
#define ORDER_OF_MAGNITUDE	3
#define NUMBER_OF_RUNS		1000000
#define BILLION				1000000000

void server(int,int);
void client(int,int,int);

int main(int argc, char **argv)
{
	int pid;
	int first_pipefd[2];
	int second_pipefd[2];

	if(pipe(first_pipefd) || pipe(second_pipefd)){
		printf("Error opening pipe\n");
		exit(-1);
	}

	pid = fork();

	if(pid == 0){
		server(first_pipefd[0], second_pipefd[1]);
	}else{
		client(first_pipefd[1], second_pipefd[0], pid);
	}

	close(first_pipefd[0]);
	close(first_pipefd[1]);
	close(second_pipefd[0]);
	close(second_pipefd[1]);



	return 0;
}

void server(int read_pipe, int write_pipe){
	printf("Server Ready\n");
	int len = (int) pow(10, ORDER_OF_MAGNITUDE); // E.g., 1000 bytes.
	char *str = malloc(len + 1);

	while(1){
		read(read_pipe, str, len);
		write(write_pipe, str, len);
	}
}

void client(int write_pipe, int read_pipe, int server_pid){
	printf("Client Ready\n");
	int len = (int) pow(10, ORDER_OF_MAGNITUDE); // E.g., 1000 bytes.
	char *str = malloc(len + 1);
	memset(str, '*', len);
	str[len] = '\0';

	struct timespec t1,t2;
	if(clock_gettime(CLOCK_REALTIME, &t1) == -1) {
		perror("clock gettime");
		exit(-1);
	}

	for(int i = 0; i < NUMBER_OF_RUNS; i++) {
		write(write_pipe, str, len);
		read(read_pipe, str, len);
	}

	if(clock_gettime(CLOCK_REALTIME, &t2) == -1) {
		perror("clock gettime");
		exit(-1);
	}


	double TP = (t2.tv_sec - t1.tv_sec) + ((double) (t2.tv_nsec + t1.tv_nsec)) / BILLION;
	double result = TP/NUMBER_OF_RUNS/2;

	printf("%d B @ %lf s\n", len, result);

	kill(server_pid, SIGKILL);
}
