#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <mutex>

class Logger {
private:
    std::ofstream logfile;
    bool log_to_file;
    std::mutex mtx; // for thread safety

    Logger() {
        log_to_file = false;
    }

    ~Logger() {
        if (logfile.is_open()) {
            logfile.close();
        }
    }

    std::string Timestamp() const {
        std::ostringstream oss;
        std::time_t now = std::time(nullptr);
        std::tm tm_now;

        localtime_s(&tm_now, &now);
        oss << std::put_time(&tm_now, "[%Y-%m-%d %H:%M:%S] ");
        return oss.str();
    }

    void Log(const std::string& level, const std::string& msg) {
        std::lock_guard<std::mutex> lock(mtx);
        std::string output = Timestamp() + "[" + level + "] " + msg;
        if (log_to_file) {
            logfile << output << std::endl;
        }
    
        std::cout << output << std::endl;
    }

    Logger(const Logger&);
    Logger& operator=(const Logger&);

public:
    static Logger& GetInstance() {
        static Logger instance;
        return instance;
    }

    // initialize logger to also write to file
    static void Init(const std::string& filename) {
        Logger& instance = GetInstance();
        instance.logfile.open(filename, std::ios::app);
        instance.log_to_file = instance.logfile.is_open();
    }

    void Info(const std::string& msg) { Log("INFO", msg); }
    void Error(const std::string& msg) { Log("ERROR", msg); }
    void Debug(const std::string& msg) { Log("DEBUG", msg); }
};
