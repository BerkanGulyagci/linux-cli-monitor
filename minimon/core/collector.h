#ifndef COLLECTOR_H
#define COLLECTOR_H
#include <atomic>

void getTcpConnections();
void generateCombinedReport();
void getCoreUtilization();
void listMountedDisks();
void showDiskIOStats();
void monitorResources();
extern std::atomic<bool> monitoring;

void getMemoryUsage();
void getTopProcesses(int count);
void getDiskUsage();
void getNetworkActivity();
void getProcessStates();
struct CpuInfo {
    float total;
    float user;
    float system;
    float idle;
};

CpuInfo getCpuUsage();


#endif

