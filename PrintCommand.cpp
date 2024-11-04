#include "PrintCommand.h"

PrintCommand::PrintCommand(std::string to_print, std::string process_name) :
Command(Command::PRINT, process_name),
to_print(to_print)
{
}

void PrintCommand::execute(int core_id, SystemTime time_executed)
{
    // create a file if it does not exist yet
    std::string filename = this->get_process_name() + ".txt";
    std::ifstream infile(filename);
    if (!infile.good()) // if the file has not been created yet
    {
        std::ofstream outfile(filename);
        if (outfile.good())
        {
            outfile << "Process name: " << this->get_process_name() << std::endl;
            outfile << "Logs:\n" << std::endl;
        }
    }

    // append to a file
    std::ofstream appendfile(filename, std::ios::app);
    if (appendfile.good())
    {
        appendfile << this->format_time(time_executed) << " Core:" << core_id << " \"" << to_print << "\"" << std::endl;
    }
}

std::string PrintCommand::format_time(const std::chrono::time_point<std::chrono::system_clock>& time_executed) {
    std::time_t time_t_time_executed = std::chrono::system_clock::to_time_t(time_executed);
    std::tm local_time_executed;

	localtime_s(&local_time_executed, &time_t_time_executed);

    std::ostringstream oss;
    oss << "(" << std::put_time(&local_time_executed, "%m/%d/%Y %I:%M:%S%p") << ")";

    return oss.str();
}
