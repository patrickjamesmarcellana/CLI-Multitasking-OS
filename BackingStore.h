#pragma once

#include <fstream>
#include <map>

#include "Process.h"

class BackingStore
{
public:
	typedef std::map<std::string, std::shared_ptr<Process>>& ProcessMap;
	BackingStore(ProcessMap process_map);
	void storeProcess(std::string process_name, int pages);
	int retrieveProcess(std::string process_name);
	size_t page_in = 0, page_out = 0;
private:
	ProcessMap process_map;
	// Backing store has no required limit
};

