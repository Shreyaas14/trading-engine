//
//  logger.hpp
//  trading_engine_server
//
//  Created by Shreyaas Sureshbabu on 11/20/24.
//

#include <stdio.h>
#include <mutex>
#include <ctime>

#pragma once

//every method will be static
enum LogPriority{
    TracePriority, DebugPriority, InfoPriority, WarningPriority, ErrorPriority, CriticalPriority
};
class Logger
{
private:
    LogPriority priority = InfoPriority;
    std::mutex logMutex;
    const char* filepath = 0;
    FILE* file = 0;
    
public:
    static void setPriority(LogPriority new_priority) {
        getInstance().priority = new_priority;
    };
    
    //if user wants to log to a file
    static void enableFileOutput() {
        Logger& logger = getInstance();
        logger.filepath = "log.txt";
        logger.EnableFileOutput();
    }
    
    static void enableFileOutput(const char* newFilePath) {
        Logger& logger = getInstance();
        logger.filepath = newFilePath;
        logger.EnableFileOutput();
    }
    
    //template allows for dynamic number of args
    template<typename... Args>
    static void Trace(const char* message, Args... args) {
        getInstance().log("[Trace]\t", TracePriority, message, args...);
    };
    
    template<typename... Args>
    static void Debug(const char* message, Args... args) {
        getInstance().log("[Debug]\t", DebugPriority, message, args...);
    };
    
    template<typename... Args>
    static void Info(const char* message, Args... args) {
        getInstance().log("[Info]\t", InfoPriority, message, args...);
    };
    
    template<typename... Args>
    static void Warning(const char* message, Args... args) {
        getInstance().log("[Warning]\t", WarningPriority, message, args...);
    };
    
    template<typename... Args>
    static void Error(const char* message, Args... args) {
        getInstance().log("[Error]\t", ErrorPriority, message, args...);
    };
    
    template<typename... Args>
    static void Critical(const char* message, Args... args) {
        getInstance().log("[Critical]\t", CriticalPriority, message, args...);
    };
    
    //functions with line num and file name
    template<typename... Args>
    static void Trace(int line, const char* sourceFile, const char* message, Args... args) {
        getInstance().log(line, sourceFile, "[Trace]\t", TracePriority, message, args...);
    };
    
    template<typename... Args>
    static void Debug(int line, const char* sourceFile, const char* message, Args... args) {
        getInstance().log(line, sourceFile, "[Trace]\t", DebugPriority, message, args...);
    };
    
    template<typename... Args>
    static void Info(int line, const char* sourceFile, const char* message, Args... args) {
        getInstance().log(line, sourceFile, "[Info]\t", InfoPriority, message, args...);
    };
    
    template<typename... Args>
    static void Warning(int line, const char* sourceFile, const char* message, Args... args) {
        getInstance().log(line, sourceFile, "[Trace]\t", WarningPriority, message, args...);
    };
    
    template<typename... Args>
    static void Error(int line, const char* sourceFile, const char* message, Args... args) {
        getInstance().log(line, sourceFile, "[Trace]\t", ErrorPriority, message, args...);
    };
    
    template<typename... Args>
    static void Critical(int line, const char* sourceFile, const char* message, Args... args) {
        getInstance().log(line, sourceFile, "[Trace]\t", CriticalPriority, message, args...);
    };
    
private:
    Logger() {}
    
    Logger(const Logger&) = delete;
    Logger& operator = (const Logger&) = delete;
    
    ~Logger() {
        freeFile();
    }
    
    static Logger& getInstance() {
        static Logger logger;
        return logger;
    }
    
    //general method to use for each log type
    template<typename...Args>
    void log(const char* messagePriorityStr, LogPriority messagePriority, const char* message, Args... args) {
        if (priority <= messagePriority) {
            std::time_t currTime = std::time(0);
            std::tm* timestamp = std::localtime(&currTime);
            char buffer[80];
            strftime(buffer, 80, "%c", timestamp);
            
            std::scoped_lock lock(logMutex);
            printf("[%s]\t", buffer);
            printf("%s", messagePriorityStr);
            printf(message, args...);
            printf("\n");
            
            if (file) {
                fprintf(file, "[%s]\t", buffer);
                fprintf(file, "%s", messagePriorityStr);
                fprintf(file, message, args...);
                fprintf(file, "\n");
            }
        }
    }
    
    template<typename...Args>
    void log(int lineNumber, const char* sourceFile, const char* messagePriorityStr, LogPriority messagePriority, const char* message, Args... args) {
        if (priority <= messagePriority) {
            std::time_t currTime = std::time(0);
            std::tm* timestamp = std::localtime(&currTime);
            char buffer[80];
            strftime(buffer, 80, "%c", timestamp);
            
            std::scoped_lock lock(logMutex);
            printf("[%s]\t", buffer);
            printf("%s", messagePriorityStr);
            printf(message, args...);
            printf("\t[line %d in %s]", lineNumber, sourceFile);
            printf("\n");
            
            if (file) {
                fprintf(file, "[%s]\t", buffer);
                fprintf(file, "%s", messagePriorityStr);
                fprintf(file, message, args...);
                fprintf(file, " one line %d in %s", lineNumber, sourceFile);
                fprintf(file, "\n");
            }
        }
    }

    
    void EnableFileOutput() {
        if (file != 0) {
            fclose(file);
        }
        
        file = fopen(filepath, "a");
        
        if (file == 0) {
            printf("Logger: Failed to open file at %s", filepath);
        }
    }
    
    void freeFile() {
        fclose(file);
        file = 0;
    }
    
};

#define LOG_TRACE(Message, ...) (Logger::Trace(__LINE__, __FILE__, Message, __VA_ARGS__))
#define LOG_DEBUG(Message, ...) (Logger::Debug(__LINE__, __FILE__, Message, __VA_ARGS__))
#define LOG_INFO(Message, ...) (Logger::Info(__LINE__, __FILE__, Message, __VA_ARGS__))
#define LOG_WARNING(Message, ...) (Logger::Warning(__LINE__, __FILE__, Message, __VA_ARGS__))
#define LOG_ERROR(Message, ...) (Logger::Error(__LINE__, __FILE__, Message, __VA_ARGS__))
#define LOG_CRITICAL(Message, ...) (Logger::Critical(__LINE__, __FILE__, Message, __VA_ARGS__))
