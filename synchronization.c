// Task synchronization and event handling test.

#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/neutrino.h>
#include <sys/resource.h>

#define COUNT	250 // Tests: 50, 100, 150, 200, 250.
#define BILLION	1E9

int client_pid;

void client(pid_t);
void server(pid_t);
void empty_handler();
void handler(int);

int main(int argc, char **argv) {
	int pid, ppid;

	signal(SIGUSR1, empty_handler);

	pid = fork();
	ppid = getppid();

	if (pid == -1) {
		perror("fork");
		exit(-1);
	}

	if (pid == 0) {
		// Child.
		// Lower the priority of the server for signal handler level test.
		// Comment this out if running the task level test.
		// setpriority(PRIO_PROCESS, ppid, 19);
		client(ppid);
	} else {
		// Parent.
		client_pid = pid;
		server(client_pid);
	}

	return 0;
}

void client(pid_t server_pid) {
	printf("Client ready (server_pid = %d).\n", server_pid);

	sigset_t set;
	struct timespec t0, t1;

	sigemptyset(&set);

	// Comment this out if running the task level test.
	// signal(SIGUSR2, empty_handler);

	if (clock_gettime(CLOCK_REALTIME, &t0) == -1) {
		perror("clock_gettime");
		exit(-1);
	}

	for (int i = 0; i < COUNT; i++) {
		sleep(1);

		// Task level test.
		kill(server_pid, SIGUSR1);
		printf("Sent SIGUSR1 to server.\n");

		// Signal handler level test.
		// Comment this out if running the task level test.
		// kill(server_pid, SIGUSR2);
		// printf("Sent SIGUSR2 to server.\n");

		printf("Client suspending...\n");
		sigsuspend(&set);
	}

	if (clock_gettime(CLOCK_REALTIME, &t1) == -1) {
		perror("clock_gettime");
		exit(-1);
	}

	// time_slept is the total number of seconds slept (i.e.,
	// client and server each sleep 1s for every run). We need
	// to subtract this amount from the total time taken to
	// get the actual amount of time teken by the signal passing.
	int time_slept = COUNT * 2;
	printf("t = %f\n", (double)((t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / BILLION));
	double TD = ((double)((t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / BILLION)) - time_slept;
	double TT = TD / COUNT / 2;

	// Task level results.
	printf("%lf us\n\n", TT);

	// Signal handler level results.
	// Comment this out if running the task level test.
	// double t = TD / COUNT;
	// double TH = t - TT;
	// printf("%lf us\n\n", TH);
}

void server(pid_t client_pid) {
	printf("Server ready (client_pid = %d).\n", client_pid);

	sigset_t set;

	sigemptyset(&set);

	// Comment this out if running the task level test.
	// signal(SIGUSR2, handler);

	while (1) {
		printf("Server suspending...\n");
		sigsuspend(&set);

		// Task level test.
		// Comment this out if running the signal handler level test.
		sleep(1);
		kill(client_pid, SIGUSR1);
		printf("Sent SIGUSR1 to client.\n");
	}
}

void empty_handler() {}

void handler(int signum) {
	if (signum == SIGUSR1) return;

	sleep(1);
	kill(client_pid, SIGUSR2);
	printf("Sent SIGUSR2 to client.\n");
}
