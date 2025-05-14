#pragma once
#include <Windows.h>
#include <iostream>
#include <mutex>

extern std::mutex cout_mutex;

inline void printInfo(const std::string &info) {
    std::lock_guard<std::mutex> lock(cout_mutex);
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole,
                            FOREGROUND_GREEN | FOREGROUND_RED |
                                FOREGROUND_BLUE); // Set text color to white
    std::cout << "[SERVER_INFO]\t" << info << std::endl;
}

inline void printWarning(const std::string &warning) {
    std::lock_guard<std::mutex> lock(cout_mutex);
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole,
                            FOREGROUND_RED |
                                FOREGROUND_GREEN); // Set text color to yellow
    // std::cout << "[SERVER_WARN]\t" << warning << std::endl;
    SetConsoleTextAttribute(hConsole,
                            FOREGROUND_GREEN | FOREGROUND_RED |
                                FOREGROUND_BLUE); // Set text color to white
}

inline void printError(const std::string &error) {
    std::lock_guard<std::mutex> lock(cout_mutex);
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED); // Set text color to red
    std::cerr << "[SERVER_ERROR]\t" << error << std::endl;
    SetConsoleTextAttribute(hConsole,
                            FOREGROUND_GREEN | FOREGROUND_RED |
                                FOREGROUND_BLUE); // Set text color to white
}