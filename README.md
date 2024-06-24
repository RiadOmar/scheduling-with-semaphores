# Simulating a Schedule with Bonary Semaphores in C

## Overview
This is a project where a simple scheduler that uses the RR scheduling algorithm was reacreated with support for binary semaphores so that it is possible to lock critical sections when required. The number of semaphores used was fixed to 3 for the basic functions of receiving an input, printing an output and accessing a file. This was all done in C with the purpose of showcasing one of the tasks that an operating system handles.

## Team Members
- Ahmed Khedr 
- Marawan Salah
- Mohammad Rageh 
- Omar Farouk 
- Omar Riad

## Key Objectives
- Mimic the functionality of an operating system’s scheduler using the C programming language.
- Make the previously mentioned scheduler abide by the rules of the Round Robin scheduling algorithm.
- Make the previously mentioned scheduler abide by Binary Semaphores, where a maximum 1 process is allowed to use the semaphore’s resource at any given time


## Execution Stages
- Taking the scheduling algorithm’s quantum value as well as the arrival time of each of the 3 provided processes as an input.
- Reading the provided .txt program files when their arrival time is reached, creating its PCB and storing the relevant information in it’s allocated memory segment.
- Run the programs that have arrived concurrently according to the rules of the scheduling algorithm and semaphores.

## Methodology

### Semaphores
3 semaphores for user input, user output and file read/write were created to guarantee that each of the resources can only be used by one process at any given time

### Scheduler Queues
The scheduler was implemented to have a ready queue, which hold the processes that are ready to be executed, as soon as the CPU finished the current process or the scheduler preempts it, and 3 blocked queues, one for each of the aforementioned semaphores. All queues store the process ID; however, the print statements get the program file name, for ease of tracing.

### Process Creation
Creating a process is composed of 3 steps, followed
directly by adding that process to the ready queue.

- Memory Allocation:
Each process is allocated 1/3 of our 60 word memory, meaning 20 words, 5 are used for the PCB, 5 are used as variable space and 10 are used as instruction space.

- PCB Creation:
The PCB for each process is assigned a process ID, depending on what process ID has been unclaimed yet, its memory bounds, depending on the previously mentioned memory allocation, a state, which is set to ”Ready” initially, and a local PC value, which is set to 0 by default.
  
- Instruction Loading:
For the process being created, it opens the corresponding program text files and reads it line by line. Each instruc- tion is stored in the memory location assigned to this process based on its predefined memory bounds.

### Execution Block
The following methods have been implemented to simulate the functionality of a system that can take a console input, outputs to the console by printing and can read and write files.

- The print method takes two inputs: the process ID as an integer and the variable name we want to print as a string. It retrieves the starting position of the variables in the PCB (Process Control Block) memory for this process and enters a loop to iterate over the 5 cells assigned to variables. It checks if the value to print corresponds to the input variable name. If a match is found, it breaks out of the loop and prints the value of the variable.

- The method printFromTo() accepts two integer inputs. It iterates from the starting value of the first input to the ending value of the second input, inclusively, and prints each number in between.

- The method assign requires three arguments: the process ID, the variable name, and the variable value. It checks the data range inside the memory to see if it’s used. If the memory cell is not used, it inserts the variable and its value into it. Otherwise, it moves to the next available cell and repeats the process until an unused cell is found.

- The method readFile() takes a single input, which is the name of the file we wish to read. If the file is not found, an error message is displayed. Otherwise, space is allocated for the line we wish to read, the line is read from the file, the file is closed, and the line is returned.

- The method writeFile() takes two inputs: filename, which is a string representing the name of the file, and writeValue, which is a string representing the value to be written to the file. The method opens the specified file, writes the provided value to it, and then closes the file.

- The enqueue method takes two inputs: a pointer to the queue where we want to enqueue an element, and the process ID that will be enqueued. The method iterates through the given queue, and the first cell that contains a 0 is assigned the process ID provided as input.

- The dequeue method takes a single input, which is a pointer to the queue we want to dequeue from. It retrieves the first value inside the queue, which is at cell 0, then shifts all elements by one cell to the left. Finally, it returns the value that was dequeued.

- The method semWait takes one argument, which is the resource. Based on this input, it performs the following actions for this semaphore: If the resource is available, it uses it and returns 1, indicating that the process is now using the resource. However, if the resource is not available, it enqueues the process in the blocked queue for this resource and returns 0.

- The method semSignal takes the resource name as its argument. If the resource is currently being used, it makes it available for future use based on the resource name.

- The setState method takes two arguments: the first is the ID of the process whose state we want to update, and the second is a string representing the new state of the process, which can be ”Terminated,” ”Ready,” ”Running,” or ”Blocked.” The method updates the state of the given process in memory accordin

### Main Loop
The main function’s loop is where the scheduling algorithm’s logic and functionality is implemented, except for the preemption by a semaphore being unavailable and unblocking a previously blocked process being unblocked, which are handled in the execute block.

During the start of each loop, the scheduler checks if any process has arrived, followed by checking if there is no currently running process; if that is the case and the ready queue is not empty, the first process in the queue starts executing.

If there had been a process that has not finished execution during the previous cycle, its remaining number of cycles is checked, if it has run out, preempt that process, place it in the ready queue and start executing the first process in the
ready queue.

After the correct process is selected by the scheduler, the loop enters the execute block, then decrements the remaining number of cycles of the set quantum.

### Program Iterations
- strcmp() not returning the correct output During the starting iterations of the code, a problem was encountered as the instruction executed was never as expected. This issue was resolved during debugging, when the strcmp() was discovered to be returning a numerical value representing the difference in strings lexicographic-ally, rather than a boolean for if it is equal or not, by checking if the ouput is 0, confirming if the strings are equal.

- End-line character not as expected This issue occurred due to the end-line character in the program .txt file being \r rather than \n. After some research, it was found out that different operating systems use different end-line characters. (Raza, 2018)

### Results
The primary objective of this milestone was to create a simulation for a simple operating system scheduler. The end result was a success with a scheduler that runs on the Round Robin scheduling algorithm with support for 3 different semaphores. The scheduler is versatile in allowing the provided programs to arrive in any order; however, it doesn’t account for reading from a non-existent file, as that is a logic error the programmer should handle.

### References
Abraham Silberschatz and Peter Baer Galvin, Operating System
Concepts, 2013.

William Stallings, Operating Systems: Internals and Design Prin-
ciples, Prenitce Hall Press, 2011.
