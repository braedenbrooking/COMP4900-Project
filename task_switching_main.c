// Task switching test.
// Main task.

#include <time.h>
#include <fcntl.h>
#include <sched.h>
#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/resource.h>

#define COUNT	1E6
#define BILLION	1E9

int main(int argc, char **argv) {
	pid_t pid;
	struct timespec t0, t1;
	double t, TA, TB, TD;
	struct inheritance inherit;
	char *argsInterweaving[] = { "task_switching_interweaving", NULL };

	inherit.flags = 0;

	if (clock_gettime(CLOCK_REALTIME, &t0) == -1) {
		perror("clock_gettime");
		exit(-1);
	}

	for (int i = 0; i < COUNT; i++) {
		// Do nothing.
	}

	if (clock_gettime(CLOCK_REALTIME, &t1) == -1) {
		perror("clock_gettime");
		exit(-1);
	}

	t = (t1.tv_sec - t0.tv_sec) + ((double) (t1.tv_nsec + t0.tv_nsec)) / BILLION;
	TA = t / COUNT / 2;

	// Task creation.
	if ((pid = spawn("task_switching_interweaving", 0, NULL, &inherit, argsInterweaving, environ)) == -1)
		perror("spawn");
	else
		printf("Spawned child with PID %d.\n", pid);

	if (clock_gettime(CLOCK_REALTIME, &t0) == -1) {
		perror("clock_gettime");
		exit(-1);
	}

	for (int i = 0; i < COUNT; i++) {
		sched_yield();
	}

	if (clock_gettime(CLOCK_REALTIME, &t1) == -1) {
		perror("clock_gettime");
		exit(-1);
	}

	t = (t1.tv_sec - t0.tv_sec) + ((double) (t1.tv_nsec + t0.tv_nsec)) / BILLION;
	TB = t / COUNT;
	TD = TB - TA;

	printf("%lf s\n", TD);

	return 0;
}
