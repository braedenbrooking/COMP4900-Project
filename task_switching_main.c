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

#define COUNT		100E6 // Tests: 100E6, 250E6, 500E6, 750E6, 1E9.
#define BILLION		1E9

int main(int argc, char **argv) {
	pid_t pid;
	double t, TA, TB, TD;
	struct timespec t0, t1;
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

	t = (double)((t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / BILLION);
	printf("t = %f\n", t);
	TA = t / COUNT / 2; // Duration of one loop cycle plus task switch.

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

	t = (double)((t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / BILLION);
	printf("t = %f\n", t);
	TB = t / COUNT; // Duration of one loop cycle.
	TD = (TB - TA) * 1000000; // Multiply by 1000000 since we want the result in microseconds (us).

	printf("%.10lf us\n", TD);

	return 0;
}
