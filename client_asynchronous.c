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


#define MAX_STRING_LEN		256
#define NUMBER_OF_MESSAGES	6
#define BILLION				1000000000

int main(int argc, char **argv)
{
	/*
	int fd;
	char rmsg[MAX_STRING_LEN];

	if(argc <3){
		printf("Error not enough arguments\n");
		exit(-1);
	}
	pid_t server_pid = atoi(argv[1]);
	int server_chid = atoi(argv[2]);

	fd = open("/dev/random", O_RDONLY);
	if (fd == -1){
		printf("Error opening file\n");
		exit(-1);
	}

	char* message_array[NUMBER_OF_MESSAGES];
	for(int i=0; i<NUMBER_OF_MESSAGES; i++){ //Creating messages of size 1, 10, 100, etc.
		long int size = (int)pow(10,i);
		char random_data[size];
		read(fd, &random_data, sizeof(random_data));
		message_array[i] = random_data;
	}

	read()

    for(int i=0; i<NUMBER_OF_MESSAGES; i++){
    	printf("%d Bytes - %Lg Nano Seconds\n\n",(int)pow(10,i), times[i]);
    }
	*/
	return 0;
}
