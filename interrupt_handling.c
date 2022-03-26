#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/neutrino.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <sys/syspage.h>
#include <sys/procmgr.h>
#include <unistd.h>
//#include <sys/irqinfo.h>

#define MAX_STRING_LEN		256
#define ORDER_OF_MAGNITUDE	3
#define NUMBER_OF_RUNS		1000000
#define BILLION				1000000000

struct sigevent event;
volatile unsigned long int counter = 0;

void server(struct timespec*);
void client(struct timespec*, pid_t);

int main(int argc, char **argv)
{
	struct timespec t1,t2;

	pid_t child_pid, wpid;
	int status = 0;

	child_pid = fork();

	if(child_pid == 0){
		server(&t2);
	}else{
		client(&t1, child_pid);
	}
	printf("And now we wait...\n");
	//while ((wpid = wait(&status)) >  0);
	waitpid(child_pid, &status, 0);

	double TP = (t2.tv_sec - t1.tv_sec) + ((double) (t2.tv_nsec + t1.tv_nsec)) / BILLION;
	double result = TP/NUMBER_OF_RUNS/2;
	printf(" %lf s\n", result);

	return 0;
}

const struct sigevent* handler(void *area, int n) {
	if(++counter == NUMBER_OF_RUNS){
		counter = 0;
		return (&event);
	}else{
		printf("Interrupted\n");
		return(NULL);
	}
}

void server(struct timespec* t2){

	SIGEV_INTR_INIT(&event);
	//event.sigev_notify = SIGEV_INTR;
	ThreadCtl(_NTO_TCTL_IO,0);

	procmgr_ability(0,PROCMGR_ADN_ROOT|PROCMGR_AOP_ALLOW|PROCMGR_AID_INTERRUPT,PROCMGR_AID_EOL);

	//int id = InterruptAttach( SYSPAGE_ENTRY(qtime)->intr, &handler, NULL, 0, 0 );
	int id = InterruptAttachEvent(1, &event, _NTO_INTR_FLAGS_END|_NTO_INTR_FLAGS_PROCESS);
	//int id = qnx_hint_attach(0, &handler, FP_SEG(&counter));
	printf("Server Ready\n");
	//for(int i =0; i< NUMBER_OF_RUNS; i++){
		printf("waiting for interrupt\n");
		InterruptWait( 0, NULL );
		printf("Successfully Interrupt\n");
	//}

	if(clock_gettime(CLOCK_REALTIME, t2) == -1) {
		perror("clock gettime");
		exit(-1);
	}
	InterruptDetach(id);
	printf("Detached\n");
	exit(0);
}

void client(struct timespec* t1, pid_t pid){
	sleep(1); // Shitty way of making sure the server is ready might need changing later but for now this is fine lmao
	printf("Client Ready\n");
	if(clock_gettime(CLOCK_REALTIME, t1) == -1) {
		perror("clock gettime");
		exit(-1);
	}

	for(int i = 0; i < NUMBER_OF_RUNS; i++)
		kill(pid, SIGINT);

	printf("client done\n");

	return;
}
