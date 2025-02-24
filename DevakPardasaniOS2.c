#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define SIMULATION_TIME 1000
#define NOTPRESENT -1
#define SLEEPING 0
#define RUNNINGONCPU1 1
#define RUNNINGONCPU2 2
#define INQUEUE1 11
#define CPUINTENSIVE 1
#define IOINTENSIVE 2
#define DAEMON 3

//arr declerations
//maximum 200 tasks PID=index
int taskarrivetime[200];
int tasktype[200];
int currentstate[200];
int taskremainingwork[200];
int taskremainingsleep[200];
int FIFOqueue[200];
int CPU1[2];
int CPU2[2];
int CPU3[2];
int CPU4[2];



//function declerations
int generateRequests(int);
int taskSet(int);
int FIFOenqueue(int, int);
void runCPU1(int);
void runCPU2(int);
void runCPU3(int);
void runCPU4(int);








int main(){
    srand(time(NULL));
    int numTasks = 0;
    while(numTasks < 100){
        int process = generateRequests(0);
        if(process > 0){
            FIFOqueue[numTasks] = process;
            numTasks++;
        }
    }
    FIFOsimulate();
    //MLFQsimulate();
    //AGINGsimulate();
    return 0;
}

int generateRequests(int time){
    //randomly grab a process and say it "wakes up"
    //store PID in a list and set as an "active task" -- scheduler will be implemented later
    int PID = rand() % 401; //gives us a 50% chance of landing between 0-200;
    if(PID<=200 && time > taskarrivetime[PID]){
        taskarrivetime[PID] = time;
        tasktype[PID] = taskSet(PID);//0-159 daemons -- 160-179 I/O intensive -- 180-200 CPU intensive
        return PID;
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
    int numTasks = backQueue - frontQueue;
    CPU1[0] = -1;
    CPU2[0] = -1;
    CPU3[0] = -1;
    CPU4[0] = -1;
    while(time < SIMULATION_TIME){
        if(numTasks < 200){
            backQueue = (FIFOenqueue(generateRequests(time), backQueue));
        }
        backQueue = runCPU1(backQueue)

        //enque -- generateRequests(time);
        //we need to create 4 CPUs
        //shared FIFO queue, give next in queue to available CPU
        //set some "timeslice" -- 
        //CPU1(currentPID);
        
        //each CPU will have some return depending on whether or not it "accepted" the process


        time++;
    }

}



int FIFOenqueue(int PID, int backQueue){
    if(PID != -1){
        FIFOqueue[backQueue] = PID;
        backQueue = (backQueue+1)%201;
    }
    return backQueue;
}

void runCPU1(int queueNum){
    if(FIFOqueue[queueNum] == -1){
        return 0;
    }

    if(CPU1[0] == -1){
        CPU1[0] = FIFOqueue[queueNum];
        CPU1[1] = 5; //corresponds to how many clock ticks we give each process
    }

    
    if(taskremainingwork[CPU1[0]])
}

void runCPU2(int queueNum){
    if(FIFOqueue[queueNum] == -1){
        return 0;
    }
    if(CPU2[0] == -1){
        CPU2[0] = FIFOqueue[queueNum];
        CPU2[1] = 5; //corresponds to how many clock ticks we give each process
    }
    //check process type and run the process accordingly
}

void runCPU3(int queueNum){
    if(FIFOqueue[queueNum] == -1){
        return 0;
    }
    if(CPU3[0] == -1){
        CPU3[0] = FIFOqueue[queueNum];
        CPU3[1] = 5; //corresponds to how many clock ticks we give each process
    }
    //check process type and run the process accordingly
}

void runCPU4(int queueNum){
    if(FIFOqueue[queueNum] == -1){
        return 0;
    }
    if(CPU4[0] == -1){
        CPU4[0] = FIFOqueue[queueNum];
        CPU4[1] = 5; //corresponds to how many clock ticks we give each process
    }
    //check process type and run the process accordingly
}