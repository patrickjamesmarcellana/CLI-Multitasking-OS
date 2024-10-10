#pragma once
#include <chrono>
#include <string>

class Command
{
public:
	typedef std::chrono::time_point<std::chrono::system_clock> SystemTime;

	enum CommandType
	{
		IO,
		PRINT
	};

	Command(CommandType command_type, std::string process_name);
	~Command() = default;
	virtual void execute(int core_id, SystemTime time_executed) = 0;
	std::string get_process_name();

private:
	CommandType command_type;
	std::string process_name;
};

