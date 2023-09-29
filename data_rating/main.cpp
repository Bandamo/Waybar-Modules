#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <cstdint>


// Find connected network interfaces

std::string getConnectedInterfaces()
{
    std::string result = "No";
    std::string line;
    std::ifstream file("/proc/net/route");
    if (file.is_open())
    {
        while (getline(file, line))
        {
            if (line.find("eth0") != std::string::npos)
            {
                result = "eth0";
                break;
            }
            else if (line.find("wlo1") != std::string::npos)
            {
                result = "wlo1";
                break;
            }
            else if (line.find("enp9s0f3u2") != std::string::npos)
            {
                result = "enp9s0f3u2";
                break;
            }
        }
        file.close();
    }
    return result;
}

std::string format_rate(uint32_t rate, bool in){
    std::string result;
    float tmpfloat = 0;
    int tmpint = 0;
    if (rate < 1024){
        result = std::to_string(rate)+ " o/s";
    }
    else if (rate < 1048576)
    {
        tmpfloat = rate / 1024.;
        result = (std::to_string(tmpfloat)).substr(0,4) + " Ko/s";
    }
    else if (rate < 1073741824)
    {
        tmpfloat = rate / 1048576.;
        result = (std::to_string(tmpfloat)).substr(0,4) + " Mo/s";
    }
    else
    {
        tmpfloat = rate / 1073741824.;
        result = (std::to_string(tmpfloat)).substr(0,4) + " Go/s";
    }

    if (result.size() < 8){
        tmpint = 10 - result.size();
        if(in){
            for (int i = 0; i < tmpint; i++){
                result = " " + result;
            }
        }
        else{
            for (int i = 0; i < tmpint; i++){
                result = result + " ";
            }
        }
    }
    
    return result;
    
}

int main(int argc, char** argv){
    std::string ConnectedInterface = "No";
    std::string filepath;
    std::string tmp;
    int count = 0;
    uint32_t rx = 0;
    uint32_t rx_rate = 0;
    uint32_t tx = 0;
    uint32_t tx_rate = 0;
    std::ifstream ratefile;

    while (1){
        if (count >= 3){
            ConnectedInterface = getConnectedInterfaces();
            filepath = "/sys/class/net/"+ConnectedInterface+"/statistics/";
            count = 0;
        }

        if (ConnectedInterface != "No"){
            // Get RX rate
            ratefile.open(filepath+"rx_bytes");
            getline(ratefile,tmp);
            ratefile.close();
            rx_rate = rx;
            rx = std::stol(tmp);
            rx_rate = (rx - rx_rate) * 1.5;

            // Get TX rate
            ratefile.open(filepath+"tx_bytes");
            getline(ratefile,tmp);
            ratefile.close();
            tx_rate = tx;
            tx = std::stol(tmp);
            tx_rate = (tx - tx_rate) * 1.5;
        }
        if (rx_rate == 0 || tx_rate == 0){
            count ++;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(666));
        std::cout << "⬆️ " << format_rate(tx_rate, 1) << " | " << format_rate(rx_rate, 0) << " ⬇️" << std::endl;
    }
}
