#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <cstdint>

#define nb_proc 12
#define base_freq 0


// Find connected network interfaces

int get_number(std::string line){
    float result = std::stof(line.substr(11));
    return static_cast<int>(result);
}

int main(int argc, char** argv){
    std::ifstream cpufile;
    std::string line;
    int freq;
    int cpu_count;
    int count = 0;
    int current_freq = 0;
    int step;

    while (1){
        if (count == 0){
            freq = 0;
            cpufile.open("/proc/cpuinfo");
            if (cpufile.is_open()){
                while (getline(cpufile, line)){
                    if (line.find("MHz") != std::string::npos){
                        freq += get_number(line);
                    }
                }
                cpufile.close();

                freq = freq/nb_proc - base_freq;
                step = (freq - current_freq)/5;
            }
        }
        else if (count == 5)
        {
            count = 0;
        }
        else{count++;}
        current_freq += step;
        
        std::cout << current_freq << " MHz"<< std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
}
