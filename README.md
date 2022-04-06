# KU_MLFQ
Multi-Level Feedback Queue (MLFQ)



## Multiple queues
– Each has a different priority level
– At any given time, a job that is ready to run is on a single queue



## Scheduling Policy
– Rule 1: If Priority(A) > Priority(B), A runs (B doesn’t)
– Rule 2: If Priority(A) = Priority(B), A & B run in RR
– Rule 3: When a job enters the system, it is placed at the highest priority
– Rule 4: Once a job uses up its time allotment at a given level (regardless of how many times it gives up the CPU), its priority is reduced
– Rule 5: After some time period S, move all the jobs in the system to the topmost queue



## Multi-Level Queues
Priority
– 1 (high)
– 2
– 3 (low)

Ready queues
– Linked list



## Time Slice
Interval timer (itimer)
– int setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value)
```C
struct itimerval {
  struct timeval it_interval; /* next value */
  struct timeval it_value; /* current value */
};
struct timeval {
  time_t tv_sec; /* seconds */
  suseconds_t tv_usec; /* microseconds */
};
```



## Scheduling Parameters
Time slice
– 1 second
Gaming tolerance
– Time allotment: 2 seconds
S
– 10 seconds
Number of CPU cores
– 1



## Signaling
System calls for signaling
– handler_t *signal(int signum, handler_t *handler)
– int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
– int kill(pid_t pid, int sig);

Signals
– SIGALRM
– SIGSTOP: Stop process
– SIGCONT: Continue if stopped



## Process Creation
System calls for process management
– int fork(void)
– int execl(char *path, char *arg0, char *arg1, …, 0)



## Application
Target application
– The user program is given in a binary format
  - Stops itself
  - Performs an infinite loop
  - Prints a character passed through argv for every 200ms
– E.g., > ku_app A
– ku_app locates in the same directory with the scheduler



## Execution Command
ku_mlfq n ts
– n: number of processes
  • The first process prints ‘A’
  • The second process prints ‘B’
  • …
  • i.e., 1<= n <= 26
– ts: number of time slices to run




(this scheduler is implemented as a user process)
