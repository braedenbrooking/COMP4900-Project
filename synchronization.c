// Task synchronization and event handling test.

#include <math.h>
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/neutrino.h>
#include <sys/resource.h>

#define COUNT	15E3 // Tests: 3E3, 6E3, 9E3, 12E3, 15E3.
#define BILLION	1E9

int client_pid;

void client(pid_t, sigset_t);
void server(pid_t, sigset_t);
void empty_handler();
void handler(int);

int main(int argc, char **argv) {
	int pid, ppid;
	sigset_t set, client_set, server_set;

	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);
	sigaddset(&set, SIGUSR2);
	sigprocmask(SIG_BLOCK, &set, NULL);

	sigemptyset(&client_set);
	sigemptyset(&server_set);

	signal(SIGUSR1, empty_handler);
	signal(SIGUSR2, empty_handler);

	pid = fork();
	ppid = getppid();

	if (pid == -1) {
		perror("fork");
		exit(-1);
	}

	if (pid == 0) {
		// Child.
		client(ppid, client_set);
	} else {
		// Parent.
		client_pid = pid;
		server(client_pid, server_set);
	}

	return 0;
}

void empty_handler() {}

void handler(int signum) {
	signal(SIGUSR2, SIG_DFL);
	kill(client_pid, SIGUSR2);
}

void client(pid_t server_pid, sigset_t set) {
	struct timespec t0, t1;
	double delta, t, TH, TT;

	// Begin task level test.
	printf("Measuring task level test, please wait...\n\n");

	if (clock_gettime(CLOCK_REALTIME, &t0) == -1) {
		perror("clock_gettime");
		exit(-1);
	}

	for (int i = 0; i < COUNT; i++) {
		kill(server_pid, SIGUSR1);
		sigsuspend(&set);
	}

	if (clock_gettime(CLOCK_REALTIME, &t1) == -1) {
		perror("clock_gettime");
		exit(-1);
	}

	delta = (double)((t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / BILLION);
	TT = (delta / COUNT / 2) * 1000000;

	// Task level results.
	printf("Task Level Results:\n");
	printf("t1    = %d.%ld s\n", (int)t1.tv_sec, t1.tv_nsec);
	printf("t0    = %d.%ld s\n", (int)t0.tv_sec, t0.tv_nsec);
	printf("delta = %lf s\n", delta);
	printf("TT    = %lf us <----- RESULT\n\n", TT);

	// Begin signal handler level test.
	printf("Measuring signal handler level test, please wait...\n\n");

	// Lower the priority of the server for signal handler level test.
	setpriority(PRIO_PROCESS, server_pid, 19);

	if (clock_gettime(CLOCK_REALTIME, &t0) == -1) {
		perror("clock_gettime");
		exit(-1);
	}

	for (int i = 0; i < COUNT; i++) {
		kill(server_pid, SIGUSR2);
		sigsuspend(&set);
	}

	if (clock_gettime(CLOCK_REALTIME, &t1) == -1) {
		perror("clock_gettime");
		exit(-1);
	}

	delta = (double)((t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / BILLION);
	t = (delta / COUNT) * 1000000;

	// Signal handler level results.
	printf("Signal Handler Level Results:\n");
	printf("t1    = %d.%ld s\n", (int)t1.tv_sec, t1.tv_nsec);
	printf("t0    = %d.%ld s\n", (int)t0.tv_sec, t0.tv_nsec);
	printf("delta = %lf s\n", delta);
	printf("t     = %lf us <----- RESULT\n\n", t);

	TH = fabs(t - TT);

	// Difference in results.
	printf("Difference between TL and SHL results:\n");
	printf("TH = %lf us\n", TH);

	kill(server_pid, SIGKILL);
}

void server(pid_t client_pid, sigset_t set) {
	while (1) {
		sigsuspend(&set);
		kill(client_pid, SIGUSR1);
	}
}
