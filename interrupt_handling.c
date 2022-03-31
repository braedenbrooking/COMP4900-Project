// Interrupt handling.
// https://www.qnx.com/developers/docs/7.1/index.html#com.qnx.doc.neutrino.sys_arch/topic/kernel_Interrupt_calls.html
#include <stdio.h>
#include <stdlib.h>
#include <sys/procmgr.h>
#include <sys/syspage.h>
#include <sys/neutrino.h>

#define TICK_RATE		1000000 // Tests: 1000000 (t_1, 1 ms), 50000000 (t_2, 50 ms).
#define NUMBER_OF_RUNS	101 // Tests: 101, 1001, 10001
#define BILLION			1E9

struct timespec t1;
struct sigevent event;
volatile unsigned counter;

const struct sigevent *handler(void *area, int id) {
	return &event;
}

int main() {
	int id;
	double delta, total, times[NUMBER_OF_RUNS];
	struct timespec t0, t1;
	struct _clockperiod clk_period;

	clk_period.nsec = TICK_RATE;
	clk_period.fract = 0;
	ClockPeriod(CLOCK_REALTIME, &clk_period, NULL, 0);

	// Initialize event structure
	event.sigev_notify = SIGEV_INTR;

	// Enable the INTERRUPT ability
	procmgr_ability(0, PROCMGR_ADN_ROOT | PROCMGR_AOP_ALLOW | PROCMGR_AID_INTERRUPT, PROCMGR_AID_EOL);

	// Attach ISR vector
	id = InterruptAttach(SYSPAGE_ENTRY(qtime)->intr, &handler, NULL, 0, 0);

	for (int i = 0; i < NUMBER_OF_RUNS; i++) {
		if (clock_gettime(CLOCK_REALTIME, &t0) == -1) {
			perror("clock_gettime");
			exit(-1);
		}

		// Wait for ISR to wake us up
		InterruptWait(0, NULL);

		if (clock_gettime(CLOCK_REALTIME, &t1) == -1) {
			perror("clock_gettime");
			exit(-1);
		}

		delta = (double)((t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / BILLION);
		times[i] = delta;
	}

	// We start at index 1 to remove the outlier.
	for (int i = 1; i < NUMBER_OF_RUNS; i++) {
		total += times[i];
	}

	total = (total / (NUMBER_OF_RUNS - 1)) * 1000000;

	printf("RESULT = %lf us\n", total);

	// Disconnect the ISR handler
	InterruptDetach(id);

	return 0;
}
