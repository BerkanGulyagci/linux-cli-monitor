#include <iomanip>

#include "collector.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>

using namespace std;

CpuInfo getCpuUsage() {
    string line;
    long user1, nice1, system1, idle1, iowait1, irq1, softirq1;
    long user2, nice2, system2, idle2, iowait2, irq2, softirq2;

    ifstream file1("/proc/stat");
    getline(file1, line);
    istringstream ss1(line);
    string cpuLabel;
    ss1 >> cpuLabel >> user1 >> nice1 >> system1 >> idle1 >> iowait1 >> irq1 >> softirq1;
    file1.close();

    usleep(100000); // 100ms bekleme

    ifstream file2("/proc/stat");
    getline(file2, line);
    istringstream ss2(line);
    ss2 >> cpuLabel >> user2 >> nice2 >> system2 >> idle2 >> iowait2 >> irq2 >> softirq2;
    file2.close();

    long idleDelta = (idle2 + iowait2) - (idle1 + iowait1);
    long total1 = user1 + nice1 + system1 + idle1 + iowait1 + irq1 + softirq1;
    long total2 = user2 + nice2 + system2 + idle2 + iowait2 + irq2 + softirq2;
    long totalDelta = total2 - total1;

    if (totalDelta == 0) totalDelta = 1;

    CpuInfo info;
    info.total = 100.0f * (1.0f - (float)idleDelta / totalDelta);
    info.user = 100.0f * (user2 + nice2 - user1 - nice1) / totalDelta;
    info.system = 100.0f * (system2 + irq2 + softirq2 - system1 - irq1 - softirq1) / totalDelta;
    info.idle = 100.0f * (idle2 + iowait2 - idle1 - iowait1) / totalDelta;

    return info;
}


void getMemoryUsage() {
    ifstream file("/proc/meminfo");
    string line;
    long memTotal = 0, memAvailable = 0, swapTotal = 0, swapFree = 0;

    while (getline(file, line)) {
        if (line.find("MemTotal:") == 0)
            sscanf(line.c_str(), "MemTotal: %ld", &memTotal);
        else if (line.find("MemAvailable:") == 0)
            sscanf(line.c_str(), "MemAvailable: %ld", &memAvailable);
        else if (line.find("SwapTotal:") == 0)
            sscanf(line.c_str(), "SwapTotal: %ld", &swapTotal);
        else if (line.find("SwapFree:") == 0)
            sscanf(line.c_str(), "SwapFree: %ld", &swapFree);
    }

    float used = (memTotal - memAvailable) / 1024.0;
    float total = memTotal / 1024.0;
    float swapUsed = (swapTotal - swapFree) / 1024.0;
    float swapSize = swapTotal / 1024.0;

    cout << "Toplam RAM: " << total << " MB" << endl;
    cout << "Kullanılan RAM: " << used << " MB" << endl;
    cout << "Swap: " << swapUsed << " MB / " << swapSize << " MB" << endl;
}

#include <cstdlib>
#include <vector>
#include <algorithm>

void getTopProcesses(int count) {
    string cmd = "ps -eo pid,comm,%cpu --sort=-%cpu | head -n " + to_string(count + 1);
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        cerr << "ps komutu çalıştırılamadı!" << endl;
        return;
    }

    char buffer[256];
    bool firstLine = true;
    int i = 1;

    cout << "En çok CPU kullanan " << count << " süreç:" << endl;

    while (fgets(buffer, sizeof(buffer), pipe)) {
        if (firstLine) { firstLine = false; continue; }

        int pid;
        char name[64];
        float cpu;

        if (sscanf(buffer, "%d %s %f", &pid, name, &cpu) == 3) {
            printf("%d. %s (PID: %d) - %%%.1f\n", i++, name, pid, cpu);
        }

        if (i > count) break;
    }

    pclose(pipe);
}

#include <sys/statvfs.h>

void getDiskUsage() {
    struct statvfs stat;

    if (statvfs("/", &stat) != 0) {
        perror("statvfs");
        return;
    }

    unsigned long total = (stat.f_blocks * stat.f_frsize) / (1024 * 1024);
    unsigned long available = (stat.f_bavail * stat.f_frsize) / (1024 * 1024);
    unsigned long used = total - available;

    std::cout << "=== Disk Kullanımı ===" << std::endl;
    std::cout << "Toplam: " << total << " MB\nKullanılan: " << used << " MB\nBoş: " << available << " MB" << std::endl;
}

void getNetworkActivity() {
    ifstream file("/proc/net/dev");
    string line;

    getline(file, line); // Başlık satırları
    getline(file, line);

    std::cout << "=== Ağ Arayüzü Aktivitesi ===" << std::endl;

    while (getline(file, line)) {
        string iface;
        long rx = 0, tx = 0;

        size_t colon = line.find(":");
        if (colon != string::npos) {
            iface = line.substr(0, colon);
            sscanf(line.c_str() + colon + 1, "%ld %*s %*s %*s %*s %*s %*s %*s %ld", &rx, &tx);
            std::cout << iface << " - Alınan: " << rx / 1024 << " KB, Gönderilen: " << tx / 1024 << " KB" << std::endl;
        }
    }
}
void getProcessStates() {
    std::cout << "=== Süreç Durumları (Running/Sleeping/Zombie vs) ===" << std::endl;
    system("ps -eo stat | cut -c1 | sort | uniq -c");
}



void getTcpConnections() {
    std::cout << "=== Açık TCP Bağlantıları ===" << std::endl;

    FILE* pipe = popen("netstat -tn | tail -n +3", "r"); // ilk 2 satırı atla
    if (!pipe) {
        std::cerr << "Bağlantı bilgileri alınamadı." << std::endl;
        return;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        std::string line(buffer);
        std::istringstream iss(line);
        std::string proto, recvq, sendq, local, remote, state;

        iss >> proto >> recvq >> sendq >> local >> remote >> state;

        std::cout << local << " → " << remote << " [" << state << "]" << std::endl;
    }

    pclose(pipe);
}


void generateCombinedReport() {
    CpuInfo cpu = getCpuUsage();

    long memTotal = 0, memAvailable = 0, swapTotal = 0, swapFree = 0;

    std::ifstream file("/proc/meminfo");
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("MemTotal:") == 0)
            sscanf(line.c_str(), "MemTotal: %ld", &memTotal);
        else if (line.find("MemAvailable:") == 0)
            sscanf(line.c_str(), "MemAvailable: %ld", &memAvailable);
        else if (line.find("SwapTotal:") == 0)
            sscanf(line.c_str(), "SwapTotal: %ld", &swapTotal);
        else if (line.find("SwapFree:") == 0)
            sscanf(line.c_str(), "SwapFree: %ld", &swapFree);
    }
    file.close();

    float memUsed = (memTotal - memAvailable) / 1024.0f;
    float swapUsed = (swapTotal - swapFree) / 1024.0f;

    std::ofstream report("report.csv");
    report << "CPU_Total,CPU_User,CPU_System,CPU_Idle,RAM_Total,RAM_Used,Swap_Used,Swap_Total\n";
    report << std::fixed << std::setprecision(2)
           << cpu.total << "," << cpu.user << "," << cpu.system << "," << cpu.idle << ","
           << memTotal / 1024.0f << "," << memUsed << "," << swapUsed << "," << swapTotal / 1024.0f << "\n";
    report.close();

    std::cout << "Rapor oluşturuldu: report.csv" << std::endl;
}

void getCoreUtilization() {
    std::ifstream file1("/proc/stat");
    std::vector<long> idle1, total1;
    std::string line;

    while (getline(file1, line)) {
        if (line.rfind("cpu", 0) == 0 && line != "cpu") {
            std::istringstream ss(line);
            std::string cpu;
            long user, nice, system, idle, iowait, irq, softirq;
            ss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq;
            idle1.push_back(idle + iowait);
            total1.push_back(user + nice + system + idle + iowait + irq + softirq);
        }
    }
    file1.close();

    usleep(100000); // 100ms bekle

    std::ifstream file2("/proc/stat");
    std::vector<long> idle2, total2;
    while (getline(file2, line)) {
        if (line.rfind("cpu", 0) == 0 && line != "cpu") {
            std::istringstream ss(line);
            std::string cpu;
            long user, nice, system, idle, iowait, irq, softirq;
            ss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq;
            idle2.push_back(idle + iowait);
            total2.push_back(user + nice + system + idle + iowait + irq + softirq);
        }
    }
    file2.close();

    std::cout << "=== CPU Core Kullanımı ===" << std::endl;
    for (size_t i = 0; i < idle1.size(); ++i) {
        long idleDelta = idle2[i] - idle1[i];
        long totalDelta = total2[i] - total1[i];
        float usage = 100.0 * (1.0 - (float)idleDelta / totalDelta);
        printf("Core %lu: %.1f%%\n", i, usage);
    }
}

void listMountedDisks() {
    std::ifstream mounts("/proc/mounts");
    std::string line;

    std::cout << "=== Bağlı Diskler ===" << std::endl;

    while (std::getline(mounts, line)) {
        std::istringstream ss(line);
        std::string device, mount, type;
        ss >> device >> mount >> type;

        if (mount.find("/snap") == 0 || type != "ext4" && type != "vfat" && type != "ntfs") {
            continue; // ilgisiz ya da sanal diskleri atla
        }

        struct statvfs stats;
        if (statvfs(mount.c_str(), &stats) == 0) {
            float total = (stats.f_blocks * stats.f_frsize) / (1024.0 * 1024.0 * 1024.0);
            float free = (stats.f_bfree * stats.f_frsize) / (1024.0 * 1024.0 * 1024.0);
            printf("Mount: %-15s - Toplam: %.1f GB, Boş: %.1f GB\n", mount.c_str(), total, free);
        }
    }
}

void showDiskIOStats() {
    std::ifstream file("/proc/diskstats");
    std::string line;

    std::cout << "=== Disk I/O İstatistikleri ===" << std::endl;

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string ignore, device;
        long reads, ignore2, ignore3, writes;

        ss >> ignore >> ignore >> device >> reads;
        for (int i = 0; i < 4; ++i) ss >> ignore2; // geçici atla
        ss >> writes;

        // sadece fiziksel diskler (loop, ram disk vb. hariç)
        if (device.find("loop") == 0 || device.find("ram") == 0) continue;

        printf("%s  → Okuma işlemi: %ld, Yazma işlemi: %ld\n", device.c_str(), reads, writes);
    }
}

#include <atomic>
#include <thread>

std::atomic<bool> monitoring(true);

void monitorResources() {
    while (monitoring) {
        CpuInfo cpu = getCpuUsage();

        long memTotal = 0, memAvailable = 0;
        std::ifstream file("/proc/meminfo");
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("MemTotal:") == 0)
                sscanf(line.c_str(), "MemTotal: %ld", &memTotal);
            else if (line.find("MemAvailable:") == 0)
                sscanf(line.c_str(), "MemAvailable: %ld", &memAvailable);
        }
        file.close();

        float memUsed = (memTotal - memAvailable) / 1024.0f;

        std::cout << "\033[2J\033[H"; // Ekranı temizle (ANSI)
        std::cout << "=== Gerçek Zamanlı İzleme ===" << std::endl;
        std::cout << std::fixed << std::setprecision(1);
        std::cout << "CPU Kullanımı: %" << cpu.total << std::endl;
        std::cout << "RAM Kullanımı: " << memUsed << " MB" << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

