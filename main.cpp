#include <iostream>
#include <functional>
#include <map>
#include <string>

// ANSI text colors
constexpr auto GREEN_TEXT = "\033[38;5;40m";
constexpr auto LIGHT_YELLOW_TEXT = "\033[38;5;229m";
constexpr auto NORMAL_TEXT = "\033[0m";
constexpr auto CLEAR_SCREEN = "\033[2J";
constexpr auto MOVE_CURSOR_TO_HOME = "\033[H";

namespace shell_commands {
    typedef std::function<void(std::string)> command_handler;

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

    void stub(std::string name) {
        std::cout << name << " command recognized. Doing something.\n" << std::endl;
    }

    std::string get_command() {
        std::string command_string;

        std::cout << NORMAL_TEXT << "Enter a command: ";
        std::getline(std::cin, command_string);

        return command_string;
    }

    const std::map<std::string, command_handler> command_map = {
        {"clear", [](auto) { clear_screen(); draw_header(); }},
        {"exit",  [](auto) { exit(0); }},
        {"initialize", stub},
        {"screen", stub},
        {"scheduler-test", stub},
        {"scheduler-stop", stub},
        {"report-util", stub},
    };
}

int main() {
    shell_commands::draw_header();

    while (true) {
        std::string command_string = shell_commands::get_command();
        
        // TODO: split the string and get the first word only (e.g. "screen -ls" -> "screen")
        auto found_command = shell_commands::command_map.find(command_string); 
        if (found_command != shell_commands::command_map.end()) {
            found_command->second(command_string);
        } else {
            std::cout << "Command not recognized.\n" << std::endl;
        }
    }
}