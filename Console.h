#pragma once
#include <string>

class Console
{
public:
	typedef std::string String;
	Console(String name);
	~Console() = default;

	String getName();
	virtual void onEnabled() = 0;
	//virtual void display() = 0;
	//virtual void process() = 0;

	String name;
	friend class ConsoleManager;
};