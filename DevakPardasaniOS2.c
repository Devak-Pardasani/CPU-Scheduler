#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIMULATION_TIME 1000
#define NOTPRESENT -1
#define SLEEPING 0
#define RUNNINGONCPU1 1
#define RUNNINGONCPU2 2
#define INQUEUE1 11
#define CPUINTENSIVE 1
#define IOINTENSIVE 2
#define DAEMON 3

//maximum 200 tasks PID=index
int taskarrivetime[200];
int tasktype[200];
int currentstate[200];
int taskremainingwork[200];
int taskremainingsleep[200];
int FIFOqueue[200];


int main(){
    srand(time(NULL));
    FIFOsimulate();
    //MLFQsimulate();
    //AGINGsimulate();
    return 0;
}

void FIFOsimulate(){
    int time = 0;
    int currentposition = 0;
    while(time < SIMULATION_TIME){
        currentposition = (generateRequests(time, currentposition)) ? currentposition++ : currentposition;
        //enque -- generateRequests(time);
        int currentTask = 0;
        //we need to create 4 CPUs
        //shared FIFO queue, give next in queue to available CPU
        //set some "timeslice" -- 
        CPU1(time);
        CPU2(time);
        CPU3(time);
        CPU4(time);

        time++;
    }

}

void MLFQsimulate(){
    int time = 0;
    while(time < SIMULATION_TIME){

        // int currentTask = nextInQUEUE


        time++;
    }

}

void AGINGsimulate(){
    int time = 0;
    while(time < SIMULATION_TIME){
        //enque -- generateRequests(time);
        // int currentTask = nextInQUEUE


        time++;
    }

}

int generateRequests(int time){
    //randomly grab a process and say it "wakes up"
    //store PID in a list and set as an "active task" -- scheduler will be implemented later
    int PID = rand() % 401; //gives us a 50% chance of landing between 0-200;
    if(PID<=200 && time > taskarrivetime[PID]){
        taskarrivetime[PID] = time;
        tasktype[PID] = getTaskType(PID);//0-159 daemons -- 160-179 I/O intensive -- 180-200 CPU intensive
        return PID;
    }
    return -1;
}

int getTaskType(int PID){
    if(PID< 160){
        return DAEMON;
    }
    else if(PID< 180){
        return IOINTENSIVE;
    }
    else{
        return CPUINTENSIVE;
    }
}

bool FIFOenqueue(int PID, int currentposition){
    if(PID != -1){
        FIFOqueue[currentposition] = PID;
        return 1;
    }
    return 0;
}

void CPU1(){

}

void CPU2(){

}

void CPU3(){

}

void CPU4(){

}