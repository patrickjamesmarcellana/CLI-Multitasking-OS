#include <iostream>
#include <functional>
#include <map>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include "Screen.h"

std::unordered_map<std::string, std::shared_ptr<Process>> processes;

namespace shell_commands {
    constexpr auto GREEN_TEXT = "\033[38;5;40m";
    constexpr auto LIGHT_YELLOW_TEXT = "\033[38;5;229m";
    constexpr auto NORMAL_TEXT = "\033[0m";
    constexpr auto CLEAR_SCREEN = "\033[2J";
    constexpr auto MOVE_CURSOR_TO_HOME = "\033[H";

    typedef std::string String;
    typedef std::function<void(std::vector<String>)> command_handler;

    void clear_screen() {
        std::cout << CLEAR_SCREEN << MOVE_CURSOR_TO_HOME;
    }

    void draw_header() {
        std::cout << R"(
 .d8888b.   .d8888b.   .d88888b.  8888888b.  8888888888  .d8888b.  Y88b   d88P 
d88P  Y88b d88P  Y88b d88P" "Y88b 888   Y88b 888        d88P  Y88b  Y88b d88P  
888    888 Y88b.      888     888 888    888 888        Y88b.        Y88o88P   
888         "Y888b.   888     888 888   d88P 8888888     "Y888b.      Y888P    
888            "Y88b. 888     888 8888888P"  888            "Y88b.     888     
888    888       "888 888     888 888        888              "888     888     
Y88b  d88P Y88b  d88P Y88b. .d88P 888        888        Y88b  d88P     888     
 "Y8888P"   "Y8888P"   "Y88888P"  888        8888888888  "Y8888P"      888     
            )" << "\n" <<
            GREEN_TEXT << "Hello, Welcome to CSOPESY commandline!\n" << 
            LIGHT_YELLOW_TEXT << "Type 'exit' to quit, 'clear' to clear the screen\n"; 
    }

    void stub(std::vector<String> command_tokens) {
        std::cout << command_tokens[0] << " command recognized. Doing something.\n" << std::endl;
    }

    String get_command() {
        String command_string;

        std::cout << NORMAL_TEXT << "Enter a command: ";
        std::getline(std::cin, command_string);

        return command_string;
    }

    void route_screen_param(std::vector<String> command_tokens) {

        if (command_tokens[1] == "-r") {
            String processName = command_tokens[2];
            auto foundProcess = processes.find(processName);

            if (foundProcess != processes.end())
            {
                clear_screen();
                Screen screen = Screen(foundProcess->second);
                screen.onEnabled();
                clear_screen();
                draw_header();
            } else
            {
                std::cout << "Screen attach failed. Process " << processName << " not found. You may need to initialize via screen -s <process name> command.\n";
            }
        } else if (command_tokens[1] == "-s") {
            clear_screen();
            String processName = command_tokens[2];
            auto foundProcess = processes.find(processName);

            if(foundProcess != processes.end())
            {
                Screen screen = Screen(foundProcess->second);
                screen.onEnabled();
                clear_screen();
                draw_header();
            } else
            {
                std::shared_ptr<Process> newProcess = std::make_shared<Process>(processes.size(), processName, 50);
                processes[processName] = newProcess;
                Screen screen = Screen(newProcess);
                screen.onEnabled();
                clear_screen();
                draw_header();
            }
        }
 
    }

    const std::map<std::string, command_handler> command_map = {
        {"clear", [](auto) { clear_screen(); draw_header(); }},
        {"exit",  [](auto) { exit(0); }},
        {"initialize", stub},
        {"screen", route_screen_param},
        {"scheduler-test", stub},
        {"scheduler-stop", stub},
        {"report-util", stub},
    };
}

int main() {

    shell_commands::draw_header();

    while (true) {
        std::string command_string = shell_commands::get_command();
        std::vector<std::string> command_tokens = str_parsing_methods::split_string_by_space(command_string);
        if(command_tokens.size() > 0) {
            std::string command_name = command_tokens[0];
    
            auto found_command = shell_commands::command_map.find(command_name); 
            if (found_command != shell_commands::command_map.end()) {
                found_command->second(command_tokens);
            } else {
                std::cout << "Command not recognized.\n" << std::endl;
            }
        }
    }
}