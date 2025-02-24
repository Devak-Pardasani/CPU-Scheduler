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
bool FIFOenqueue(int, int);









int main(){
    srand(time(NULL));
    int currProc  = generateRequests(3);
    //FIFOsimulate();
    //MLFQsimulate();
    //AGINGsimulate();
    printf("%d\n", currProc);
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
        taskremainingwork[PID] = 3;
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
    int backQueue = 0;
    while(time < SIMULATION_TIME){
        backQueue = (FIFOenqueue(generateRequests(time), backQueue)) ? (backQueue+1)%201 : backQueue;
        //enque -- generateRequests(time);
        //we need to create 4 CPUs
        //shared FIFO queue, give next in queue to available CPU
        //set some "timeslice" -- 
        //CPU1(currentPID);
        
        //each CPU will have some return depending on whether or not it "accepted" the process


        time++;
    }

}

/*void MLFQsimulate(){
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

}*/


bool FIFOenqueue(int PID, int frontQueue){
    /*if(PID != -1){
        FIFOqueue[currentposition] = PID;
        return 1;
    }*/
    return 0;
}
/*
int updateQueue(bool val, int frontQueue){
    if(val){
        frontQueue = (frontQueue + 1)%201;
    }
    return frontQueue;
}

bool CPU1(time){
    int currentPID;
    int remainingTimeslice;
    //when called we check what process is running, we will have some check to see what type of process
        remainingTimeslice--;


}

bool CPU2(time){
    int currentPID;
    int remainingTimeslice;
    //when called we check what process is running, we will have some check to see what type of process
        remainingTimeslice--;
}

bool CPU3(time){
    int currentPID;
    int remainingTimeslice;
    //when called we check what process is running, we will have some check to see what type of process
        remainingTimeslice--;
}

bool CPU4(time){
    int currentPID;
    int remainingTimeslice;
    //when called we check what process is running, we will have some check to see what type of process
        remainingTimeslice--;
}*/