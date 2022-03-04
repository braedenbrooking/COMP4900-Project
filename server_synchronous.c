#include <stdio.h>
#include <stdlib.h>
#include <sys/neutrino.h>
#include <process.h>
#include <errno.h>
#include <unistd.h>

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

	while(1){
		rcvid = MsgReceive(chid, &rmsg, sizeof(rmsg), NULL);
		printf("Message received: %s\n", rmsg);
		char* replymsg = "1";
		MsgReply(rcvid, EOK, &replymsg, sizeof(replymsg));
	}

	return 0;
}
