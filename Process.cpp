#include "Process.h"

#include "PrintCommand.h"

Process::Process(int id, String processName, long long int totalLines, size_t memory_required) :
	id(id),
	currLine(0LL),
	totalLines(totalLines),
	processName(processName),
	arrivalTime(std::chrono::system_clock::now()),
	memory_required(memory_required),
	memory_address(nullptr)
{
	//for(int commandCnt = 0; commandCnt < totalLines; commandCnt++)
	//{
	//	this->commandList.push_back(std::make_shared<PrintCommand>("Hello world from " + this->processName, this->processName));
	//}
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

bool Process::is_done_executing()
{
	return this->currLine >= this->totalLines;
}

void Process::set_time_executed(SystemTime time_executed)
{
	this->time_executed = time_executed;
}

SystemTime Process::get_time_executed()
{
	return this->time_executed;
}

void Process::set_assigned_core_id(int core_id)
{
	this->assigned_core_id = core_id;
}

int Process::get_assigned_core_id()
{
	return this->assigned_core_id;
}

size_t Process::get_memory_required()
{
	return this->memory_required;
}

void* Process::get_memory_address()
{
	return this->memory_address;
}

void Process::set_memory_address(void* memory_address)
{
	this->memory_address = memory_address;
}