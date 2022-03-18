DEBUG = -g
CC = qcc
LD = qcc

TARGET = -Vgcc_ntox86_64
#TARGET = -Vgcc_ntox86
#TARGET = -Vgcc_ntoarmv7le
#TARGET = -Vgcc_ntoaarch64le

CFLAGS += $(DEBUG) $(TARGET) -Wall -lm
LDFLAGS+= $(DEBUG) $(TARGET) -lm
BINS = asynchronous_message_passing client_synchronous server_synchronous synchronization task_switching_main task_switching_interweaving interrupt_handling
all: $(BINS)

clean:
	rm -f *.o $(BINS);
#	cd solutions; make clean
