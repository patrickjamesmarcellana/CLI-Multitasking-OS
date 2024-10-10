#pragma once
#include <chrono>
#include <string>

#include "Command.h"

typedef std::string String;
typedef std::chrono::time_point<std::chrono::system_clock> SystemTime;

class Process
{
private:
	const int id;
	String processName;
	int currLine;
	int totalLines;
	std::chrono::time_point<std::chrono::system_clock> arrivalTime;
	int cpuCoreID = -1;
	std::vector<std::shared_ptr<Command>> commandList;

public:
	Process(int id, String processName, int totalLines);
	~Process() = default;

	int getId();
	String getProcessName();
	int getCurrLine();
	int getTotalLines();
	std::chrono::time_point<std::chrono::system_clock> getArrivalTime();
	void incCurrLine();
	std::vector<std::shared_ptr<Command>> getCommandList();
};