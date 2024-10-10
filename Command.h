#pragma once
#include <string>

class Command
{
public:
	enum CommandType
	{
		IO,
		PRINT
	};

	Command(CommandType command_type, std::string process_name);
	~Command() = default;
	virtual void execute(int core_id) = 0;
	std::string get_process_name();

private:
	CommandType command_type;
	std::string process_name;
};

