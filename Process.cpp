#include "Process.h"

#include "PrintCommand.h"

Process::Process(int id, String processName, long long int totalLines) :
	id(id),
	currLine(0LL),
	totalLines(totalLines),
	processName(processName),
	arrivalTime(std::chrono::system_clock::now())
{
	for(int commandCnt = 0; commandCnt < totalLines; commandCnt++)
	{
		this->commandList.push_back(std::make_shared<PrintCommand>("Hello world from " + this->processName, this->processName));
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

long long int Process::getCurrLine()
{
	return this->currLine;
}

long long int Process::getTotalLines()
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
