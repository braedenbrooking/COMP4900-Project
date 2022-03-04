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

#define MAX_STRING_LEN    	256
#define NUMBER_OF_RUNS		100
#define BILLION				1E9

int main(int argc, char **argv)
{
	int coid, fd;
	char rmsg[MAX_STRING_LEN];

	if(argc < 3){
		printf("Error not enough arguments\n");
		exit(-1);
	}

	pid_t server_pid = atoi(argv[1]);
	int server_chid = atoi(argv[2]);

	fd = open("/dev/random", O_RDONLY);

	if (fd == -1) {
		printf("Error opening file\n");
		exit(-1);
	}

//	char* message_array[NUMBER_OF_MESSAGES];
//
//	for(int i = 0; i < NUMBER_OF_MESSAGES; i++){ //Creating messages of size 1, 10, 100, etc.
//		char* random_data = (char*) malloc(pow(10, i));
//		read(fd, &random_data, sizeof(random_data));
//		message_array[i] = random_data;
//	}

	coid = ConnectAttach(0,server_pid, server_chid, _NTO_SIDE_CHANNEL, 0);

	int len = (int) pow(10, 8); // E.g., 1000 bytes.
	char *str = malloc(len + 1);
	memset(str, '*', len);
	str[len] = '\0';

	struct timespec t1,t2;

	if(clock_gettime(CLOCK_REALTIME, &t1) == -1) {
		perror("clock gettime");
		exit(-1);
	}

	for(int i = 0; i < NUMBER_OF_RUNS; i++) {
		MsgSend(coid, str, strlen(str) + 1, &rmsg, sizeof(rmsg));
	}

	if(clock_gettime(CLOCK_REALTIME, &t2) == -1) {
		perror("clock gettime");
		exit(-1);
	}

	double TM = (t2.tv_sec - t1.tv_sec) + ((double) (t2.tv_nsec + t1.tv_nsec)) / BILLION;
	double result = TM/NUMBER_OF_RUNS/2;

	printf("%d B @ %lf s\n", len, result);

	close(fd);

	return 0;
}
