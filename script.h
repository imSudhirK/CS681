#ifndef SCRIPT_H
#define SCRIPT_H

#include<bits/stdc++.h>

using namespace std;


int Index = 0;


string getNextLine(vector<string> info_list){
	int tempIND = Index;
	Index +=1;
	return info_list[tempIND];
}

vector<string> split (const string &s, char delim) {
    vector<string> result;
    stringstream ss (s);
    string item;

    while (getline (ss, item, delim)) {
        result.push_back (item);
    }

    return result;
}

float mean(vector<float> v){
	float sum = accumulate(v.begin(), v.end(), 0.0);
	float length = v.size();
	return sum/length;
}

float std_dev(vector<float> v){
	float mean_avg = mean(v);
	float std_deviation;
	for (int i = 0; i < v.size(); ++i)
	{
		std_deviation += pow(v[i]-mean_avg, 2);
	}
	return sqrt(std_deviation/v.size());
}

void script(vector<string> info_list, int exclusion, string result_file){
	vector< vector<float> > num_users;
	vector<float> time_list;
	vector< vector<float> > response_time_list;
	vector< vector<float> > utilisation_list;
	vector<int> context_switch_list;
	vector<int> drop_list;
	vector<int> goodput_list;
	vector<int>timeout_list;

	float current_time, previous_time, transient_time = 0.0;

	while(Index < info_list.size()){
		string line = getNextLine(info_list);
		vector<string> words = split(line, '\t');
		string token = words[0];

		if (token == "RUN")
		{
		 	time_list.push_back(current_time-transient_time);
		 	int count = 0;
		 	while(count < exclusion){
		 		words = split(getNextLine( info_list), '\t');
		 		token = words[0];
		 		if (token == "TIMEOUT" || token == "DEPART" || token == "DROP")
		 		{
		 			count += 1;
		 		}
		 	}
		 	words = split(getNextLine( info_list), '\t');
		 	token = words[0];
		 	assert(token == "TIME");

		 	previous_time = stof(words[1]);
		 	current_time = stof(words[1]);
		 	transient_time = stof(words[1]);
		
		 	drop_list.push_back(0);
		 	goodput_list.push_back(0);
		 	timeout_list.push_back(0);
		 	context_switch_list.push_back(0);

		 	utilisation_list.push_back({});
		 	num_users.push_back({});
		 	response_time_list.push_back({});
		}

		else if (token == "TIME")
		{
		 	previous_time = current_time;
		 	current_time = stof(words[1]);
		 	utilisation_list.back().push_back((current_time - previous_time)* stof(words[2]));
		 	num_users.back().push_back((current_time - previous_time)* stof(words[3]));
		}

		else if (token == "CONTEXT")
		{
		 	context_switch_list.back() += 1;
		}

		else if (token == "TIMEOUT")
		{
			timeout_list.back() += 1;
			response_time_list.back().push_back(current_time - stof(words[2]));
		}

		else if (token == "DROP")
		{
			drop_list.back() += 1;
		}

		else if (token == "DEPART")
		{
			goodput_list.back() += 1;
			response_time_list.back().push_back(current_time - stof(words[2]));
		}

		else{
			break;
		}
	}

	time_list.erase(time_list.begin());
	time_list.push_back(current_time - transient_time);

	vector<float> response_time_list_final;
	for (int i = 0; i < response_time_list.size(); ++i)
	 {
	 	float sum = accumulate(response_time_list[i].begin(), response_time_list[i].end(), 0);
	 	float length = response_time_list[i].size();
	 	response_time_list_final.push_back(sum/length);
	 }


	vector<float> utilisation_list_final;
	if (utilisation_list.size() == time_list.size())
	 {
	 	for (int i = 0; i < utilisation_list.size(); ++i)
	 	{
	 		float sum = accumulate(utilisation_list[i].begin(), utilisation_list[i].end(), 0.0);
	 		utilisation_list_final.push_back(sum/time_list[i]);
	 	}
	 }

	vector<float> num_users_final;
	if (num_users.size() == time_list.size())
	 {
	 	for (int i = 0; i < num_users.size(); ++i)
	 	{
	 		float sum = accumulate(num_users[i].begin(), num_users[i].end(), 0.0);
	 		num_users_final.push_back(sum/time_list[i]);
	 	}
	 }

	 vector<float> context_switch_list_final;
	 if (context_switch_list.size() == goodput_list.size() && goodput_list.size() == timeout_list.size())
	 {
	 	for (int i = 0; i < timeout_list.size(); ++i)
	 	{
	 		int denomi = goodput_list[i] + timeout_list[i];
	 		context_switch_list_final.push_back((float)context_switch_list[i]/(float)denomi);
	 	}
	 }

	vector<float> drop_list_final;
	if (drop_list.size() == time_list.size())
	 {
	 	for (int i = 0; i < time_list.size(); ++i)
	 	{
	 		drop_list_final.push_back((float)drop_list[i]/time_list[i]);
	 	}
	 }

	vector<float> goodput_list_final;
	if (goodput_list.size() == time_list.size())
	 {
	 	for (int i = 0; i < time_list.size(); ++i)
	 	{
	 		goodput_list_final.push_back((float)goodput_list[i]/time_list[i]);
	 	}
	 }

	vector<float> timeout_list_final;
	if (timeout_list.size() == time_list.size())
	 {
	 	for (int i = 0; i < time_list.size(); ++i)
	 	{
	 		timeout_list_final.push_back((float)timeout_list[i]/time_list[i]);
	 	}
	 }

	vector<float> throughput_list_final;
	if (goodput_list.size() == timeout_list.size())
	 {
	 	for (int i = 0; i < timeout_list.size(); ++i)
	 	{
	 		throughput_list_final.push_back((float)goodput_list_final[i]+(float)timeout_list_final[i]);
	 	}
	 }


	cout<<"###################################### Averages ######################################"<<endl;
	cout<<"Observation time: "<<mean(time_list)<<" in ms "<<endl;
	cout<<"Avg. drop rate: "<<mean(drop_list_final)<<" reqs per ms "<<endl;
	cout<<"Avg. goodput: "<<mean(goodput_list_final)<<" reqs per ms "<<endl;
	cout<<"Avg. badput: "<<mean(timeout_list_final)<<" reqs per ms "<<endl;
	cout<<"Avg. throughput: "<<mean(throughput_list_final)<<" reqs per ms"<<endl;
	cout<<"Avg. no. of context switch per request: "<<mean(context_switch_list_final)<<" switches per req"<<endl;
	cout<<"Avg. utilisation: "<<mean(utilisation_list_final)<<" fraction "<<endl;
	cout<<"Avg. response time: "<<mean(response_time_list_final)<<" in ms "<<endl;
	cout<<"Avg. no. of users in system: "<<mean(num_users_final)<<" number "<<endl;
	cout<<"######################################################################################"<<endl;

	ofstream res_file;
	res_file.open(result_file, ios::app);
	string stats = to_string(mean(response_time_list_final)) + " " + to_string(std_dev(response_time_list_final)) + " " + to_string(mean(utilisation_list_final)) + " " + to_string(mean(context_switch_list_final)) + " " + to_string(mean(goodput_list_final)) + " " + to_string(mean(timeout_list_final)) + " " + to_string(mean(throughput_list_final)) + " " + to_string(mean(drop_list_final)) + "\n";
	res_file << stats;
}

#endif
