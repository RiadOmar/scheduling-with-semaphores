#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

int quantum;
int lastPid = 0;
int currentRunningProcess = -1;
char memory[60][256];
int finishedPrograms = 0;

struct process
{
    char processName[20]; // for debugging purposes, not actually used for anything but printing
    int processId;
    // index 1: state -- Ready -- Blocked -- Terminated -- Running
    // index 2: pc
    // index 3: lowerBound
    // index 4: upperBound
};

struct process processes[3];

char *getProcessName(int pid)
{
    if (pid == 0)
    {
        return "None";
    }
    for (int i = 0; i < 3; i++)
    {
        if (processes[i].processId == pid)
        {
            return processes[i].processName;
        }
    }
}

int inputSem = 1;
int outputSem = 1;
int fileSem = 1;

int readyQueue[3];
int userInputBlockedQueue[3];
int userOutputBlockedQueue[3];
int fileBlockedQueue[3];

void enqueue(int *queue, int pid)
{
    // printf("Queuing Value: %i\n", pid);
    int *pointer = queue;
    for (int i = 0; i < 3; i++)
    {
        if (queue[i] == 0)
        {
            queue[i] = pid;
            break;
        }
    }
}

int dequeue(int *queue)
{
    int *pointer = queue;

    int retVal = queue[0];

    queue[0] = queue[1];
    queue[1] = queue[2];
    queue[2] = 0;
    return retVal;
}

bool queueIsEmpty()
{
    if (readyQueue[0] == 0 && readyQueue[1] == 0 && readyQueue[2] == 0)
    {
        return true;
    }

    return false;
}

void createProcess(char *processName)
{

    int processStartPosition = lastPid * 20;

    // if (strcmp(memory[processStartPosition + 1], "Status: Terminated" || strcmp("")) == 0)
    char tmp[50];
    sprintf(tmp, "PID: %i", (lastPid + 1));
    strcpy(memory[processStartPosition], tmp);
    // strcpy(memory[processStartPosition + 1], "State: Ready");

    strcpy(memory[processStartPosition + 2], "PC: 0");

    sprintf(tmp, "Lower Bound: %i", (processStartPosition));
    strcpy(memory[processStartPosition + 3], tmp);

    sprintf(tmp, "Upper Bound: %i", (processStartPosition + 19));
    strcpy(memory[processStartPosition + 4], tmp);

    strcpy(memory[processStartPosition + 5], "Unused Variable Space");
    strcpy(memory[processStartPosition + 6], "Unused Variable Space");
    strcpy(memory[processStartPosition + 7], "Unused Variable Space");
    strcpy(memory[processStartPosition + 8], "Unused Variable Space");
    strcpy(memory[processStartPosition + 9], "Unused Variable Space");

    for (int i = 0; i < 10; i++)
    {
        strcpy(memory[processStartPosition + 10 + i], "Unused Instruction Space");
    }

    load(processName);

    struct process createdProcess;

    strcpy(createdProcess.processName, processName);
    createdProcess.processId = lastPid + 1;

    processes[lastPid] = createdProcess;

    setState(lastPid + 1, "Ready");

    // 10 spaces in the memory loaded for unparsed instruction loaded from file, fill in any
    // remaining unused spaces with "TERMINATE"
    lastPid++;
    enqueue(&readyQueue, lastPid);
}

void load(char *processName)
{
    int processStartPosition = lastPid * 20;
    int instruction_start = 10;
    if (strcmp(processName, "Program_1.txt") == 0)
    {
        FILE *file = fopen(processName, "r");
        if (file == NULL)
        {
            printf("Error opening file %s\n", processName);
            return;
        }
        char line[256];
        // printf("the start of instructions for P1 is: %i \n", processStartPosition + instruction_start);
        while (fgets(line, sizeof(line), file))
        {
            line[strcspn(line, "\r")] = '\0';
            // line[strcspn(line, "\n")] = '\0';
            strcpy(memory[processStartPosition + instruction_start], line);
            instruction_start++;
        }
    }
    else if (strcmp(processName, "Program_2.txt") == 0)
    {
        FILE *file = fopen(processName, "r");
        if (file == NULL)
        {
            printf("Error opening file %s\n", processName);
            return;
        }
        char line[256];
        // printf("the start of instructions for P2 is: %i \n", processStartPosition + instruction_start);
        while (fgets(line, sizeof(line), file))
        {
            line[strcspn(line, "\r")] = '\0';
            // line[strcspn(line, "\n")] = '\0';
            strcpy(memory[processStartPosition + instruction_start], line);
            instruction_start++;
        }
    }
    else
    {
        FILE *file = fopen(processName, "r");
        if (file == NULL)
        {
            printf("Error opening file %s\n", processName);
            return;
        }
        char line[256];
        // printf("the start of instructions for P3 is: %i \n", processStartPosition + instruction_start);
        while (fgets(line, sizeof(line), file))
        {
            line[strcspn(line, "\r")] = '\0';
            // line[strcspn(line, "\n")] = '\0';
            strcpy(memory[processStartPosition + instruction_start], line);
            instruction_start++;
        }
    }
}

char *getVarName(char *varString)
{
    char *varName = malloc(256);

    for (int i = 0; i < 256 && varString[i] != ':'; i++)
    {
        varName[i] = varString[i];
    }

    return varName;
}

char *getVarValue(char *varString)
{
    char *varValue = malloc(256);

    int i = 0;

    for (; i < 256 && varString[i] != ':'; i++)
    {
    }
    i += 2;

    for (int x = 0; i < 256 && varString[i] != ':'; i++, x++)
    {
        if (varString[i] == ' ')
        {
            break;
        }

        varValue[x] = varString[i];
    }

    return varValue;
}

char *getVarValueByName(int pid, char *varName)
{
    char *result = malloc(256);

    int startingPosition = (pid - 1) * 20 + 5;

    for (int i = 0; i < 5; i++)
    {
        char *varString;
        strcpy(varString, memory[startingPosition + i]);
        if (strcmp(getVarName(varString), varName) == 0)
        {
            strcpy(result, getVarValue(varString));
            return result;
        }
    }

    return "VARIABLE NOT FOUND";
}

void print(int pid, char *varName)
{
    int varPoisitionInMemory = (pid - 1) * 20 + 5;
    char varInMemory[256];
    char varValue[256];

    for (int i = 0; i < 5; i++)
    {
        strcpy(varInMemory, memory[varPoisitionInMemory + i]);

        if (strcmp(getVarName(varInMemory), varName) == 0)
        {
            break;
        }
    }

    printf("    Print Output: %s\n", getVarValue(varInMemory));
}

void assign(int pid, char *varName, char *varValue)
{
    int varPoisitionInMemory = (pid - 1) * 20 + 5;
    int lastUsedVariablePosition = varPoisitionInMemory;

    for (int i = 0; i < 5; i++)
    {
        if (strcmp(memory[varPoisitionInMemory + i], "Unused Variable Space") == 0)
        {
            break;
        }
        else
        {
            lastUsedVariablePosition++;
        }
    }
    // char *variable = malloc(256);
    char variable[256];
    strcpy(variable, varName);
    strcat(variable, ": ");
    strcat(variable, varValue);

    strcpy(memory[lastUsedVariablePosition], variable);
    printf("    Memory Position %i was assigned %s\n", lastUsedVariablePosition, variable);
}

int semWait(char *semName)
{
    if (strcmp(semName, "userInput") == 0)
    {
        if (inputSem == 1)
        {
            inputSem = 0;
            return 1;
        }
        else
        {
            enqueue(&userInputBlockedQueue, currentRunningProcess);
            currentRunningProcess = -1;
            return 0;
        }
    }
    else
    {
        if (strcmp(semName, "userOutput") == 0)
        {
            if (outputSem == 1)
            {
                outputSem = 0;
                return 1;
            }
            else
            {
                enqueue(&userOutputBlockedQueue, currentRunningProcess);
                currentRunningProcess = -1;
                return 0;
            }
        }
        else
        {
            if (strcmp(semName, "file") == 0)
            {
                if (fileSem == 1)
                {
                    fileSem = 0;
                    return 1;
                }
                else
                {
                    enqueue(&fileBlockedQueue, currentRunningProcess);
                    currentRunningProcess = -1;
                    return 0;
                }
            }
            else
            {
                printf("Unidentified Semaphore\n");
                return -1;
            }
        }
    }
}

void semSignal(char *semName)
{
    // printf("Input to semsignal is: %s\n", semName);
    if (strcmp(semName, "userInput") == 0)
    {
        if (inputSem == 1)
        {
            printf("ERROR: Attempted to set an already unused semaphore to 1\n");
            return;
        }
        else
        {
            inputSem = 1;
            int process_id = dequeue(&userInputBlockedQueue);
            if (process_id != 0)
            {
                setState(process_id, "Ready");
                enqueue(&readyQueue, process_id);
            }
        }
    }
    else
    {
        if (strcmp(semName, "userOutput") == 0)
        {
            if (outputSem == 1)
            {
                printf("ERROR: Attempted to set an already unused semaphore to 1\n");
                return;
            }
            else
            {
                outputSem = 1;
                int process_id = dequeue(&userOutputBlockedQueue);
                if (process_id != 0)
                {
                    setState(process_id, "Ready");
                    enqueue(&readyQueue, process_id);
                }
            }
        }
        else
        {
            if (strcmp(semName, "file") == 0)
            {
                if (fileSem == 1)
                {
                    printf("ERROR: Attempted to set an already unused semaphore to 1\n");
                    return;
                }
                else
                {
                    fileSem = 1;
                    int process_id = dequeue(&fileBlockedQueue);
                    if (process_id != 0)
                    {
                        setState(process_id, "Ready");
                        enqueue(&readyQueue, process_id);
                    }
                }
            }
            else
            {
                printf("ERROR: Attempted to set a non-existent semaphore\n");
            }
        }
    }
}

void writeFile(char *filename, char *writeValue)
{
    FILE *file;

    file = fopen(filename, "w");

    fprintf(file, writeValue);

    fclose(file);
}

void printFromTo(int a, int b)
{
    // printf("Input to print from to is: %i,%i \n", a, b);
    printf("    printFromTo Output: ");

    for (int i = a; i <= b; i++)
    {
        printf("%i ", i);
    }
    printf("\n");
}

char *readFile(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening file %s\n", filename);
        return;
    }
    char *line = malloc(256);

    fgets(line, 256, file);
    fclose(file);
    return line;
}

int execute(int pid)
{
    // during execution, getting the instruction we need, we do PC + lowerBound + 10
    // changing the process state to terminated is done when the "TERMINATE" string is hit or if the PC > upperBound

    int PC = atoi(getVarValue(memory[(pid - 1) * 20 + 2]));

    char instructionString[256];
    strcpy(instructionString, memory[(pid - 1) * 20 + PC + 10]);

    printf("    Current Running Instruction: %s\n", instructionString);

    char *token = strtok(instructionString, " ");
    char *splitInstruction[4] = {NULL};
    int token_count = 0;
    while (token != NULL && token_count < 4)
    {
        splitInstruction[token_count] = strdup(token);
        token_count++;
        token = strtok(NULL, " ");
    }

    if (strcmp(splitInstruction[0], "semWait") == 0)
    {
        // printf("Input to semWait is: %s \n", splitInstruction[1]);
        int avaliable = semWait(splitInstruction[1]);
        if (!avaliable)
        {
            setState(pid, "Blocked");
            printQueues();

            return;
        }
    }
    else if (strcmp(splitInstruction[0], "assign") == 0)
    {
        if (strcmp(splitInstruction[2], "readFile") != 0)
        {
            char input_string[256];
            printf("Input of variable (%s) of the program (%s): ", splitInstruction[1], getProcessName(pid));
            scanf("%s", input_string);
            assign(pid, splitInstruction[1], input_string);
        }
        else
        {
            char *var1 = getVarValueByName(pid, splitInstruction[3]);
            char *readed_line = readFile(var1);
            assign(pid, splitInstruction[1], readed_line);
        }
    }
    else if (strcmp(splitInstruction[0], "semSignal") == 0)
    {
        semSignal(splitInstruction[1]);
    }
    else if (strcmp(splitInstruction[0], "printFromTo") == 0)
    {
        char *var1 = getVarValueByName(pid, splitInstruction[1]);
        char *var2 = getVarValueByName(pid, splitInstruction[2]);
        printFromTo(atoi(var1), atoi(var2));
    }
    else if (strcmp(splitInstruction[0], "writeFile") == 0)
    {
        char *var1 = getVarValueByName(pid, splitInstruction[1]);
        char *var2 = getVarValueByName(pid, splitInstruction[2]);
        writeFile(var1, var2);
    }
    else if (strcmp(splitInstruction[0], "print") == 0)
    {
        print(pid, splitInstruction[1]);
    }

    // PC INREMENTED
    PC++;
    if (PC > 9 | strcmp(memory[(pid - 1) * 20 + 10 + PC], "Unused Instruction Space") == 0)
    {
        setState(pid, "Terminated");
        currentRunningProcess = -1;
        finishedPrograms++;
    }

    char newPCString[256];
    sprintf(newPCString, "PC: %i", PC);

    strcpy(memory[(pid - 1) * 20 + 2], newPCString);
}

void setState(int pid, char *state)
{
    printf("    The State of the Program (%s) has been set to: %s\n", getProcessName(pid), state);
    char concat[256];
    strcpy(concat, "State: ");
    strcat(concat, state);
    strcpy(memory[(pid - 1) * 20 + 1], concat);
}

void printQueues()
{
    printf("    Queue Prints:\n");
    printf("        Ready Queue: [%s, %s, %s]\n", getProcessName(readyQueue[0]), getProcessName(readyQueue[1]), getProcessName(readyQueue[2]));
    printf("        User Input Blocked Queue: [%s, %s, %s]\n", getProcessName(userInputBlockedQueue[0]), getProcessName(userInputBlockedQueue[1]), getProcessName(userInputBlockedQueue[2]));
    printf("        User Output Blocked Queue: [%s, %s, %s]\n", getProcessName(userOutputBlockedQueue[0]), getProcessName(userOutputBlockedQueue[1]), getProcessName(userOutputBlockedQueue[2]));
    printf("        File Blocked Queue: [%s, %s, %s]\n", getProcessName(fileBlockedQueue[0]), getProcessName(fileBlockedQueue[1]), getProcessName(fileBlockedQueue[2]));
}

void printMemory()
{
    printf("<<<<<<<<<<<<<<<<<<\n");
    for (int i = 0; i < 12; i++)
    {
        printf("Memory Elements %i-%i:\n", 5 * i, 5 * i + 4);
        printf("    [");
        for (int j = 5 * i; j < 5 * i + 5; j++)
        {
            if (j != 5 * i)
            {
                printf(", %s", memory[j]);
            }
            else
            {
                printf(" %s", memory[j]);
            }
        }
        printf("]\n");
    }
    printf(">>>>>>>>>>>>>>>>>>\n");
}

int main()
{

    // enqueue(&readyQueue, dequeue(&userInputBlockedQueue))
    // Format of enqueuing and dequeing wara ba3do

    int arrivalTimes[3];

    printf("Please Enter The Quantum: ");
    scanf("%i", &quantum);
    printf("Please Process 1 Arrival Time: ");
    scanf("%i", &arrivalTimes[0]);
    printf("Please Process 2 Arrival Time: ");
    scanf("%i", &arrivalTimes[1]);
    printf("Please Process 3 Arrival Time: ");
    scanf("%i", &arrivalTimes[2]);

    char program1Name[20] = "Program_1.txt";
    char program2Name[20] = "Program_2.txt";
    char program3Name[20] = "Program_3.txt";

    // TODO: Assign names

    int cycle = 0;
    int runtimeLeft = quantum;
    while (finishedPrograms < 3)
    {
        printf("------------------------------------------\nCurrent Cycle: %i \n", cycle);
        // printf("The lastpid is: %i \n", lastPid);
        // printf("The Sempahore userInput: %i \n", inputSem);
        // printf("The Sempahore userOutput: %i \n", outputSem);
        // printf("The Sempahore file: %i \n", fileSem);

        if (arrivalTimes[0] == cycle)
        {
            printf("    The Program (%s) has Arrived\n", program1Name);
            createProcess(program1Name);
            printQueues();
        }
        if (arrivalTimes[1] == cycle)
        {
            printf("    The Program (%s) has Arrived\n", program1Name);
            createProcess(program2Name);
            printQueues();
        }
        if (arrivalTimes[2] == cycle)
        {
            printf("    The Program (%s) has Arrived\n", program1Name);
            createProcess(program3Name);
            printQueues();
        }

        // check if we'll be creating the processes 1, 2 or 3 or none during the cycle
        // printf("Queue Before: %i,%i,%i \n", readyQueue[0], readyQueue[1], readyQueue[2]);
        if (currentRunningProcess == -1 && !queueIsEmpty())
        {
            // attempt to get the first element from the queue
            currentRunningProcess = dequeue(readyQueue);
            setState(currentRunningProcess, "Running");
            runtimeLeft = quantum;
            printQueues();
        }
        // printf("Current Proccess: %i\n", currentRunningProcess);
        if (currentRunningProcess != -1)
        {
            if (runtimeLeft == 0)
            {
                printf("    Runtime for this Program Expired \n");
                setState(currentRunningProcess, "Ready");
                enqueue(readyQueue, currentRunningProcess);
                currentRunningProcess = dequeue(readyQueue);
                setState(currentRunningProcess, "Running");
                runtimeLeft = quantum;
                printQueues();
            }

            // Do the execution
            // State of program changes to terminated if its done
            printf("    Currently Running Process: %s\n", getProcessName(currentRunningProcess));

            execute(currentRunningProcess);

            // At the end of the method, we check if we should terminate or not

            if (strcmp(memory[currentRunningProcess * 20 + 1], "State: Terminated") == 0)
            {
                currentRunningProcess = -1;
            }

            runtimeLeft--;
        }

        printMemory();
        // printf("%s", memory[3]);
        char tmp = "";
        if (cycle % 25 == 0 && cycle != 0)
        {
            printf("Continue? [Y/n]: ");
            scanf("%c", &tmp);
            if (tmp == 'n' || tmp == 'N')
            {
                return 0;
            }
            getchar();
        }
        // Do our prints
        // Print current running program name as well as its ID, the non-0 process IDs from the queue, the
        // unstruction that executed this cycle, The wntire memory
        cycle++;
        // printf("Queue After: %i,%i,%i \n", readyQueue[0], readyQueue[1], readyQueue[2]);
    }

    printf("EXECUTION DONE\n");

    return 0;
}