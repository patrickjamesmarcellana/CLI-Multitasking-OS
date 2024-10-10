#include "Command.h"

Command::Command(CommandType command_type, std::string process_name) : command_type(command_type), process_name(process_name)
{
}

std::string Command::get_process_name()
{
	return this->process_name;
}

