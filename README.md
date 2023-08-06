# Parallel-Reversi-AlphaBeta

Parallelized version of a Reversi board game using the alpha-beta algorithm to find the most beneficial move. The focus is on resource-intensive computations, and the project leverages parallel processing using semaphores for efficiency.

## Project Components

### multiprocessor.h
Contains the definitions of the library interface. It summarizes the functionalities provided by multiprocessor.c, which includes a simple interface to semaphores and shared memory.

### multiprocessor.c
Provides a simple interface to semaphores and shared memory. It utilizes features from the Linux kernel, such as primitives, to facilitate parallel processing.

### Mailbox.h
Defines the interfaces and features provided by mailbox.c. The mailbox is a bounded buffer that allows child processes to return their game search results to the parent process. It contains triple variables in a struct, including "result" (for completed alpha-beta function), "move number" (identifying the move), and "position explored" (counting explored positions).

### Mailbox.c
Implements a bounded buffer using semaphores. It contains two functions: "mailbox_send" and "mailbox_receive." The implementation includes three semaphores - "space available," "mutex," and "item available" - to ensure synchronized communication between parent and child processes.

### Para64bit.c
This file is responsible for calling the library routine to create a mailbox and utilize multiprocessor semaphores. It creates child processes that explore the game tree in parallel, and then the parent process waits for results from the mailbox receive function. The best move is determined based on the received results.

### Reversi Game
The core of the project involves implementing the Reversi game and using the alpha-beta search strategy to explore moves. It evaluates all legal moves that can be played by the player and their potential counters.
