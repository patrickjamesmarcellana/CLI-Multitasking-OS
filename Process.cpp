#include "Process.h"

#include "PrintCommand.h"

Process::Process(int id, String processName, int totalLines) :
	id(id),
	currLine(0),
	totalLines(totalLines),
	processName(processName),
	arrivalTime(std::chrono::system_clock::now())
{
	for(int commandCnt = 0; commandCnt < totalLines; commandCnt++)
	{
		this->commandList.push_back(std::make_shared<PrintCommand>("Hello world from " + this->processName, this->processName, this->arrivalTime));
	}
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

SystemTime Process::getArrivalTime()
{
	return this->arrivalTime;
}

void Process::incCurrLine()
{
	this->currLine++;
}

std::vector<std::shared_ptr<Command>> Process::getCommandList()
{
	return this->commandList;
}
