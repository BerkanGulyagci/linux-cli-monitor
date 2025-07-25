#include "reporter.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iomanip>
#include <sys/statvfs.h>
#include "collector.h"



using namespace std;

void saveCpuUsageToJson(float usage) {
    ofstream file("report.json");
    file << "{\n";
    file << "  \"CPU_Usage\": " << fixed << setprecision(2) << usage << "\n";
    file << "}\n";
    file.close();
}

void saveMemUsageToCsv(float total, float used, float swapUsed, float swapTotal) {
    ofstream file("report.csv");
    file << "ToplamRAM(MB) , KullanilanRAM(MB) , SwapKullanilan(MB) , SwapToplam(MB)\n";
    file << fixed << setprecision(2)
         << total << "," << used << "," << swapUsed << "," << swapTotal << "\n";
    file.close();
}

void analyzeReportCsv() {
    std::ifstream file("report.csv");
    std::string line;

    // Başlık satırını atla
    std::getline(file, line);

    // Veri satırını oku
    if (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string cpuTotalStr;
        std::getline(ss, cpuTotalStr, ','); // ilk sütun: CPU_Total

        try {
            float cpuTotal = std::stof(cpuTotalStr);
            std::cout << "[Analyzer] CPU Total Usage: " << std::fixed << std::setprecision(2) << cpuTotal << "%" << std::endl;
        } catch (...) {
            std::cerr << "Veri çözümlenemedi!" << std::endl;
        }
    } else {
        std::cerr << "Veri satırı bulunamadı!" << std::endl;
    }

    file.close();
}

#include <nlohmann/json.hpp>
using json = nlohmann::json;

void saveFullReportToJson(float cpu_total, float cpu_user, float cpu_system, float cpu_idle,
                          float ram_total, float ram_used,
                          float swap_used, float swap_total,
                          float disk_usage,
                          float net_rx, float net_tx,
                          int active_connections) {
    json j;
    j["cpu_total"] = cpu_total;
    j["cpu_user"] = cpu_user;
    j["cpu_system"] = cpu_system;
    j["cpu_idle"] = cpu_idle;
    j["ram_total"] = ram_total;
    j["ram_used"] = ram_used;
    j["swap_used"] = swap_used;
    j["swap_total"] = swap_total;
    j["disk_usage"] = disk_usage;
    j["net_rx_mb"] = net_rx;
    j["net_tx_mb"] = net_tx;
    j["active_connections"] = active_connections;

    std::ofstream file("report.json");
    file << std::setw(4) << j << std::endl;
    file.close();
}

void saveFullSystemReport() {
    CpuInfo cpu = getCpuUsage();

    // RAM
    long memTotal = 0, memAvailable = 0, swapTotal = 0, swapFree = 0;
    std::ifstream memfile("/proc/meminfo");
    std::string line;
    while (std::getline(memfile, line)) {
        if (line.find("MemTotal:") == 0)
            sscanf(line.c_str(), "MemTotal: %ld", &memTotal);
        else if (line.find("MemAvailable:") == 0)
            sscanf(line.c_str(), "MemAvailable: %ld", &memAvailable);
        else if (line.find("SwapTotal:") == 0)
            sscanf(line.c_str(), "SwapTotal: %ld", &swapTotal);
        else if (line.find("SwapFree:") == 0)
            sscanf(line.c_str(), "SwapFree: %ld", &swapFree);
    }
    memfile.close();
    float ramUsed = (memTotal - memAvailable) / 1024.0f;
    float ramTotal = memTotal / 1024.0f;
    float swapUsed = (swapTotal - swapFree) / 1024.0f;
    float swapTotalMB = swapTotal / 1024.0f;

    // Disk
    struct statvfs stat;
    float diskUsage = 0;
    if (statvfs("/", &stat) == 0) {
        float total = (stat.f_blocks * stat.f_frsize) / (1024.0f * 1024.0f);
        float available = (stat.f_bavail * stat.f_frsize) / (1024.0f * 1024.0f);
        float used = total - available;
        diskUsage = (used / total) * 100.0f;
    }

    // Ağ
    std::ifstream net("/proc/net/dev");
    std::string netLine;
    getline(net, netLine); // başlık
    getline(net, netLine); // başlık
    long rx = 0, tx = 0;
    while (getline(net, netLine)) {
        size_t colon = netLine.find(":");
        if (colon != std::string::npos) {
            long ifaceRx = 0, ifaceTx = 0;
            sscanf(netLine.c_str() + colon + 1, "%ld %*s %*s %*s %*s %*s %*s %*s %ld", &ifaceRx, &ifaceTx);
            rx += ifaceRx;
            tx += ifaceTx;
        }
    }
    float rxMB = rx / 1024.0f / 1024.0f;
    float txMB = tx / 1024.0f / 1024.0f;

    // TCP bağlantı sayısı
    FILE* pipe = popen("netstat -tn | tail -n +3 | wc -l", "r");
    int activeConnections = 0;
    if (pipe) {
        char buffer[32];
        fgets(buffer, sizeof(buffer), pipe);
        activeConnections = atoi(buffer);
        pclose(pipe);
    }

    // JSON yaz
    std::ofstream file("report.json");
    file << std::fixed << std::setprecision(2);
    file << "{\n";
    file << "  \"cpu_total\": " << cpu.total << ",\n";
    file << "  \"cpu_user\": " << cpu.user << ",\n";
    file << "  \"cpu_system\": " << cpu.system << ",\n";
    file << "  \"cpu_idle\": " << cpu.idle << ",\n";
    file << "  \"ram_total\": " << ramTotal << ",\n";
    file << "  \"ram_used\": " << ramUsed << ",\n";
    file << "  \"swap_total\": " << swapTotalMB << ",\n";
    file << "  \"swap_used\": " << swapUsed << ",\n";
    file << "  \"disk_usage\": " << diskUsage << ",\n";
    file << "  \"net_rx_mb\": " << rxMB << ",\n";
    file << "  \"net_tx_mb\": " << txMB << ",\n";
    file << "  \"active_connections\": " << activeConnections << "\n";
    file << "}\n";
    file.close();

    std::cout << "JSON sistem raporu oluşturuldu: report.json\n";
}

