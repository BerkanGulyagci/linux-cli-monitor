#  System Resource Monitoring Tool - Operating Systems Term Project

This project was developed as part of the **SWE-208 Operating Systems** course to monitor Linux system resources using a command-line C++ application.  
The application gathers real-time data about CPU, memory, disk, network activity, and processes from system interfaces and files such as `/proc`.


---

##  Project Overview

This tool offers a modular and extensible architecture for tracking essential system metrics in real-time.  
It leverages the Linux `/proc` filesystem and standard system tools to present information interactively via terminal commands, with optional reporting to `.json` and `.csv` formats.

The project demonstrates:
- Direct access to OS-level statistics
- CLI interaction without GUI overhead
- Structured reporting and logging
- Clean and maintainable C++ codebase

---

##  Technologies & Resources

- **Language:** C++  
- **Platform:** Linux (tested on Ubuntu 22.04 via WSL)  
- **Data Sources:**  
  `/proc/stat`, `/proc/meminfo`, `/proc/net/dev`, `ps`, `statvfs()`  
- **Interface:** Terminal-based CLI  
- **Reporting:** JSON (`report.json`), CSV (`report.csv`)

---

##  Available Commands

| Command              | Description                                  |
|----------------------|----------------------------------------------|
| `./minimon cpu`      | Displays current CPU usage and saves report  |
| `./minimon mem`      | Displays memory and swap usage               |
| `./minimon top`      | Lists top 5 processes by CPU usage           |
| `./minimon disk`     | Shows disk usage information                 |
| `./minimon net`      | Displays network RX/TX per interface         |
| `./minimon states`   | Displays process state distribution          |

---

## Testing

Independent testing of the core logic is available through:

```bash
g++ tests/test_collector.cpp core/collector.cpp -o test
./test
```

This checks core metrics such as CPU load, memory use, and process listing.

---

##  Project Structure

```
minimon/
├── main.cpp
├── README.md
├── core/
│   ├── collector.cpp / .h
│   ├── reporter.cpp / .h
│   ├── analyzer.cpp
├── ui/
│   └── cli.cpp
├── utils/
│   └── config.cpp
├── tests/
│   └── test_collector.cpp
├── report.json
└── report.csv
```

---

## Conclusion

This system monitoring tool is built to meet the requirements of the term project specification in full.  
It covers core functionality, follows a modular approach, and offers report generation for further analysis.  
All features were developed and tested in a Linux environment with full CLI support.

