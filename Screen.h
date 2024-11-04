#pragma once
#include "Console.h"
#include "Process.h"
#include <functional>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <sstream>

class Screen : public Console
{
    typedef std::function<void(std::vector<String>)> commandHandler;
    typedef std::string String;
    std::map<std::string, commandHandler> command_map;

private:
    String getCommand();
    void displayProcessInfo();

public:
	std::shared_ptr<Process> runningProcess;
	Screen(std::shared_ptr<Process> process);
    ~Screen() = default;
    void onEnabled() override;
    //void process() override;
    //void display() override;
};

namespace str_parsing_methods {
    std::vector<std::string> split_string_by_space(String string);
    void remove_quotes(String& text);
}
