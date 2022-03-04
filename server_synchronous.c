#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/neutrino.h>
#include <process.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>

#define MAX_STRING_LEN    256

int main(void)
{
	//Declare variables
	int chid, rcvid;
	pid_t pid;
	char rmsg[MAX_STRING_LEN];

	//create a channel
	chid = ChannelCreate(0);

	if(chid == -1){
		perror("ChannelCreate()");
		exit(-1);
	}

	pid = getpid();

	printf("chid = %d; pid = %d\n", chid, pid);

	int len = (int) pow(10, 3); // E.g., 1000 bytes.
	char *str = malloc(len + 1);
	memset(str, '*', len);
	str[len] = '\0';

	while(1){
		rcvid = MsgReceive(chid, &rmsg, sizeof(rmsg), NULL);
		printf("Message received: %s\n", rmsg);
		//char* replymsg = "1";
		MsgReply(rcvid, EOK, str, strlen(str));
	}

	return 0;
}
