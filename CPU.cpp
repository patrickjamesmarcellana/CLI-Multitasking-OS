#include <chrono>
#include <iostream>

#include "CPU.h"
using namespace std::literals::chrono_literals;


CPU::CPU(int id, Algorithm algorithm, ProcessQueue process_queue) : Worker(), id(id), algorithm(algorithm), process_queue(process_queue) {

}

void CPU::loop() {
    if(algorithm == FCFS)
    {
        if (!active_process || active_process->getCurrLine() >= active_process->getTotalLines()) {
            active_process = this->process_queue->try_pop();
        }

        if (active_process && active_process->getCurrLine() < active_process->getTotalLines()) {
            // get the command from the command list that is parallel to the current line of instruction, then execute it by passing the core ID
            active_process->getCommandList()[active_process->getCurrLine()]->execute(this->id, std::chrono::system_clock::now());
            //std::cout << "CPU " + std::to_string(id) + ": Hello world from " + active_process->getProcessName() + "\n";
            active_process->incCurrLine();
        }

        sleep(1ms);
    } else if(algorithm == RR)
    {
	    // TODO: MO1
    }
}