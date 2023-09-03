#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <cstdint>

#define nb_proc 12
#define base_freq 1190


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

    while (1){
        freq = 0;
        cpufile.open("/proc/cpuinfo");
        if (cpufile.is_open()){
            while (getline(cpufile, line)){
                if (line.find("MHz") != std::string::npos){
                    freq += get_number(line);
                }
            }
            cpufile.close();

            freq = freq/nb_proc- base_freq;
        }
        
        std::cout << freq << " MHz"<< std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(750));
    }
}