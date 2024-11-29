#pragma once

#include <fstream>
#include "Process.h"

class BackingStore
{
public:
	void storeProcess(Process process, int pages);
	int retrieveProcess(int process_id);

private:
	// Backing store has no required limit
};

