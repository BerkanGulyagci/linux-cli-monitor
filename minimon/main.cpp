#include "ui/cli.h"

#include "core/reporter.h"
#include <fstream>
#include <iomanip>
#include <thread>
#include <atomic>
#include <csignal>
#include <string>
#include <iostream>
#include <string>

#include "core/collector.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        // Hiç argüman verilmediyse → CLI arayüzünü başlat
        mainMenu();
        return 0;
    }

    string command = argv[1];

    if (command == "cpu") {

        CpuInfo cpu = getCpuUsage();

cout << fixed << setprecision(1)
     << "CPU Usage: %" << cpu.total
     << " (Current User: %" << cpu.user
     << ", System: %" << cpu.system
     << ", Idle: %" << cpu.idle << ")" << endl;

         saveCpuUsageToJson(cpu.total);


    }

 else if (command == "mem") {
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

    saveMemUsageToCsv(total, used, swapUsed, swapSize); // CSV kaydı


    }


 else if (command == "top") {
        int count = 5; // varsayılan
    if (argc >= 3) {
        try {
            count = std::stoi(argv[2]);
        } catch (...) {
            std::cerr << "Geçersiz sayı girdin, varsayılan 5 gösterilecek." << std::endl;
        }
    }
    getTopProcesses(count);



    }

else if (command == "disk") {
    getDiskUsage();
}
else if (command == "net") {
    getNetworkActivity();
}
else if (command == "states") {
    getProcessStates();
}

else if (command == "conn") {
    getTcpConnections();
}

else if (command == "report") {
    generateCombinedReport();
saveFullSystemReport();
}
else if (command == "analyze") {
    analyzeReportCsv();
}

else if (command == "help") {
    cout << "=== MiniMon Komut Listesi ===" << endl;
    cout << "cpu       \t→ CPU kullanım bilgisini gösterir" << endl;
    cout << "mem       \t→ RAM ve Swap kullanımını gösterir" << endl;
    cout << "top [n]   \t→ En çok CPU kullanan n süreci listeler (varsayılan: 5)" << endl;
    cout << "disk      \t→ Disk kullanım bilgisini gösterir" << endl;
    cout << "net       \t→ Ağ arayüzü veri trafiğini gösterir" << endl;
    cout << "states    \t→ Süreç durum sayılarını gösterir (Running/Sleeping/Zombie)" << endl;
    cout << "conn      \t→ Açık TCP bağlantılarını listeler" << endl;
    cout << "report    \t→ CPU+RAM+Swap verilerini report.csv dosyasına kaydeder" << endl;
    cout << "analyze   \t→ report.csv'den CPU kullanımını analiz eder" << endl;
    cout << "cores     \t→ Her çekirdek için CPU kullanımını gösterir" << endl; 
    cout << "help      \t→ Bu yardım menüsünü gösterir" << endl;
    cout << "disks      \t→ Bağlı diskleri ve boş alanı gösterir" << endl;
    cout << "io         \t→ Disk I/O işlemlerini gösterir (okuma/yazma sayısı)" << endl;
    cout << "monitor    \t→ Gerçek zamanlı kaynak kullanım izleme (multithreaded). Çıkmak için Ctrl+C 'ye basınız " << endl;
    cout << "Genel menüye ulaşmak için sadece /.minimon yazınız." <<endl;

}
else if (command == "cores") {
    getCoreUtilization();
}
else if (command == "disks") {
    listMountedDisks();
}
else if (command == "io") {
    showDiskIOStats();
}
else if (command == "monitor") {
    thread t(monitorResources);
    cout << "Gerçek zamanlı izleme başladı. Çıkmak için Ctrl + C\n";

    signal(SIGINT, [](int) {
        monitoring = false;
    });

    t.join();
}


 else {
        cout << "Geçersiz Komut.   Kullanım: ./minimon [cpu|mem|top|disk|net|states|conn|report|analyze]" << endl;
        return 1;
    }

    return 0;
}


