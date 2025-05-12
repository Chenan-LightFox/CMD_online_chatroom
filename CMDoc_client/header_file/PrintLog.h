#pragma once
#include <iostream>
#include <Windows.h>
#include <mutex>


extern std::mutex cout_mutex;

inline void PrintInfo(const std::string& info) {
	std::lock_guard<std::mutex> lock(cout_mutex);
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE); // Set text color to white
	std::cout << "[CLIENT_INFO]\t" << info << std::endl;
}

inline void PrintWarning(const std::string& warning) {
	std::lock_guard<std::mutex> lock(cout_mutex);
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN); // Set text color to yellow
	std::cout << "[CLIENT_WARN]\t" << warning << std::endl;
	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE); // Set text color to white
}

inline void PrintError(const std::string& error) {
	std::lock_guard<std::mutex> lock(cout_mutex);
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED); // Set text color to red
	std::cerr << "[CLIENT_ERROR]\t" << error << std::endl;
	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE); // Set text color to white
}