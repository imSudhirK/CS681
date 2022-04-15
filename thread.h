#ifndef THREAD_H
#define THREAD_H

#include "event.h"

using namespace std;


class Request{
public:
    int id;                    // request ID 
    float timeout;             // time to wait before timeout 
    float time_required;       // time required for complition
    float timestamp;           // creation time 
    float time_spentonCPU;     // Total time spent on cpu so far

    Request(int i, float to, float tr, float ts){
        id =  i;
        timeout = to;
        time_required = tr;
        timestamp = ts;
        time_spentonCPU = 0;
    }

    void print_me(){
        printf("Request ID: %d\t", id);
        printf("Time out : %f\t", timeout);
        printf("Time Stamp: %f\t", timestamp);
        printf("Time required: %f\t", time_required);
        printf("Time Spent on CPU: %f\n", time_spentonCPU);
        printf("\n------------print request complited----------\n");
    }
};


class Thread{
public:
    Request* request;               // request it have
    int ID;                         // thread ID

    Thread(Request* rq, int id){
        request = rq;
        ID = id;
    }

    void print_me(){
        printf("Thread ID: %d\t", ID);
        request->print_me();
        printf("\n------------print thread end----------\n");
    }

};

class ThreadList{
public:
    int max_threads;                      //maximum nmber of threads in the list    
    vector<Thread*> running_threads;      //list of threads, currently in use 
    vector<int> available_threads;        //threads ID(only) available for new request 

    ThreadList(int mn){
        max_threads = mn;
        vector<Thread*> vec;
        running_threads = vec;
        for(int i=1; i<=max_threads; i++){ 
            available_threads.push_back(i);
        }
    }

    // is there any thread available for new request 
    bool isThreadAvailableForARequest(){
        return !available_threads.empty();
    }

    // allocate a thread to request if available otherwise null
    Thread* getThreadToRunOnCpu(Request* request){
        if(!isThreadAvailableForARequest()){
            // printf("No thread is available\n");
            return NULL;                        
        }else{
            Thread* newthread = new Thread(request, available_threads[0]);
            available_threads.erase(available_threads.begin());
            running_threads.push_back(newthread);

            return newthread;
        }
    }

    // if request is completed or timeout 
    // mark corresponding thread ID available
    void removeThread(int id){
        int tempr=-1;
        for(auto thrd: running_threads){
            tempr++;
            if(thrd->ID == id){
                available_threads.push_back(thrd->ID);
                running_threads.erase(running_threads.begin()+tempr);
                break;
            }
        }

    }

    void print_me(){
        printf("Thread List:\n");
        printf("max num threads: %d\n", max_threads);
        for(auto rt: running_threads){
            rt->print_me();
        }
        printf("Available threads: \t");
        for(int i=0; i<available_threads.size(); i++){
            printf("%d\t", available_threads[i]);
        }
        printf("\n------------print threadlist end----------\n");
    }

};


class RequestQueue{
public:
    int maxQlength;                  // max no of request queue can hold 
    vector<Request*> request_list;   // list of request currently in queue

    RequestQueue(int mql){
        maxQlength = mql;
        vector<Request*> rl;
        request_list = rl;

    }

    // add a request into queue if not full
    // otherwise drop
    bool addToQueue(Request* rq){
        if(request_list.size() == maxQlength){
            // printf("request queue is already full\n"); 
            return false;
        }
        else{
            request_list.push_back(rq);
            return true;
        }
    }

    // is request queue empty 
    bool isEmpty(){
        return request_list.empty();
    }

    // remove first request from queue
    Request* removeFromQueue(){
        if(isEmpty()){
            //printf("request list is already empty\n");
            return NULL;
        }
        else{
            Request* ret_req = request_list[0];
            request_list.erase(request_list.begin());
            return ret_req;
        }

    }

    void print_me(){
        printf("Request Queue\n");
        printf("max queue length: %d\t",  maxQlength);
        for(auto rl: request_list){
            rl->print_me();
        }
        printf("\n------------print trequestqueue end----------\n");
    }
};

#endif