#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/neutrino.h>
#include <process.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>

#define MAX_STRING_LEN		256
#define ORDER_OF_MAGNITUDE	0 // Tests: 0, 1, 2, 3, 4. Remember to change client OOM.

int main(void) {
	pid_t pid;
	int chid, len, rcvid;
	char rmsg[MAX_STRING_LEN];

	chid = ChannelCreate(0);

	if (chid == -1) {
		perror("ChannelCreate");
		exit(-1);
	}

	pid = getpid();

	printf("chid = %d; pid = %d\n", chid, pid);

	len = (int) pow(10, ORDER_OF_MAGNITUDE);
	char *str = malloc(len + 1);
	memset(str, '*', len);
	str[len] = '\0';

	while (1) {
		rcvid = MsgReceive(chid, &rmsg, sizeof(rmsg), NULL);
		MsgReply(rcvid, EOK, str, strlen(str));
	}

	return 0;
}
