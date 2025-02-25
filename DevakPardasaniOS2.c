#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define READY 1
#define SIMULATION_TIME 1000
#define NOTPRESENT -1
#define SLEEPING 0
#define RUNNINGONCPU1 1
#define RUNNINGONCPU2 2
#define RUNNINGONCPU3 3
#define RUNNINGONCPU4 4

#define INQUEUE1 11
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
void updateDaemons(int time);






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

int generateRequests(int tim, int prob) {
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
        taskremainingwork[PID] = 5;
        return IOINTENSIVE;
    }
    else{
        taskremainingsleep[PID] = 0;
        taskremainingwork[PID] = 10;
        return CPUINTENSIVE;
    }
}

void FIFOsimulate(){
    int time = 0;
    int frontQueue = 0;
    int backQueue = 100;
    int numTasks = 100;
    
    while(time < SIMULATION_TIME){
        
        if(numTasks < 200){
            backQueue = (FIFOenqueue(generateRequests(time, 20)));
        }


        
        numTasks = backQueue - frontQueue; //calculate numtasks
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

int FIFOdequeue(void) {
    if (queueFront == queueBack) {  // Queue empty.
        return -1;
    }
    int PID = FIFOqueue[queueFront];
    queueFront = (queueFront + 1) % 200;
    return PID;
}