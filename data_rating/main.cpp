#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <cstdint>
#include <sys/stat.h>

#define smooth_factor 5

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
        tmpfloat = rate / 1024.0;
        result = (std::to_string(tmpfloat)).substr(0,4) + " Ko/s";
    }
    else if (rate < 1073741824)
    {
        tmpfloat = rate / 1048576.0;
        result = (std::to_string(tmpfloat)).substr(0,4) + " Mo/s";
    }
    else
    {
        tmpfloat = rate / 1073741824.0;
        result = (std::to_string(tmpfloat)).substr(0,4) + " Go/s";
    }

    if (result[3] == '.'){
        result = " " + result;
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

std::string getCmdOutput(const std::string& mStr)
{
    std::string result, file;
    FILE* pipe{popen(mStr.c_str(), "r")};
    char buffer[256];

    while(fgets(buffer, sizeof(buffer), pipe) != NULL)
    {
        file = buffer;
        result += file.substr(0, file.size() - 1);
    }

    pclose(pipe);
    return result;
}

int main(int argc, char** argv){
    std::string ConnectedInterface = "No";
    std::string filepath;
    std::string tmp;
    int nocon_count = 3;
    int count = smooth_factor;
    uint32_t rx = 0;
    uint32_t rx_rate = 0;
    int drx = 0;
    uint32_t current_rx = 0;
    uint32_t tx = 0;
    uint32_t tx_rate = 0;
    uint32_t current_tx = 0;
    int dtx = 0;
    std::ifstream ratefile;

    struct stat buffer;
    std::string access = "/tmp/data_rate_clicked";

    while (1){
        if (nocon_count >= 3){
            ConnectedInterface = getConnectedInterfaces();
            filepath = "/sys/class/net/"+ConnectedInterface+"/statistics/";
            nocon_count = 0;
        }
        if (count == 0){
            // Verify if is clicked
            if (stat(access.c_str(), &buffer) == 0){
                if (ConnectedInterface == "eth0"){
                    std::cout << "Ethernet" << std::endl;
                }
                else if (ConnectedInterface == "wlo1"){
                    // Get SSID name
                    std::cout << getCmdOutput("iwgetid -r") << std::endl;
                }
                else if (ConnectedInterface == "enp9s0f3u2"){
                    std::cout << "USB" << std::endl;
                }
                else{
                    std::cout << "No interface found" << std::endl;
                }
                std::remove(access.c_str());
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }

            // Get Data rate
            count = smooth_factor;
            rx_rate = 0;
            tx_rate = 0;
            if (ConnectedInterface != "No"){
                // Get RX rate
                ratefile.open(filepath+"rx_bytes");
                getline(ratefile,tmp);
                ratefile.close();
                rx_rate = rx;
                rx = std::stol(tmp);
                rx_rate = (rx - rx_rate);
                drx = rx_rate - current_rx;

                // Get TX rate
                ratefile.open(filepath+"tx_bytes");
                getline(ratefile,tmp);
                ratefile.close();
                tx_rate = tx;
                tx = std::stol(tmp);
                tx_rate = (tx - tx_rate);
                dtx = tx_rate - current_tx;
            }
            if (rx_rate == 0 || tx_rate == 0){
                nocon_count ++;
            }
        }
        else{
            count --;
            current_rx += drx/smooth_factor;
            current_tx += dtx/smooth_factor;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000/smooth_factor));
        std::cout << "⬆️ " << format_rate(current_tx, 1) << " | " << format_rate(current_rx, 0) << " ⬇️" << std::endl;
    }
}
