# Producer-consumer-problem
The multiple producer-multiple consumer problem using FIFO policy.

## To make file and run

`gcc -pthread -o mpmc mpmc.c`

`./mpmc <producers> <consumers> <tasks> <buffersize>`
  
## Demonstration

Suppose you have...

- producers have 4 threads
- consumers have 2 threads
- number of tasks to produce is 10 tasks
- buffer size is 3

`./mpmc 4 2 10 3`
