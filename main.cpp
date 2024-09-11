#include <iostream>

using namespace std;

// constant variables
constexpr auto GREEN_TEXT = "\x1b[38;5;40m";
constexpr auto LIGHT_YELLOW_TEXT = "\x1b[38;5;229m";
constexpr auto NORMAL_TEXT = "\x1b[0m";

// function prototypes
void display_header();

int main()
{
    display_header();
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