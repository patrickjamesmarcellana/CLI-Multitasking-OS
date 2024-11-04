#pragma once
#include <chrono>
#include <iostream>
#include <fstream>
#include <string>

#include "Command.h"

class PrintCommand : public Command
{
private:
	typedef std::chrono::time_point<std::chrono::system_clock> SystemTime;
	std::string to_print;
	SystemTime arrival_time;

public:
	PrintCommand(std::string to_print, std::string process_name);
	~PrintCommand() = default;
	void execute(int core_id, SystemTime time_executed) override;
	std::string format_time(const std::chrono::time_point<std::chrono::system_clock>& time_executed);
};

