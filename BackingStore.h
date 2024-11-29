#pragma once

#include <fstream>
#include <unordered_map>

#include "Process.h"

class BackingStore
{
public:
	typedef std::unordered_map<std::string, std::shared_ptr<Process>>& ProcessMap;
	BackingStore(ProcessMap process_map);
	void storeProcess(std::string process_name, int pages);
	int retrieveProcess(std::string process_name);

private:
	ProcessMap process_map;
	// Backing store has no required limit
};

