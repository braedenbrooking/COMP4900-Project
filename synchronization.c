// Task synchronization and event handling.

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/neutrino.h>

#define COUNT	5
#define BILLION	1E9

void handler();
void client(pid_t);
void server(pid_t);

int main(int argc, char **argv) {
	int pid, ppid;

	signal(SIGUSR1, handler);

	pid = fork();
	ppid = getppid();

	if (pid == -1) {
		perror("fork");
		exit(-1);
	}

	if (pid == 0) {
		// Child.
		client(ppid);
	} else {
		// Parent.
		server(pid);
	}

	return 0;
}

void client(pid_t server_pid) {
	printf("Client ready (server_pid = %d).\n", server_pid);

	sigset_t set;
	struct timespec t0, t1;

	sigemptyset(&set);

	if (clock_gettime(CLOCK_REALTIME, &t0) == -1) {
		perror("clock_gettime");
		exit(-1);
	}

	for (int i = 0; i < COUNT; i++) {
		sleep(1);
		kill(server_pid, SIGUSR1);
		printf("Sent SIGUSR1 to server.\n");
		printf("Client suspending...\n");
		sigsuspend(&set);
	}

	if (clock_gettime(CLOCK_REALTIME, &t1) == -1) {
		perror("clock_gettime");
		exit(-1);
	}

	double TT = (t1.tv_sec - t0.tv_sec) + ((double) (t1.tv_nsec + t0.tv_nsec)) / BILLION;

	// COUNT * 2 is the total number of seconds slept (i.e.,
	// client and server each sleep 1s for every run). Subtract
	// this amount from the total time taken to get the actual
	// amount of time teken by the signal passing.
	double result = (TT - (COUNT * 2)) / COUNT / 2;

	printf("%lf s\n", result);
}

void server(pid_t client_pid) {
	printf("Server ready (client_pid = %d).\n", client_pid);

	sigset_t set;

	sigemptyset(&set);

	while (1) {
		printf("Server suspending...\n");
		sigsuspend(&set);
		sleep(1);
		printf("Sent SIGUSR1 to client.\n");
		kill(client_pid, SIGUSR1);
	}
}

void handler() {}
