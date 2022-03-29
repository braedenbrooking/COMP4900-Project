// Asynchronous message passing test.
// Calculates the duration of a pipe message transfer (TP).

#include <math.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/neutrino.h>

#define ORDER_OF_MAGNITUDE	0 // Tests: 0, 1, 2, 3, 4.
#define NUMBER_OF_RUNS		10E6
#define BILLION				1E9

void server(int,int);
void client(int,int,int);

int main(int argc, char **argv) {
	int pid;
	int first_pipefd[2];
	int second_pipefd[2];

	if (pipe(first_pipefd) || pipe(second_pipefd)) {
		perror("pipe");
		exit(-1);
	}

	fcntl(first_pipefd[0], F_SETFL, O_NONBLOCK);
	fcntl(first_pipefd[1], F_SETFL, O_NONBLOCK);
	fcntl(second_pipefd[0], F_SETFL, O_NONBLOCK);
	fcntl(second_pipefd[1], F_SETFL, O_NONBLOCK);

	pid = fork();

	if (pid == 0) {
		server(first_pipefd[0], second_pipefd[1]);
	} else {
		client(first_pipefd[1], second_pipefd[0], pid);
	}

	close(first_pipefd[0]);
	close(first_pipefd[1]);
	close(second_pipefd[0]);
	close(second_pipefd[1]);

	return 0;
}

void server(int read_pipe, int write_pipe) {
	printf("Server ready (pid = %d).\n", getpid());
	int len = (int)pow(10, ORDER_OF_MAGNITUDE);
	char *str = malloc(len + 1);

	while(1) {
		read(read_pipe, str, len);
		write(write_pipe, str, len);
	}
}

void client(int write_pipe, int read_pipe, int server_pid) {
	printf("Client ready (server pid = %d).\n", server_pid);

	struct timespec t0, t1;
	int len = (int)pow(10, ORDER_OF_MAGNITUDE);
	char *str = malloc(len + 1);

	memset(str, '*', len);
	str[len] = '\0';

	if (clock_gettime(CLOCK_REALTIME, &t0) == -1) {
		perror("clock_gettime");
		exit(-1);
	}

	for (int i = 0; i < NUMBER_OF_RUNS; i++) {
		write(write_pipe, str, len);
		read(read_pipe, str, len);
	}

	if (clock_gettime(CLOCK_REALTIME, &t1) == -1) {
		perror("clock_gettime");
		exit(-1);
	}

	double t = (double)((t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / BILLION);
	double TP = (t / NUMBER_OF_RUNS / 2) * 1000000;

	printf("%d B @ %lf us\n", len, TP);
	kill(server_pid, SIGKILL);
}
