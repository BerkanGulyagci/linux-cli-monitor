#include <vector>
#include <algorithm>
#include <string>

struct ProcessInfo {
    int pid;
    std::string name;
    float cpu;
};

bool compareByCpu(const ProcessInfo& a, const ProcessInfo& b) {
    return a.cpu > b.cpu;
}

void sortProcessesByCpu(std::vector<ProcessInfo>& processes) {
    std::sort(processes.begin(), processes.end(), compareByCpu);
}

