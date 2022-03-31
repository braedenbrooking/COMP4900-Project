// Interrupt handling.
// https://www.qnx.com/developers/docs/7.1/index.html#com.qnx.doc.neutrino.sys_arch/topic/kernel_Interrupt_calls.html
#include <stdlib.h>
#include <stdio.h>
#include <sys/procmgr.h>
#include <sys/syspage.h>
#include <sys/neutrino.h>

#define NUMBER_OF_RUNS 10000 //10, 100, 1000, 10000
#define BILLION		1E9

struct sigevent event;
volatile unsigned counter;


const struct sigevent *handler(void *area, int id) {
    // Wake up the thread every 100th interrupt
    if (++counter == 100) {
        counter = 0;
        return &event;
	} else
        return NULL;
    }

int main() {
	int i;
	int id;
	struct timespec t1, t2;

	// Initialize event structure
	event.sigev_notify = SIGEV_INTR;

	// Enable the INTERRUPT ability
	procmgr_ability(0, PROCMGR_ADN_ROOT|PROCMGR_AOP_ALLOW|PROCMGR_AID_INTERRUPT, PROCMGR_AID_EOL);

	// Attach ISR vector
	id = InterruptAttach(SYSPAGE_ENTRY(qtime)->intr, &handler, NULL, 0, 0);



	if (clock_gettime(CLOCK_REALTIME, &t1) == -1) {
		perror("clock_gettime");
		exit(-1);
	}

	for (i = 0; i < NUMBER_OF_RUNS; ++i) {
		// Wait for ISR to wake us up
		InterruptWait(0, NULL);
		//printf("100 events, %d\n", i);
	}

	if (clock_gettime(CLOCK_REALTIME, &t2) == -1) {
		perror("clock_gettime");
		exit(-1);
	}

	double t = (double)((t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / BILLION);
	double TP = (t / NUMBER_OF_RUNS / 100 /2 / 1000 ) * 1000000;

	printf("%.10lf us\n", TP);

	// Disconnect the ISR handler
	InterruptDetach(id);

	return 0;
}
