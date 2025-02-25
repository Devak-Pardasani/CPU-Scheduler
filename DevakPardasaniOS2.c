#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>


#define SIMULATION_TIME 200
#define NOTPRESENT -1
#define SLEEPING 0
#define READY 1
#define RUNNING  2
#define IO_WAIT  3
#define CPUINTENSIVE 1
#define IOINTENSIVE 2
#define DAEMON 3

//not using structs
//arr declerations
//maximum 200 tasks PID=index
int taskarrivetime[200];
int tasktype[200];
int currentstate[200];
int taskremainingwork[200];
int taskremainingsleep[200];
int FIFOqueue[200];
int taskfinishtime[200];


int FIFOqueue[200];
int queueFront = 0;
int queueBack = 0; 


int CPU_PID[4];
int CPUremainingSlice[4];



//function declerations
int generateRequests(int, int);
int taskSet(int);
int FIFOenqueue(int);
void initializeTasks();
void updateDaemons(int);
void updateIOWaitTasks(int);
void FIFOsimulate();
int FIFOenqueue(int);
int FIFOdequeue();






int main(){
    srand(time(NULL));
    int numTasks = 0;
    initializeTasks();
    while(numTasks < 100){
        int process = generateRequests(0, 100);
        FIFOqueue[numTasks] = process;
        numTasks++;
    }
    FIFOsimulate();

    //MLFQsimulate();
    //AGINGsimulate();
    for(int i = 0; i < 200; i++){
        int responseTime = taskfinishtime[i] - taskarrivetime[i];
        printf("Task%d response time: ", i, responseTime);
    }
    return 0;
}

void initializeTasks() {
    for (int i = 0; i < 200; i++) {
        taskarrivetime[i] = NOTPRESENT;
        currentstate[i] = NOTPRESENT;
        taskfinishtime[i] = NOTPRESENT;
    }
    for (int i = 0; i < 4; i++) {
        CPU_PID[i] = NOTPRESENT;
        CPUremainingSlice[i] = 0;
    }
    queueFront = 0;
    queueBack = 0;
}

int generateRequests(int time, int prob) {
    if (rand() % 100 >= prob) { //gives us a prob% chance of 
        return -1; 
    }

    int PID;
    for (int i = 0; i < 200; i++) {
        PID = rand() % 200;  
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
        taskremainingwork[PID] = 2;
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
    CPUremainingSlice[cpuIndex]--;
    
    
    if (taskremainingwork[PID] <= 0) { //checking to see if the task has completed
        taskfinishtime[PID] = time;
        currentstate[PID] = NOTPRESENT;
        CPU_PID[cpuIndex] = NOTPRESENT;
    }
    
    //if the process has run out of clock ticks we:
    else if (CPUremainingSlice[cpuIndex] <= 0) {
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
    for (PID = 0; PID < 200; PID++) {
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
    for (int PID = 0; PID < 200; PID++) {
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

void FIFOsimulate() {
    int time = 0;
    while (time < SIMULATION_TIME) {
        int newPID = generateRequests(time, 20); //Gives us a 20% probablity of getting a new task
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
                    CPUremainingSlice[i] = 5;  //gives each process a 5 clock tick duration
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
        queueBack = (queueBack+1)%200;
    }
    return queueBack;
}

int FIFOdequeue() {
    if (queueFront == queueBack) {  // Queue empty.
        return -1;
    }
    int PID = FIFOqueue[queueFront];
    queueFront = (queueFront + 1) % 200;
    return PID;
}