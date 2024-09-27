#pragma once
#include <chrono>
#include <string>

typedef std::string String;
typedef std::chrono::time_point<std::chrono::system_clock> SystemTime;

class Process
{
private:
	const int id;
	String processName;
	int currLine;
	int totalLines;
	std::chrono::time_point<std::chrono::system_clock> dateCreated;

public:
	Process(int id, String processName, int totalLines);
	~Process() = default;

	int getId();
	String getProcessName();
	int getCurrLine();
	int getTotalLines();
	std::chrono::time_point<std::chrono::system_clock> getDateCreated();
};