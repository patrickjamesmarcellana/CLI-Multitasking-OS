#include "BackingStore.h"

#include <iostream>

void BackingStore::storeProcess(Process process, int pages)
{
    std::ofstream stream(process.getProcessName() + "_memory.txt", std::ios::trunc);
    stream << "id " << process.getId() << std::endl;
    stream << "name " << process.getProcessName() << std::endl;
    stream << "pages " << pages << std::endl;
    stream << "size " << process.get_memory_required() << std::endl;
}

int BackingStore::retrieveProcess(int process_id)
{
    const std::string& processMemoryFile = process_id + "_memory.txt";
    std::ifstream file(processMemoryFile);
    std::string line;

    int process_id = -1;
    std::string process_name = "";
    int pages_count = 1;
    size_t memory_size = 0;

    if(file.is_open())
    {
        while (getline(file, line)) {
            std::istringstream iss(line);
            std::string key, value;

            if (!(iss >> key >> value)) {
                continue;
            }

            if (key == "id")
            {
                process_id = std::stoi(value);
            }
            else if (key == "name")
            {
                process_name = value;
            } 
            else if (key == "pages")
            {
                pages_count = std::stoi(value);
            } else if(key == "size")
            {
                memory_size = std::stoll(value);
            }
        }
        file.close();
    }
    else {
        std::cerr << "Unable to open memory file of process: " << processMemoryFile << std::endl;
    }

    return process_id;
}