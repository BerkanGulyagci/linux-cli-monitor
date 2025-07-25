#include <ncurses.h>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include "../core/collector.h"
#include "../core/reporter.h"
#include <ncurses.h>
#include <iomanip>
#include <iostream>

void drawMenu(const std::vector<std::string>& items, int selected) {
    clear();
    int startx = (COLS - 40) / 2;
    int starty = 2;

    mvprintw(starty - 2, startx, "========== MiniMon ===========");

    for (size_t i = 0; i < items.size(); ++i) {
        if ((int)i == selected) {
            attron(A_REVERSE);
            mvprintw(starty + i, startx, "  %s", items[i].c_str());
            attroff(A_REVERSE);
        } else {
            mvprintw(starty + i, startx, "  %s", items[i].c_str());
        }
    }

    mvprintw(starty + items.size() + 2, startx, " Asagi ve yukari  ok tuslariyla kontrol ediniz. | Enter ile seç");
    refresh();
}
void mainMenu() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);

    std::vector<std::string> menuItems = {
        "CPU Kullanımı",
        "RAM Durumu",
        "Top Operations",
        "Disk Kullanimi",
        "Disk I/O",
        "Core Kullanimi",
        "Ag Trafigi",
        "TCP Baglantilari",
        "Gercek Zamanli Izleme",
        "Raporla - JSON ve CSV",
        "Rapor Analizi",
        "Exit"
    };

    int selected = 0;
    int ch;

    while (true) {
        drawMenu(menuItems, selected);
        ch = getch();
        if (ch == KEY_UP) {
            selected = (selected == 0) ? menuItems.size() - 1 : selected - 1;
        } else if (ch == KEY_DOWN) {
            selected = (selected == (int)menuItems.size() - 1) ? 0 : selected + 1;
        } else if (ch == '\n') {
            clear();
            endwin();
            switch (selected) {
                case 0: {
    CpuInfo cpu = getCpuUsage();
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "CPU Usage: %" << cpu.total
              << " (Current User: %" << cpu.user
              << ", System: %" << cpu.system
              << ", Idle: %" << cpu.idle << ")" << std::endl;
    break;
}
                case 1: getDiskUsage(); break;
                case 2: getTopProcesses(5); break;
                case 3: listMountedDisks(); break;
                case 4: showDiskIOStats(); break;
                case 5: getCoreUtilization(); break;
                case 6: getNetworkActivity(); break;
                case 7: getTcpConnections(); break;
                case 8: {
                    monitoring = true;
                    std::thread t(monitorResources);
                    std::this_thread::sleep_for(std::chrono::seconds(5));
                    monitoring = false;
                    t.join();
                    break;
                }
               
case 9:
    saveFullSystemReport();
      generateCombinedReport();
    break;

   


                case 10: analyzeReportCsv(); break;
                case 11: return;
            }

            initscr(); noecho(); cbreak(); keypad(stdscr, TRUE);
        }
    }

    endwin();
}

