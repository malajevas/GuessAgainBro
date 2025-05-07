#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>

class Logger {
private:
    std::ofstream logfile;
    bool log_to_file;

    std::string Timestamp() const {
        std::ostringstream oss;
        std::time_t now = std::time(nullptr);
        std::tm tm_now;

        localtime_s(&tm_now, &now); // thread-safe

        oss << std::put_time(&tm_now, "[%Y-%m-%d %H:%M:%S] ");
        return oss.str();
    }

public:
    Logger(const std::string& filename = "") {
        if (!filename.empty()) {
            logfile.open(filename, std::ios::app);
            log_to_file = logfile.is_open();
        } 
        else {
            log_to_file = false;
        }
    }

    ~Logger() {
        if (logfile.is_open()) {
            logfile.close();
        }
    }

    void Info(const std::string& msg) {
        Log("INFO", msg);
    }

    void Error(const std::string& msg) {
        Log("ERROR", msg);
    }

    void Debug(const std::string& msg) {
        Log("DEBUG", msg);
    }

private:
    void Log(const std::string& level, const std::string& msg) {
        std::string output = Timestamp() + "[" + level + "] " + msg;
        if (log_to_file) {
            logfile << output << std::endl;
        } 
        else {
            std::cout << output << std::endl;
        }
    }
};
