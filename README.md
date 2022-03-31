# COMP4900-Project
Measuring RTOS performance
## How to run the tests

 - Asynchronous Message Passing
	 - ./asynchronous_message_passing
	- Change the "ORDER_OF_MAGITUDE" to the order of magnitude of bytes you want (e.g. 3 for 10^3 Bytes)
	- Change the "NUMBER_OF_RUNS" to the number of runs of the test you want to do (recommend 10E6)
 - Synchronous Message Passing
	 - ./server_synchronous &; (Then record the coid and pid) (You could also use two terminals rather than using '&')
	 - ./client_synchronous coid pid; (Where coid and pid are the recorded from the server)
	 - Change the "ORDER_OF_MAGITUDE" to the order of magnitude of bytes you want (e.g. 3 for 10^3 Bytes). Make sure to change in both files
	 - Change the "NUMBER_OF_RUNS" to the number of runs of the test you want to do (recommend 10E6). Only in client_synchronous
- Task Synchronization
	-  ./synchronization
	- Change the "COUNT" to the number of runs of the test you want to do (recommend 3000-15000)
	- Note: Both the task-level and the signal handler level run together
- Task Switching
	- ./task_switching_interweaving &; (You could also use two terminals rather than using '&')
	- ./task_switching_main
	- Change the "COUNT" to the number of runs of the test you want to do (recommend 100E6-1E9)
- Interrupt Handling
	- ./interrupt_handling
	- Change the "NUMBER_OF_RUNS" to the number of runs of the test you want to do (recommend <=10000).
