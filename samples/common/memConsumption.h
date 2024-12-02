#pragma once
#include <cuda_runtime.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <thread>
#include <iostream>

#include <fstream>
#include <sstream>
#include <memory>
#include <string>
#define ABS(a, b) ((a) > (b)) ? ((a) - (b)):((b) - (a))

class memoryConsumption {
    public:
	    memoryConsumption() {}
	    void printMemInfo(std::string info, bool showDiff);
    private:
	    void fetchNum(std::string s, std::string line);
	    void fetchCpuMemUsed();

	    float memAvailLast_ = 0;
	    float cpuMemUsedLast_ = 0;
	    float memAvailCurr_ = 0;
	    float cpuMemUsedCurr_ = 0;

	    int totalMemAvailUsed_ = 0;
	    int totalMemNvMapUsed_ = 0;
	    int totalMemCpuUsed_ = 0;
};
