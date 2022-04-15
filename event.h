#ifndef EVENT_H
#define EVENT_H

#include<bits/stdc++.h>
using namespace std;

enum EventType{
    create_request = 1,         //Attributes : ID, timeout, service_time
    end_quantum = 2,            //Attributes : core_id
    switch_context = 3,         //Attributes : core_id
    departure = 4               //Attributes : core_id
};


class Event{
public:
    int event_id;                       // event ID
    int event_type;                     // event type could be from (1-4)
    float start_time;                   // start time (time when to start event)
    map<string, float> attr;            // attributes of event 

    Event(int id, int et, float st, map<string, float> attribute){
        event_id = id;
        event_type = et;
        start_time = st;
        attr = attribute;
    }

    void print_me(){
        printf("Event items: \n");
        printf("event id: %d\t", event_id);
        printf("event_type: %d\t", event_type);
        printf("start time : %f\t", start_time);
        printf("attribute: \n");
        for(auto at: attr){
            printf("%s: %f\t", at.first.c_str(), at.second);
        }
        printf("\n------------event complited----------\n");
    }
};

bool CompareEventId( Event* e1, Event* e2)
{
    return (e1->event_id < e2->event_id);
}

bool CompareEventStartTime( Event* e1, Event* e2)
{
    return (e1->start_time < e2->start_time);
}

class EventList{
public:
    vector<Event*> queue;             // list of events 
    int counter;                      // number of events
    
    EventList(){
        vector<Event*> npq;
        queue = npq;
        counter = 0;

    }

    //returns next event to execute or null if nothing
    Event* getNextEvent(){
        if(queue.empty()){
            
            return NULL;
        }
        Event* ret_event = queue[0];
        queue.erase(queue.begin());
        return ret_event;
    }
    
    //add an event to event list and increment counter 
    void addEvent(EventType et, float st, map<string, float> event_attribute){
        Event* newevent = new Event(counter, et, st, event_attribute);
        queue.push_back(newevent);
        sort(queue.begin(), queue.end(), CompareEventId);
        sort(queue.begin(), queue.end(), CompareEventStartTime);
        counter++;
    }

    //whether event list is empty 
    bool isEmpty(){
        return queue.empty();
    }

    void print_me(){
        printf("Event list: \n");
        printf("counter: %d\n", counter);
        for(auto el: queue){
            el->print_me();
        }
        printf("\n------------eventlist complited----------\n");
    }
};

#endif