#include "logger.h"
#include <iostream>
#include <ctime>

// Реализация конструктора
Logger::Logger(const std::string& filename) {
    logFile.open(filename, std::ios::app);  // Открываем файл в режиме добавления
    if (!logFile.is_open()) {
        std::cerr << "Ошибка открытия файла лога: " << filename << std::endl;
    }
}

// Реализация метода log
void Logger::log(const std::string& message) {
    if (!logFile.is_open()) return;

    // Получаем текущее время
    time_t now = time(nullptr);
    char timestamp[100];
    strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S] ", localtime(&now));

    // Записываем в файл и консоль
    logFile << timestamp << message << std::endl;
    logFile.flush();
    std::cout << timestamp << message << std::endl;
}

// Реализация деструктора
Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}