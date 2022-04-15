#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H

#include<bits/stdc++.h>


float get_random_variate(std::string dist_, std::vector<float> &param){

    std::random_device rd;
    std::mt19937 gen(rd());

    if(dist_ == "uniform"){
        float low = param[1], high= param[2];
        std::uniform_real_distribution<> dis_uni(low, high);
        return dis_uni(gen);
    }else if(dist_ == "exponential"){
        float lambda = param[3];
        std::exponential_distribution<> d_expo(lambda);
        return d_expo(gen);        
    }else if(dist_ == "c+exponential"){
        float constant_c = param[4];
        float lambda_c = param[5];
        std::exponential_distribution<> cexpo(lambda_c);
        return constant_c+cexpo(gen);
    }else if(dist_ == "deterministic"){
        float constant_d = param[6];
        return constant_d;
    }
    else return -1;
}

#endif
