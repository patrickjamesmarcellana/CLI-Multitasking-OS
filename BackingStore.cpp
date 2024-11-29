#include "BackingStore.h"

#include <iostream>
BackingStore::BackingStore(ProcessMap process_map):
process_map(process_map)
{
}

void BackingStore::storeProcess(std::string process_name, int pages)
{
    std::ofstream stream(this->process_map[process_name]->getProcessName() + "_memory.txt", std::ios::trunc);
    stream << "id " << this->process_map[process_name]->getId() << std::endl;
    stream << "name " << this->process_map[process_name]->getProcessName() << std::endl;
    stream << "pages " << pages << std::endl;
    stream << "size " << this->process_map[process_name]->get_memory_required() << std::endl;
}

int BackingStore::retrieveProcess(std::string process_name)
{
    const std::string& processMemoryFile = process_name + "_memory.txt";
    std::ifstream file(processMemoryFile);
    std::string line;

    int process_id = -1;
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
