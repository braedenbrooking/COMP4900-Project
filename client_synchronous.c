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

#define MAX_STRING_LEN		256
#define ORDER_OF_MAGNITUDE	0 // Tests: 0, 1, 2, 3, 4. Remember to change server OOM.
#define NUMBER_OF_RUNS		10E6
#define BILLION				1E9

int main(int argc, char **argv) {
	pid_t server_pid;
	struct timespec t0, t1;
	char rmsg[MAX_STRING_LEN];
	int coid, len, server_chid;

	if (argc < 3) {
		printf("Error: not enough arguments.\n");
		exit(-1);
	}

	server_pid = atoi(argv[1]);
	server_chid = atoi(argv[2]);
	coid = ConnectAttach(0,server_pid, server_chid, _NTO_SIDE_CHANNEL, 0);
	len = (int)pow(10, ORDER_OF_MAGNITUDE);
	char *str = malloc(len + 1);
	memset(str, '*', len);
	str[len] = '\0';

	if (clock_gettime(CLOCK_REALTIME, &t0) == -1) {
		perror("clock_gettime");
		exit(-1);
	}

	for (int i = 0; i < NUMBER_OF_RUNS; i++) {
		MsgSend(coid, str, strlen(str) + 1, &rmsg, sizeof(rmsg));
	}

	if (clock_gettime(CLOCK_REALTIME, &t1) == -1) {
		perror("clock_gettime");
		exit(-1);
	}

	double t = (double)((t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / BILLION);
	double TM = (t / NUMBER_OF_RUNS / 2) * 1000000;

	printf("%d B @ %lf us\n", len, TM);

	return 0;
}
