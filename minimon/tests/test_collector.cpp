#include "../core/collector.h"
#include <iostream>

int main() {
    std::cout << "=== Collector Test ===" << std::endl;

    float cpu = getCpuUsage();
    std::cout << "CPU Kullanımı: %" << cpu << std::endl;

    std::cout << "\nBellek Kullanımı:" << std::endl;
    getMemoryUsage();

    std::cout << "\nTop 3 Süreç (Test):" << std::endl;
    getTopProcesses(3);

    return 0;
}

