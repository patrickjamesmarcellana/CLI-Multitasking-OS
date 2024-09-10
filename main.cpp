#include <iostream>
#include <functional>
#include <map>
#include <string>

namespace shell_commands {
    typedef std::function<void(std::string)> command_handler;
    void draw_header() {
        std::cout << "\033[2J" <<
            "   ____ ____   ___  ____  _____ ______   __\n" <<
            "  / ___/ ___| / _ \\|  _ \\| ____/ ___\\ \\ / /\n" <<
            " | |   \\___ \\| | | | |_) |  _| \\___ \\\\ V / \n" <<
            " | |___ ___) | |_| |  __/| |___ ___) || |  \n" <<
            "  \\____|____/ \\___/|_|   |_____|____/ |_|  \n" <<
            "\n" <<
            "\033[32mHello, Welcome to CSOPESY commandline!\n" << // 32m is green
            "\033[93mType 'exit' to quit, 'clear' to clear the screen\n"; // 93m is yellow
    }

    void stub(std::string name) {
        std::cout << name << " command recognized. Doing something." << std::endl;
    }

    const std::map<std::string, command_handler> command_map = {
        {"clear", [](auto) { draw_header(); }},
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
        std::string command_string;

        std::cout << "\033[0mEnter a command: "; // 0m is reset all
        std::getline(std::cin, command_string);
        
        // TODO: split the string and get the first word only (e.g. "screen -ls" -> "screen")
        auto found_command = shell_commands::command_map.find(command_string); 
        if (found_command != shell_commands::command_map.end()) {
            found_command->second(command_string);
        }
        else {
            std::cout << "Command not recognized." << std::endl;
        }
    }
}