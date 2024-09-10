#include <cstdlib> // for system()
#include <iostream>
#include <string>
using namespace std;

void displayHeader() {
    cout << R"(
     .d8888b.   .d8888b.   .d88888b.  8888888b.  8888888888  .d8888b.  Y88b   d88P 
    d88P  Y88b d88P  Y88b d88P" "Y88b 888   Y88b 888        d88P  Y88b  Y88b d88P  
    888    888 Y88b.      888     888 888    888 888        Y88b.        Y88o88P   
    888         "Y888b.   888     888 888   d88P 8888888     "Y888b.      Y888P    
    888            "Y88b. 888     888 8888888P"  888            "Y88b.     888     
    888    888       "888 888     888 888        888              "888     888     
    Y88b  d88P Y88b  d88P Y88b. .d88P 888        888        Y88b  d88P     888     
     "Y8888P"   "Y8888P"   "Y88888P"  888        8888888888  "Y8888P"      888     
    )" << endl;
}

void clearScreen() {
// evil but idc
// https://stackoverflow.com/a/15481700
#ifdef _WIN32
    std::system("cls");
#else
    // Assume POSIX
    std::system("clear");
#endif
}

int main() {
    string command;

    displayHeader();
    cout << "Hello, welcome to the CSOPESY command line!" << endl;

    while (true) {
        cout << "Available commands: initialize, screen, scheduler-test, "
                "scheduler-stop, report-util, clear, exit"
             << endl;
        cout << "Enter a command: ";
        getline(cin, command);

        if (command == "initialize" || command == "screen" ||
            command == "scheduler-test" || command == "scheduler-stop" ||
            command == "report-util") {
            cout << command << " command recognized. Doing something.\n"
                 << endl;
        } else if (command == "clear") {
            clearScreen();
            displayHeader();
            cout << "Hello, welcome to the CSOPESY command line!" << endl;
        } else if (command == "exit") {
            break;
        } else {
            cout << "Command not recognized.\n" << endl;
        }
    }

    return 0;
}
