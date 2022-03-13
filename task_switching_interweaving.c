// Task switching test.
// Interweaving task.

#include <time.h>
#include <fcntl.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/neutrino.h>
#include <sys/resource.h>

#define BILLION	1E9

int main(int argc, char **argv) {
	while (1) {
		sched_yield();
	}

	return 0;
}
