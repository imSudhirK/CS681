#ifndef SERVER_H
#define SERVER_H

#include "thread.h"

using namespace std;



class Buffer{
public:
    int ID;                          // buffer id for core
    vector<Thread*> buffer_list;     // list of threads currently in buffer

    Buffer(int id){
        ID = id;
        vector<Thread*> bfl;
        buffer_list = bfl;
    }

    // returns first job (thread) from buffer
    Thread* getNextJob(){
        if(buffer_list.empty()){
            //printf("buffer is empty\n");
            return NULL;
        }else{
            Thread* ret_thr = buffer_list[0];
            buffer_list.erase(buffer_list.begin());
            return ret_thr;
        }
    }

    // returns length of buffer list
    int getBufferLength(){
        return buffer_list.size();
    }

    // is buffer list empty
    bool isEmpty(){
        return buffer_list.empty();
    }

    //add a job to buffer
    int addJob(Thread* job){
        buffer_list.push_back(job);
        return 1;
    }

    void print_me(){
        printf("Buffer:\n");
        printf("buffer id: %d\t", ID);
        for(auto tr: buffer_list){
            tr->print_me();
        }
        printf("\n------------print buffer end----------\n");
    }
};


class Core{
public:
    int ID;                   // core id
    Buffer* buffer;           // its buffer
    string policy;            // scheduling policy
    float quantum_size;       // time quantum to run a job
    bool idle;                // is core idle 
    Thread* runningThread;    // thread, if core is busy


    Core(int id, Buffer* bfr, string plcy, float tq){
        ID= id;
        buffer = bfr;
        policy = plcy;
        quantum_size = tq;
        idle = true;
        runningThread = NULL;
    }

    // whether the core is idle 
    bool isIdle(){
        return idle;
    }

    // if server is idle, is there somthing to schedule into CPU
    void checkBuffer(ThreadList* tl,  EventList* el, float st){
        if(!idle){
            printf("error core is not idle\n");
        }
        else{
            Thread* job = buffer->getNextJob();
            if(job != NULL){
                if(policy == "roundRobin"){
                    idle = false;
                    runningThread = job;
                    if(job->request->time_required - job->request->time_spentonCPU <= quantum_size){
                        float starttime = st + job->request->time_required - job->request->time_spentonCPU;
                        map<string, float> attribute;
                        attribute.insert({"core_id", ID});
                        el->addEvent(departure, starttime, attribute);
                    }
                    else{
                        float starttime = st + quantum_size;
                        job->request->time_spentonCPU += quantum_size;
                        map<string, float> attribute;
                        attribute.insert({"core_id", ID});
                        el->addEvent(end_quantum, starttime, attribute);
                    }
                }
            }
        }
    }

    // request is completed, it should be departed 
    void Departure(ThreadList* tl,  EventList* el, float st){
        runningThread->request->time_spentonCPU = runningThread->request->time_required;
        tl->removeThread(runningThread->ID);
        idle = true;
        runningThread = NULL;
        checkBuffer(tl, el, st);
    }

    void print_me(){
        printf("Core : \n");
        printf("core id: %d\t", ID);
        printf("policy: %s\t", policy.c_str());
        printf("quantum size: %f\t", quantum_size);
        printf("idle: %d\n", idle);
        buffer->print_me();
        runningThread->print_me();
        printf("\n------------print core end----------\n");   
    }  
};

// decides which core to assign to a particular thread
class CoreHandler{
public:
    vector<Core*> core_list;

    CoreHandler(vector<Core*> cl){
        core_list = cl;
    }

    // add thread to run on core with lowest load
    void getCore(Thread* thr, ThreadList* thrdl, EventList* evl, float st){
        vector<int> list_loads;
        for(auto cs: core_list){
            int tempi = cs->buffer->getBufferLength();
            if(!cs->isIdle()) tempi++;
            list_loads.push_back(tempi);
        }

        int minind = min_element(list_loads.begin(), list_loads.end()) - list_loads.begin();
        Core* newcore = core_list[minind];
        newcore->buffer->addJob(thr);
        if(newcore->isIdle()){
            newcore->checkBuffer(thrdl, evl, st);
        }
    }

    // over all utilization at any time
    float getUtilization(){
        float utz=0;
        for(auto cs: core_list){
            if(!cs->isIdle()) utz++;
        }
        return utz/core_list.size();
    }

    void print_me(){
        printf("core handler : \n");
        for(auto cr: core_list){
            cr->print_me();
        }
        printf("\n------------print corehandler end----------\n");
    }
};

#endif