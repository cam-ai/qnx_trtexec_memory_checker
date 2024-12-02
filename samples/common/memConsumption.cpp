#include "memConsumption.h"
#include <malloc.h>

void memoryConsumption::fetchNum(std::string s, std::string line) {
    float num;
    printf("%s\n", line.c_str());
    size_t pos = line.find("(");
    std::string str = line.substr(pos + 1, 6);
    num = std::stof(str);
    if (line.rfind("GB", -3) != std::string::npos)
    {
        memAvailCurr_ = num * 1024;
    } else
        memAvailCurr_ = num;
}

void memoryConsumption::fetchCpuMemUsed() {
    struct mallinfo hostMemInfo;
    hostMemInfo = mallinfo();
    
    cpuMemUsedCurr_ = (hostMemInfo.uordblks + hostMemInfo.hblkhd + hostMemInfo.usmblks)  / 1024 / 1024;
	printf("CpuMemUsed is %d MB\n", (int)(cpuMemUsedCurr_));
}

void memoryConsumption::printMemInfo(std::string s, bool notEnd) {
    sleep(5);
    printf("----------------%s---------------\n", s.c_str());

    std::ifstream file("/proc/vm/stats");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.rfind("pages_free", 0) != std::string::npos)
                fetchNum("pages_free", line);
        }
        file.close();
    }

    fetchCpuMemUsed();

    if (memAvailLast_ != 0) {
        int memAvailUsed = ABS((int)memAvailLast_, (int)memAvailCurr_);
        int cpuMemUsed = ABS((int)cpuMemUsedLast_, (int)cpuMemUsedCurr_);
        int memNvMapUsed = memAvailUsed - cpuMemUsed;
        if (memNvMapUsed < 0)  memNvMapUsed = 0;
        if (memAvailUsed == 0)
        {
            cpuMemUsed = 0;
            memNvMapUsed = 0;
        }
        printf("Consumed mem size during [%s]: %d MB\tCPU Mem: %d MB\tGPU Mem: %d MB\t \n", s.c_str(),
            memAvailUsed, cpuMemUsed, memNvMapUsed);
        totalMemAvailUsed_ += memAvailUsed;
        totalMemCpuUsed_ += cpuMemUsed;
        totalMemNvMapUsed_ += memNvMapUsed;
    }

    if (!notEnd) {
        printf("Consumed mem size in total: %d MB\tCPU Mem in total: %d MB\tGPU Mem in total: %d MB\t \n", totalMemAvailUsed_, totalMemCpuUsed_, totalMemNvMapUsed_);
    }
    memAvailLast_ = memAvailCurr_;
    cpuMemUsedLast_ = cpuMemUsedCurr_;
    printf("\n");
}

