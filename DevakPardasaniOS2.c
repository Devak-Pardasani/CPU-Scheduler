#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define MAX_TASKS 200
#define SIMULATION_TIME 1000

//states
#define NOTPRESENT -1
#define SLEEPING 0
#define READY 1
#define RUNNING  2
#define IO_WAIT  3
#define FINISHED 4

//tasktypes
#define CPUINTENSIVE 1
#define IOINTENSIVE 2
#define DAEMON 3

//MLFQ parameters
#define HIGHEST_PRIORITY 0
#define MEDIUM_PRIORITY 1
#define LOWEST_PRIORITY 2
#define QUEUE_SIZE 200

//arr declerations - PID = index
int taskarrivetime[MAX_TASKS];
int tasktype[MAX_TASKS];
int currentstate[MAX_TASKS];
int taskremainingwork[MAX_TASKS];
int taskremainingsleep[MAX_TASKS];
int taskfinishtime[MAX_TASKS];

//declerations for FIFO
int FIFOqueue[MAX_TASKS];
int queueFront = 0;
int queueBack = 0; 

//declerations for MLFQ
int timeSliceForPriority[3] = {3, 5, 7};
int taskpriority[MAX_TASKS];

//declerations for CPU
int CPU_PID[4];
int CPU_remainingSlice[4];



//function declerations
int generateRequests(int, int);
int taskSet(int);
void shuffleArray(int[], int);
int FIFOenqueue(int);
void initializeTasks(void);
void updateDaemons(int);
void updateIOWaitTasks(int);
void FIFOsimulate(void);
int FIFOenqueue(int);
int FIFOdequeue(void);
void initializeStartTasks(void);
void MLFQsimulate(void);






int main(){
    srand(time(NULL));
    initializeTasks();
    initializeStartTasks();
    //FIFOsimulate();

    MLFQsimulate();
    //AGINGsimulate();
    int numTasks = 0;
    for(int i = 0; i < MAX_TASKS; i++){
        if(currentstate[i] == FINISHED){
            int responseTime = taskfinishtime[i] - taskarrivetime[i];
            printf("Task %d response time: %d\n", i, responseTime);
            numTasks++;
        }
    }
    printf("%d\n", numTasks);
    return 0;
}

void initializeStartTasks(void) {
    int pids[MAX_TASKS];
    for (int i = 0; i < MAX_TASKS; i++) {
        pids[i] = i;
    }
    
    shuffleArray(pids, MAX_TASKS);
    
    
    int count = 0;
    for (int i = 0; i < MAX_TASKS && count < 100; i++) {
        int PID = pids[i];
        taskarrivetime[PID] = 0;
        tasktype[PID] = taskSet(PID);
        if (tasktype[PID] == DAEMON) { //start sleeping
            currentstate[PID] = SLEEPING;
        } else {
            currentstate[PID] = READY;
            FIFOenqueue(PID); // enqueue only if task is ready
        }
        count++;
    }
}

void initializeTasks(void) {
    for (int i = 0; i < MAX_TASKS; i++) {
        taskarrivetime[i] = NOTPRESENT;
        currentstate[i] = NOTPRESENT;
        taskfinishtime[i] = SIMULATION_TIME;
    }
    for (int i = 0; i < 4; i++) {
        CPU_PID[i] = NOTPRESENT;
        CPU_remainingSlice[i] = 0;
    }
    queueFront = 0;
    queueBack = 0;
}

int generateRequests(int time, int prob) {
    if (rand() % 100 > prob) { //gives us a prob% chance of 
        return -1; 
    }

    int PID;
    for (int i = 0; i < 1000; i++) {
        PID = rand() % MAX_TASKS;  
        if (taskarrivetime[PID] == NOTPRESENT) {
            taskarrivetime[PID] = time;
            tasktype[PID] = taskSet(PID);
            if (tasktype[PID] == DAEMON) {
                currentstate[PID] = SLEEPING;
            } else {
                currentstate[PID] = READY;
            }
            return PID;
        }
    }
    return -1;
}

int taskSet(int PID){
    if(PID< 160){
        taskremainingsleep[PID] = 10;
        taskremainingwork[PID] = 6;
        return DAEMON;
    }
    else if(PID< 180){
        taskremainingsleep[PID] = 0;
        taskremainingwork[PID] = 10;
        return IOINTENSIVE;
    }
    else{
        taskremainingsleep[PID] = 0;
        taskremainingwork[PID] = 20;
        return CPUINTENSIVE;
    }
}
   
void runCPU(int cpuIndex, int time) {
    if (CPU_PID[cpuIndex] == NOTPRESENT){ //if there is no task to run we exit
        return;  
    }
    
    int PID = CPU_PID[cpuIndex];
    
    //we say the task has run for one clock tick
    if (taskremainingwork[PID] > 0)
        taskremainingwork[PID]--;
    CPU_remainingSlice[cpuIndex]--;
    
    
    if (taskremainingwork[PID] <= 0) { //checking to see if the task has completed
        taskfinishtime[PID] = time;
        currentstate[PID] = FINISHED;
        CPU_PID[cpuIndex] = NOTPRESENT;
    }
    
    //if the process has run out of timeslice we:
    else if (CPU_remainingSlice[cpuIndex] <= 0) {
        if (tasktype[PID] == DAEMON) { //put it to sleep if daemon
            currentstate[PID] = SLEEPING;
            taskremainingsleep[PID] = 10;  
        } else if (tasktype[PID] == IOINTENSIVE) { //put in in I/O wait if I/O
            currentstate[PID] = IO_WAIT;
            taskremainingsleep[PID] = 3; 
        } else { //or we do nothing if CPU intensive
            currentstate[PID] = READY;
        }
       
        if (currentstate[PID] == READY)  // If task is READY, re-enqueue it.
            FIFOenqueue(PID);
        CPU_PID[cpuIndex] = NOTPRESENT;
    }
}

void updateDaemons(int time) {
    int PID;
    for (PID = 0; PID < MAX_TASKS; PID++) {
        if (tasktype[PID] == DAEMON && currentstate[PID] == SLEEPING) {
            if (taskremainingsleep[PID] > 0) { //it stays sleeping but we count for 1 sleep
                taskremainingsleep[PID]--;
            }
            if (taskremainingsleep[PID] == 0) { //we wake and set it ready to run
                currentstate[PID] = READY;
                FIFOenqueue(PID);
            }
        }
    }
}

void updateIOWaitTasks(int time) {
    for (int PID = 0; PID < MAX_TASKS; PID++) {
        if (tasktype[PID] == IOINTENSIVE && currentstate[PID] == IO_WAIT) {
            if (taskremainingsleep[PID] > 0)
                taskremainingsleep[PID]--;
            if (taskremainingsleep[PID] == 0) {
                currentstate[PID] = READY;
                FIFOenqueue(PID);
            }
        }
    }
}

void FIFOsimulate(void) {
    int time = 0;
    while (time < SIMULATION_TIME) {
        int newPID = generateRequests(time, 20); //gives us a 20% probablity of getting a new task
        if (newPID != -1) {
            if (tasktype[newPID] != DAEMON)
                FIFOenqueue(newPID);
        }
        
        updateDaemons(time);
        updateIOWaitTasks(time);
    
        //checks to see if there is a task on the CPU
        //if there is not, we pull one from the queue
        for (int i = 0; i < 4; i++) { 
            if (CPU_PID[i] == NOTPRESENT) { 
                int PID = FIFOdequeue();
                if (PID != -1 && currentstate[PID] == READY) {
                    CPU_PID[i] = PID;
                    CPU_remainingSlice[i] = 5;  //gives each process a 5 clock tick duration
                    currentstate[PID] = RUNNING;
                }
            }
        }
    
        for (int i = 0; i < 4; i++) { //one clock tick for each CPU
            runCPU(i, time);
        }

        
        time++;
    }
}

int FIFOenqueue(int PID){
    if(PID != -1){
        FIFOqueue[queueBack] = PID;
        queueBack = (queueBack+1)%MAX_TASKS;
    }
    return queueBack;
}

int FIFOdequeue(void) {
    if (queueFront == queueBack) {  // Queue empty.
        return -1;
    }
    int PID = FIFOqueue[queueFront];
    queueFront = (queueFront + 1) % MAX_TASKS;
    return PID;
}

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void shuffleArray(int arr[], int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        swap(&arr[i], &arr[j]);
    }
}

void MLFQsimulate(void) {
    
    int highQueue[QUEUE_SIZE], medQueue[QUEUE_SIZE], lowQueue[QUEUE_SIZE];
    int highFront = 0, highBack = 0;
    int medFront = 0, medBack = 0;
    int lowFront = 0, lowBack = 0;
    

    int time = 0;
    while (time < SIMULATION_TIME) {
       
        int newPID = generateRequests(time, 20); //20 percent chance per tick
        if (newPID != -1) {
            if (tasktype[newPID] != DAEMON) { //every new task is given highest priority to start
                taskpriority[newPID] = HIGHEST_PRIORITY;
                highQueue[highBack] = newPID;
                highBack = (highBack + 1) % QUEUE_SIZE;
            }
        }
        

        updateDaemons(time);
        updateIOWaitTasks(time);
        
        //The FIFO queue list is used as a helper so we must drain the list
        //This is so our simulator can also switch between FIFO and MLFQ scheduler by just calling a different method
        while (queueFront != queueBack) {
            int PID = FIFOdequeue();
            if (taskpriority[PID] == HIGHEST_PRIORITY) {
                highQueue[highBack] = PID;
                highBack = (highBack + 1) % QUEUE_SIZE;
            } else if (taskpriority[PID] == 1) {  // MEDIUM priority
                medQueue[medBack] = PID;
                medBack = (medBack + 1) % QUEUE_SIZE;
            } else {  // LOWEST priority
                lowQueue[lowBack] = PID;
                lowBack = (lowBack + 1) % QUEUE_SIZE;
            }
        }
        
        //For each CPU, if idle, schedule a task from the three local queues.
        for (int i = 0; i < 4; i++) {
            if (CPU_PID[i] == NOTPRESENT) {
                int PID = -1;
                // Try high-priority first.
                if (highFront != highBack) {
                    PID = highQueue[highFront];
                    highFront = (highFront + 1) % QUEUE_SIZE;
                } else if (medFront != medBack) {  // then medium.
                    PID = medQueue[medFront];
                    medFront = (medFront + 1) % QUEUE_SIZE;
                } else if (lowFront != lowBack) {  // then low.
                    PID = lowQueue[lowFront];
                    lowFront = (lowFront + 1) % QUEUE_SIZE;
                }
                if (PID != -1 && currentstate[PID] == READY) {
                    CPU_PID[i] = PID;
                    //Set CPU's time slice based on the task's current priority.
                    CPU_remainingSlice[i] = timeSliceForPriority[ taskpriority[PID] ];
                    currentstate[PID] = RUNNING;
                }
            }
        }
        
        //Let each CPU run one tick.
        for (int i = 0; i < 4; i++) {
            runCPU(i, time);
        }
        
        time++;
    }
}