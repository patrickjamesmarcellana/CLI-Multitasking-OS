#include <iostream>
#include <string>
#include <unordered_set>
#include <string.h>
#include <cstdlib>

using namespace std;

// constant variables
constexpr auto GREEN_TEXT = "\x1b[38;5;40m";
constexpr auto LIGHT_YELLOW_TEXT = "\x1b[38;5;229m";
constexpr auto NORMAL_TEXT = "\x1b[0m";
constexpr auto CLEAR_SCREEN = "\x1b[2J";
constexpr auto MOVE_CURSOR_TO_HOME = "\x1b[H";

unordered_set<string> COMMANDS_SET = {
    "initialize", "screen", "scheduler-test", "scheduler-stop", "report-util", "clear", "exit"
};

// function prototypes
void display_header();
string get_command();
bool is_valid_command(string);
void clear_screen();

int main()
{
    display_header();
    while (true) {
        string command = get_command();

        if (!command.compare("clear")) {
            clear_screen();
        }

    }

}

void display_header() {
    cout << R"(
 ______   ______   _______   ______   ______   ______   _     _
|  ____| |  ____| |  ___  | |  __  | |  ____| |  ____| | |___| |
| |      | |____  | |   | | | |__| | | |____  | |____  |__   __|
| |      |_____ | | |   | | |  ____| |  ____| |____  |    | |
| |____   ____| | | |___| | | |      | |____   ____| |    | |
|______| |______| |_______| |_|      |______| |______|    |_|
    )" << endl;

    cout << GREEN_TEXT << "Hello, Welcome to the CSOPESY command line!" << NORMAL_TEXT << endl;
    cout << LIGHT_YELLOW_TEXT << "Type 'exit' to quit, 'clear' to clear the screen" << NORMAL_TEXT << endl;
    cout << "Enter a command: ";
}

string get_command() {
    string command;
    getline(cin, command);

    if (is_valid_command(command)) {
        cout << "X command recognized. Doing something." << endl;
        return command;
    }
    else {
        cout << "X command not recognized." << endl;
    }

}

bool is_valid_command(string command) {
    if (COMMANDS_SET.find(command) != COMMANDS_SET.end()) {
        return true;
    }
    else {
        return false;
    }
}

void clear_screen() {
    cout << CLEAR_SCREEN << MOVE_CURSOR_TO_HOME;
    display_header();
}