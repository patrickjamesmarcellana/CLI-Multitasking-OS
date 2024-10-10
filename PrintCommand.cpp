#include "PrintCommand.h"

PrintCommand::PrintCommand(std::string to_print, std::string process_name, SystemTime arrival_time) :
Command(Command::PRINT, process_name),
to_print(to_print),
arrival_time(arrival_time)
{
}

void PrintCommand::execute(int core_id)
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
        appendfile << "(" << this->arrival_time << ")" << " Core:" << core_id << " \"" << to_print << "\"" << std::endl;
    }
}
