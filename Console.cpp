#include "Console.h"

typedef std::string String;

Console::Console(String name)
{
	this->name = name;
}

String Console::getName()
{
	return this->name;
}