#include "Screen.h"
#include "Process.h"
#include <format>

Screen::Screen(std::shared_ptr<Process> process) : Console::Console("screen"),
runningProcess(process),
command_map({
    { "exit",  [](auto) {  } },
    { "process-smi", [this](auto) { this->displayProcessInfo(); }
    }})
{
}

String Screen::getCommand()
{
    String commandString;
    std::cout << "root:\\>";
    std::getline(std::cin, commandString);
    return commandString;
}


void Screen::displayProcessInfo()
{
    std::cout << "Process: " << this->runningProcess->getProcessName() << std::endl;
    std::cout << "ID: " << this->runningProcess->getId() << std::endl;
    auto local_time = std::chrono::current_zone()->to_local(this->runningProcess->getArrivalTime());
    std::cout << std::format("{:%m/%d/%Y %r}", round<std::chrono::seconds>(local_time)) << std::endl; // see fmt.dev for syntax

    std::cout << "\nCurrent instruction line: " << this->runningProcess->getCurrLine() << std::endl;
    std::cout << "Lines of code: " << this->runningProcess->getTotalLines() << std::endl;
}

void Screen::onEnabled()
{
    bool running = true;
	displayProcessInfo();

    while (running) {
        String command_string = Screen::getCommand();
        std::vector<std::string> command_tokens = str_parsing_methods::split_string_by_space(command_string);
        std::string command_name = command_tokens[0];

        if(command_name == "exit")
        {
            break;
        }

        auto found_command = Screen::command_map.find(command_name);
        if (found_command != Screen::command_map.end()) {
            found_command->second({command_string});
        }
        else {
            std::cout << "Unrecognized command: " << command_string << "\n" << std::endl;
        }
    }
}

namespace str_parsing_methods {
    std::vector<std::string> split_string_by_space(String string) {
        std::vector<String> tokens_list;
        std::istringstream stream(string);
        std::string token;

        while (stream >> token) {
            tokens_list.push_back(token);
        }

        return tokens_list;
    }

    std::string remove_quotes(String text)
    {
        text.erase(std::remove(text.begin(), text.end(), '\"'), text.end());
        return text;
    }
}