#include "thread.h"
#include "server.h"
#include "event.h"
#include "script.h"
#include "distribution.h"
#include "config1class.h"


#include<bits/stdc++.h>
using namespace std;

//should start as a stream to generate random variables
auto start_main = chrono::high_resolution_clock::now();

int main(int argc, char** argv){

//=========================basic var declaretion 
    int NUM_RUNS;
    int NUM_CORES;
    int NUM_THREADS;
    int MAX_REQUEST_QUEUE_LENGTH;
    string POLICY;
    float QUANTUM_SIZE;
    int NUM_USERS;
    float CONTEXT_SWITCH_OVERHEAD;
    int STOPPING_CRITERION;
    string THINK_TIME_DISTRIBUTION;
    string SERVICE_TIME_DISTRIBUTION;
    string TIMEOUT_DISTRIBUTION;
    string Result_File;


    ConfigReader* cfg = ConfigReader::getInstance();
    cfg->parseFile("config.cfg");


    //=====basic var configuration 
    cfg->getValue("NUM_RUNS", NUM_RUNS);
    cfg->getValue("NUM_CORES", NUM_CORES);
    cfg->getValue("NUM_THREADS", NUM_THREADS);
    cfg->getValue("MAX_REQUEST_QUEUE_LENGTH", MAX_REQUEST_QUEUE_LENGTH);
    cfg->getValue("POLICY", POLICY);
    cfg->getValue("QUANTUM_SIZE", QUANTUM_SIZE);
    cfg->getValue("NUM_USERS", NUM_USERS);
    cfg->getValue("CONTEXT_SWITCH_OVERHEAD", CONTEXT_SWITCH_OVERHEAD);
    cfg->getValue("STOPPING_CRITERION", STOPPING_CRITERION);
    cfg->getValue("THINK_TIME_DISTRIBUTION", THINK_TIME_DISTRIBUTION);
    cfg->getValue("SERVICE_TIME_DISTRIBUTION", SERVICE_TIME_DISTRIBUTION);
    cfg->getValue("TIMEOUT_DISTRIBUTION", TIMEOUT_DISTRIBUTION);
    cfg->getValue("Result_File", Result_File);

    // printf("basic var configuration\n"); //debug
    
    //=========== helper var declaration 
    vector<float> think_params(7,0), timeout_params(7,0), servicetime_params(7,0);
    float SEED ;        //[0] seed for any
    cfg->getValue("SEED", SEED);
    think_params[0] = SEED;
    timeout_params[0] = SEED;
    servicetime_params[0] = SEED;

    float TTD_RANGE_A ;      //[1] lower range for uniform 
    float TTD_RANGE_B ;     //[2] higher range for uniform
    cfg->getValue("TTD_RANGE_A", TTD_RANGE_A);
    cfg->getValue("TTD_RANGE_B", TTD_RANGE_B);
    think_params[1] = TTD_RANGE_A;
    think_params[2] = TTD_RANGE_B;

    float TTD_LAMBDA ;     //[3] for exponential
    cfg->getValue("TTD_LAMBDA", TTD_LAMBDA);
    think_params[3] = TTD_LAMBDA;

    float TTD_CONSTANT_C ;   //[4] constant c+exponential 
    float TTD_LAMBDA_C ;     //[5] for c+exponential
    cfg->getValue("TTD_CONSTANT_C", TTD_CONSTANT_C);
    cfg->getValue("TTD_LAMBDA_C", TTD_LAMBDA_C);
    think_params[4] = TTD_CONSTANT_C;
    think_params[5] = TTD_LAMBDA_C;

    float TTD_CONSTANT_D  ;   //[6] constant ditiministic
    cfg->getValue("TTD_CONSTANT_D", TTD_CONSTANT_D);
    think_params[6] = TTD_CONSTANT_D;
    //===========think time done 

    float STD_RANGE_A ;      //[1] lower range for uniform 
    float STD_RANGE_B ;     //[2] higher range for uniform
    cfg->getValue("STD_RANGE_A", STD_RANGE_A);
    cfg->getValue("STD_RANGE_B", STD_RANGE_B);
    servicetime_params[1] = STD_RANGE_A;
    servicetime_params[2] = STD_RANGE_B;

    float STD_LAMBDA ;     //[3] for exponential
    cfg->getValue("STD_LAMBDA", STD_LAMBDA);
    servicetime_params[3] = STD_LAMBDA;

    float STD_CONSTANT_C ;   //[4] constant c+exponential 
    float STD_LAMBDA_C ;     //[5] for c+exponential
    cfg->getValue("STD_CONSTANT_C", STD_CONSTANT_C);
    cfg->getValue("STD_LAMBDA_C", STD_LAMBDA_C);
    servicetime_params[4] = STD_CONSTANT_C;
    servicetime_params[5] = STD_LAMBDA_C;

    float STD_CONSTANT_D  ;   //[6] constant ditiministic
    cfg->getValue("STD_CONSTANT_D", STD_CONSTANT_D);
    servicetime_params[6] = STD_CONSTANT_D;
    //==================service time params done 

    float TOD_RANGE_A ;      //[1] lower range for uniform 
    float TOD_RANGE_B ;     //[2] higher range for uniform
    cfg->getValue("TOD_RANGE_A", TOD_RANGE_A);
    cfg->getValue("TOD_RANGE_B", TOD_RANGE_B);
    timeout_params[1] = TOD_RANGE_A;
    timeout_params[2] = TOD_RANGE_B;

    float TOD_LAMBDA ;     //[3] for exponential
    cfg->getValue("TOD_LAMBDA", TOD_LAMBDA);
    timeout_params[3] = TOD_LAMBDA;

    float TOD_CONSTANT_C ;   //[4] constant c+exponential 
    float TOD_LAMBDA_C ;     //[5] for c+exponential
    cfg->getValue("TOD_CONSTANT_C", TOD_CONSTANT_C);
    cfg->getValue("TOD_LAMBDA_C", TOD_LAMBDA_C);
    timeout_params[4] = TOD_CONSTANT_C;
    timeout_params[5] = TOD_LAMBDA_C;

    float TOD_CONSTANT_D  ;   //[6] constant ditiministic
    cfg->getValue("TOD_CONSTANT_D", TOD_CONSTANT_D);
    timeout_params[6] = TOD_CONSTANT_D;
    //==================time out params  done 
    // printf("heler var configuration taken\n"); exit(2); //debug

    //====================printing things into configuration file
    // printf("dump values in config.cfg ---------------\n");
    cout << endl << "=================================================" << endl << endl;
    cfg->dumpFileValues();  
    cout << endl << "=================================================" << endl << endl;
//================================================parsing done 

    //=======for plotting 
    // NUM_USERS = atoi(argv[1]);
    // Result_File = argv[2];
    //cout<<NUM_USERS<<endl;

    vector<string> info_list;                        // list for logs information

    for (int z = 0; z < NUM_RUNS; ++z)
    {
        string temp7 = "RUN\t"+to_string(z);
        info_list.push_back(temp7);                  //Initialisation of each run
        vector<Core*> cores;                         // cores in system
        for (int i = 0; i < NUM_CORES; ++i)
        {   // initialization of cores
            Buffer * temp_buffer = new Buffer(i);
            Core* temp1 = new Core(i, temp_buffer, POLICY, QUANTUM_SIZE);
            cores.push_back(temp1);
        }
        CoreHandler* core_handler = new CoreHandler(cores);      // instance of core handler

        EventList* event_list = new EventList();                                 // instance of event list
        ThreadList* thread_list = new ThreadList(NUM_THREADS);                   // instance of thread list
        RequestQueue* request_queue = new RequestQueue(MAX_REQUEST_QUEUE_LENGTH);// instance of request queue
        // printf("check0\n"); request_queue->print_me(); exit(2); //debug

        float sim_time = 0.0;                      // current simulation time
        int request_id = 0;
        int num_serviced = 0;                     // no of request dropped, timeout or departed
        float start_time, timeout, service_time;

        for (int i = 0; i < NUM_USERS; ++i)
        {   // initialization of event list
            start_time = sim_time + get_random_variate(THINK_TIME_DISTRIBUTION,think_params); 
            timeout = get_random_variate(TIMEOUT_DISTRIBUTION, timeout_params);
            service_time = get_random_variate(SERVICE_TIME_DISTRIBUTION, servicetime_params);

            // attribute initialization
            map<string, float> attribute;
            attribute.insert({"id",(float)request_id});
            attribute.insert({"timeout",timeout});
            attribute.insert({"service_time",service_time});
            event_list->addEvent(create_request, start_time, attribute);
            request_id++;
            // printf("check point 1:\n"); exit(2); //debug
        }

        // for each run
        while(num_serviced<STOPPING_CRITERION)
        {   // event_list->print_me(); exit(2);//debug
            Event* event = event_list->getNextEvent();
            if(event == NULL){ break;}  // nothing is left now

            float prev_sim_time = sim_time;
            sim_time = event->start_time;
            string temp6 = "TIME\t" + to_string(sim_time) + "\t" + to_string(core_handler->getUtilization()) + "\t" +  to_string(thread_list->running_threads.size()) + "\t" + to_string(request_queue->request_list.size());
            info_list.push_back(temp6);
            // printf("check point 3:\n"); exit(2); //debug

            if (event->event_type == create_request)
            {   
                Request* request = new Request((int)event->event_id, event->attr["timeout"], event->attr["service_time"], event->start_time);
                Thread* response = thread_list->getThreadToRunOnCpu(request);
                // request->print_me(); printf("check point 4:\n"); exit(2) ; //debug

                if (response == NULL)
                {   
                    bool queue_response = request_queue->addToQueue(request);    
                    if (queue_response == false)
                    {
                        num_serviced++;
                        string temp5 = "DROP\t" + to_string(request->id) + "\t" + to_string(request->timestamp);
                        info_list.push_back(temp5);
                        start_time = sim_time + get_random_variate(THINK_TIME_DISTRIBUTION,think_params); // works only after completion of random number generator
                        timeout = get_random_variate(TIMEOUT_DISTRIBUTION, timeout_params);
                        service_time = get_random_variate(SERVICE_TIME_DISTRIBUTION, servicetime_params);
                        
                        // printf("hello 1.0\n"); exit(2); // debug
                        map<string, float> attribute;
                        attribute.insert({"id",request_id});
                        attribute.insert({"timeout",timeout});
                        attribute.insert({"service_time",service_time});
                        event_list->addEvent(create_request, start_time, attribute);
                        attribute.clear();
                        request_id++;
                    }
                }
                else{ // providing space in core buffer to request in thread
                    core_handler->getCore(response, thread_list, event_list, sim_time);
                }
            }

            else if(event->event_type == departure){
                    num_serviced +=1;
                    int core_id = event->attr["core_id"];
                    Thread* request = core_handler->core_list[core_id]->runningThread;
                    //printf("DEAR\n"); core_handler->print_me(); printf("done\n"); exit(2); // debug
                    core_handler->core_list[core_id]->Departure(thread_list, event_list, sim_time);
                    if (request->request->timeout + request->request->timestamp < sim_time)
                    {
                        string temp3 = "TIMEOUT\t" + to_string(request->request->id) + "\t" + to_string(request->request->timestamp);
                        info_list.push_back(temp3);


                    }
                    else{
                        string temp4 = "DEPART\t" + to_string(request->request->id) + "\t" + to_string(request->request->timestamp);
                        info_list.push_back(temp4);
                    }

                    Request* request1 = request_queue->removeFromQueue();   

                    if (request1 != NULL)
                    {
                        Thread* response = thread_list->getThreadToRunOnCpu(request1);
                        core_handler->getCore(response, thread_list, event_list, sim_time);
                    }

                    start_time = sim_time + get_random_variate(THINK_TIME_DISTRIBUTION,think_params); // works only after completion of random number generator
                    timeout = get_random_variate(TIMEOUT_DISTRIBUTION, timeout_params);
                    service_time = get_random_variate(SERVICE_TIME_DISTRIBUTION, servicetime_params);

                    map<string, float> attribute;
                    attribute.insert({"id",request_id});
                    attribute.insert({"timeout",timeout});
                    attribute.insert({"service_time",service_time});
                    event_list->addEvent(create_request, start_time, attribute);
                    attribute.clear();
                    request_id++;
                    //cout<<"i am here "<<endl;
                }

            else if(event->event_type == end_quantum){
                Core* core = core_handler->core_list[event->attr["core_id"]];
                if(core->buffer->isEmpty()){
                    core->idle = true;
                    core->buffer->addJob(core->runningThread);
                    core->checkBuffer(thread_list, event_list, sim_time);
                }
                else{
                    map<string, float> attribute;
                    attribute.insert({"core_id", event->attr["core_id"]});
                    event_list->addEvent(switch_context, sim_time+ CONTEXT_SWITCH_OVERHEAD, attribute);
                }
            }

            else if(event->event_type == switch_context){
                //cout<<"its switch context "<<endl;
                Core* core = core_handler->core_list[event->attr["core_id"]];
                string temp2 = "CONTEXT\t" + to_string(core->ID);
                info_list.push_back(temp2);
                core->buffer->addJob(core->runningThread);
                core->runningThread = NULL;
                core->idle = true;
                core->checkBuffer(thread_list, event_list, sim_time);
            }
        }
        // printf("hurrey %d\n", num_serviced); // debug
    }

//================================================================creating output log
    FILE* tout = fopen("output.log", "w+");
    for(auto item: info_list){
        fprintf(tout, "%s\n", item.c_str());
    }

    auto end_main = chrono::high_resolution_clock::now();

    double time_taken_by_main = chrono::duration_cast<chrono::nanoseconds>(end_main - start_main).count();
    time_taken_by_main *= 1e-9;
    cout << "Main Time : " << fixed << time_taken_by_main << setprecision(9);
    cout << " sec" << endl;

    auto start_script = chrono::high_resolution_clock::now();
    // printf("script started\n"); exit(1); // debug
    script(info_list, STOPPING_CRITERION/2, Result_File);
    // printf("check point 8: script done \n"); exit(1); // debug
    auto end_script = chrono::high_resolution_clock::now();

    double time_taken_by_script = chrono::duration_cast<chrono::nanoseconds>(end_script - start_script).count();
    time_taken_by_main *= 1e-9;
    cout << "Script Time : " << fixed << time_taken_by_main << setprecision(9);
    cout << " sec" << endl;

    // printf("mission accomplished\n"); // debug
    return 0;
}
