#ifndef REPORTER_H
#define REPORTER_H
void saveFullSystemReport();

void saveCpuUsageToJson(float usage);
void saveMemUsageToCsv(float total, float used, float swapUsed, float swapTotal);
void analyzeReportCsv();
void saveFullReportToJson(float cpu_total, float cpu_user, float cpu_system, float cpu_idle,
                          float ram_total, float ram_used,
                          float swap_used, float swap_total,
                          float disk_usage,
                          float net_rx, float net_tx,
                          int active_connections);


#endif

