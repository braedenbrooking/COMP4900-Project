// Interrupt handling.
// https://www.qnx.com/developers/docs/7.1/index.html#com.qnx.doc.neutrino.sys_arch/topic/kernel_Interrupt_calls.html

#include <stdio.h>
#include <sys/procmgr.h>
#include <sys/syspage.h>
#include <sys/neutrino.h>

#define COUNT 10

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

	// Initialize event structure
	event.sigev_notify = SIGEV_INTR;

	// Enable the INTERRUPT ability
	procmgr_ability(0, PROCMGR_ADN_ROOT|PROCMGR_AOP_ALLOW|PROCMGR_AID_INTERRUPT, PROCMGR_AID_EOL);

	// Attach ISR vector
	id = InterruptAttach(SYSPAGE_ENTRY(qtime)->intr, &handler, NULL, 0, 0);

	for (i = 0; i < COUNT; ++i) {
		// Wait for ISR to wake us up
		InterruptWait(0, NULL);
		printf("100 events\n");
	}

	// Disconnect the ISR handler
	InterruptDetach(id);

	return 0;
}
