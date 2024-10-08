#include "Process.h"
#include <string>

Process::Process(int id, String processName, int totalLines) :
	id(id),
	currLine(0),
	totalLines(totalLines),
	processName(processName),
	dateCreated(std::chrono::system_clock::now())
{
}

int Process::getId()
{
	return this->id;
}

String Process::getProcessName()
{
	return this->processName;
}



int Process::getCurrLine()
{
	return this->currLine;
}

int Process::getTotalLines()
{
	return this->totalLines;
}

SystemTime Process::getDateCreated()
{
	return this->dateCreated;
}



void Process::incCurrLine()
{
	this->currLine++;
}