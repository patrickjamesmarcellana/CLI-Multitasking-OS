#include <iostream>
#include <string>
#include <windows.h>

static void clear_screen(char fill = ' ') {
    COORD tl = { 0,0 };
    CONSOLE_SCREEN_BUFFER_INFO s;
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(console, &s);
    DWORD written, cells = s.dwSize.X * s.dwSize.Y;
    FillConsoleOutputCharacter(console, fill, cells, tl, &written);
    FillConsoleOutputAttribute(console, s.wAttributes, cells, tl, &written);
    SetConsoleCursorPosition(console, tl);
}

static void display() {
    std::cout << R"(
      __    _____  ___    ________     __      _____  ___    __  ___________  ___  ___         ______    ________  
     |" \  (\"   \|"  \  /"       )   /""\    (\"   \|"  \  |" \("     _   ")|"  \/"  |       /    " \  /"       ) 
     ||  | |.\\   \    |(:   \___/   /    \   |.\\   \    | ||  |)__/  \\__/  \   \  /       // ____  \(:   \___/  
     |:  | |: \.   \\  | \___  \    /' /\  \  |: \.   \\  | |:  |   \\_ /      \\  \/       /  /    ) :)\___  \    
     |.  | |.  \    \. |  __/  \\  //  __'  \ |.  \    \. | |.  |   |.  |      /   /       (: (____/ //  __/  \\   
     /\  |\|    \    \ | /" \   :)/   /  \\  \|    \    \ | /\  |\  \:  |     /   /         \        /  /" \   :)  
    (__\_|_)\___|\____\)(_______/(___/    \___)\___|\____\)(__\_|_)  \__|    |___/           \"_____/  (_______/   
                                                                                                                      
    )" << '\n';

    std::cout << "Hello, welcome to the Insanity OS commandline!" << '\n';
    std::cout << "Type 'exit' to quit, 'clear' to clear the screen." << '\n';
}

enum command {
    INITIALIZE,
    SCREEN,
    SCHEDULER_TEST,
    SCHEDULER_STOP,
    REPORT_UTIL,
    CLEAR,
    EXIT,
    INVALID
};

command getCommand(std::string& str) {
    if (str == "initialize") return INITIALIZE;
    if (str == "screen") return SCREEN;
    if (str == "scheduler-test") return SCHEDULER_TEST;
    if (str == "scheduler-stop") return SCHEDULER_STOP;
    if (str == "report-util") return REPORT_UTIL;
    if (str == "clear") return CLEAR;
    if (str == "exit") return EXIT;
    return INVALID;
}

int main()
{
    bool on = true;

    display();
    
    while (on) {
        std::cout << "Enter command: ";

        std::string input;
        std::cin >> input;
        command command = getCommand(input);

        switch (command) {
        case INITIALIZE:
        case SCREEN:
        case SCHEDULER_TEST:
        case SCHEDULER_STOP:
        case REPORT_UTIL:
            std::cout << input << " command recognized. Doing something." << std::endl;
            break;
        case CLEAR:
            clear_screen();
            display();
            break;
        case EXIT:
            on = false;
            break;
        case INVALID:
        default:
            std::cout << "Invalid command." << std::endl;
            break;
        }
    }

    return 0;
}