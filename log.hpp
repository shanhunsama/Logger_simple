#pragma once

#include <string>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <mutex>

// 日志级别枚举
enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    // 构造函数，初始化日志级别和日志文件名
    Logger(const std::string& logFileName, LogLevel level = LogLevel::INFO);
    // 析构函数，关闭日志文件
    ~Logger();

    // 设置日志级别
    void setLogLevel(LogLevel level);

    // 日志输出函数，使用模板支持不同参数类型
    template <typename... Args>
    void log(LogLevel level, const char* file, int line, Args&&... args);

    // 以下是针对不同日志级别的具体输出函数
    template <typename... Args>
    void debug(const char* file, int line, Args&&... args);

    template <typename... Args>
    void info(const char* file, int line, Args&&... args);

    template <typename... Args>
    void warning(const char* file, int line, Args&&... args);

    template <typename... Args>
    void error(const char* file, int line, Args&&... args);

private:
    LogLevel logLevel;
    std::string logFileName;
    std::ofstream logFile;
    std::mutex mutex;

    // 日志输出函数的内部实现，使用模板支持不同参数类型
    template <typename... Args>
    void log(const std::string& message, Args&&... args);

    // 辅助函数，将多个参数写入输出流
    template <typename T, typename... Args>
    void logMessage(std::ostream& stream, T&& value, Args&&... args);

    // 辅助函数，递归结束条件
    void logMessage(std::ostream&);

    // 将日志级别转换为字符串
    std::string logLevelToString(LogLevel level);
};



#include <iostream>
Logger::Logger(const std::string& logFileName, LogLevel level)
    : logLevel(level), logFileName(logFileName) {
    logFile.open(logFileName, std::ios_base::app);
    if (!logFile.is_open()) {
        std::cerr << "Error opening log file." << std::endl;
        return;
    }
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void Logger::setLogLevel(LogLevel level) {
    logLevel = level;
}

template <typename... Args>
void Logger::log(LogLevel level, const char* file, int line, Args&&... args) {
    if (level >= logLevel) {
        std::time_t now = std::time(nullptr);
        std::tm tm = *std::localtime(&now);

        std::stringstream ss;
        // 构建日志消息，包括时间、文件路径、行号
        ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " [" << logLevelToString(level) << "] " << file << ":" << line << " - ";
        // 使用辅助函数输出消息内容
        log(ss.str(), std::forward<Args>(args)...);
    }
}

// 以下为不同日志级别的具体输出函数的实现
// 模板参数 Args... 表示可变数量的参数，用于构造日志消息内容
template <typename... Args>
void Logger::debug(const char* file, int line, Args&&... args) {
    log(LogLevel::DEBUG, file, line, std::forward<Args>(args)...);
}

template <typename... Args>
void Logger::info(const char* file, int line, Args&&... args) {
    log(LogLevel::INFO, file, line, std::forward<Args>(args)...);
}

template <typename... Args>
void Logger::warning(const char* file, int line, Args&&... args) {
    log(LogLevel::WARNING, file, line, std::forward<Args>(args)...);
}

template <typename... Args>
void Logger::error(const char* file, int line, Args&&... args) {
    log(LogLevel::ERROR, file, line, std::forward<Args>(args)...);
}

// 辅助函数，实现消息内容的构建和输出
template <typename... Args>
void Logger::log(const std::string& message, Args&&... args) {
    std::lock_guard<std::mutex> lock(mutex);
    if (logFile.is_open()) {
        logFile << message;
        // 调用 logMessage 辅助函数输出多个参数
        logMessage(logFile, std::forward<Args>(args)...);
        logFile << std::endl;
    }
}

// 辅助函数，递归地将参数写入输出流
template <typename T, typename... Args>
void Logger::logMessage(std::ostream& stream, T&& value, Args&&... args) {
    stream << std::forward<T>(value);
    logMessage(stream, std::forward<Args>(args)...);
}

void Logger::logMessage(std::ostream&)
{
    
}
// 将日志级别转换为字符串
std::string Logger::logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::ERROR:
            return "ERROR";
    }
    return "";
}





#endif // __LOGGER_H__


// int main() {
//     Logger logger("my_log.txt", LogLevel::INFO);

//     logger.debug(__FILE__, __LINE__, "Debug message."); // 不会被记录，因为日志级别设置为 INFO
//     logger.info(__FILE__, __LINE__, "Info message.");
//     logger.warning(__FILE__, __LINE__, "Warning message.");
//     logger.error(__FILE__, __LINE__, "Error message.");

//     return 0;
// }