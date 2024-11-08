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
	long long int currLine;
	long long int totalLines;
	SystemTime time_executed;
	std::chrono::time_point<std::chrono::system_clock> arrivalTime;
	std::vector<std::shared_ptr<Command>> commandList;
	int assigned_core_id = -1;

	size_t memory_required;
	void* memory_address;

public:
	Process(int id, String processName, long long int totalLines, size_t memory_required);
	~Process() = default;

	int getId();
	String getProcessName();
	long long int getCurrLine();
	long long int getTotalLines();
	std::chrono::time_point<std::chrono::system_clock> getArrivalTime();
	void incCurrLine();
	std::vector<std::shared_ptr<Command>> getCommandList();
	bool is_done_executing();
	void set_time_executed(SystemTime time_executed);
	SystemTime get_time_executed();
	void set_assigned_core_id(int core_id);
	int get_assigned_core_id();

	size_t get_memory_required();
	void* get_memory_address();
	void set_memory_address(void* memory_address);
};