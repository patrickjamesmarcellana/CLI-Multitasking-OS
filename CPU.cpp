#include <chrono>
#include <iostream>

#include "CPU.h"
using namespace std::literals::chrono_literals;


CPU::CPU(int id, Algorithm algorithm, ProcessQueue process_queue, long long int delay_per_exec) : Worker(),
id(id),
algorithm(algorithm),
process_queue(process_queue),
delay_per_exec(delay_per_exec)
{
}

void CPU::loop() {
    if(algorithm == FCFS)
    {
        if (!active_process || active_process->getCurrLine() >= active_process->getTotalLines()) {
            this->is_busy = false;
            active_process = this->process_queue->try_pop();
        }

        if (active_process && active_process->getCurrLine() < active_process->getTotalLines()) {
            this->is_busy = true;

            // get the command from the command list that is parallel to the current line of instruction, then execute it by passing the core ID
            active_process->getCommandList()[active_process->getCurrLine()]->execute(this->id, std::chrono::system_clock::now());
            active_process->incCurrLine();
        }

        // TODO: Convert delay_per_exec to CPU CYCLES
        std::chrono::seconds sleep_duration(delay_per_exec);
        sleep(sleep_duration);
    } else if(algorithm == RR)
    {
	    // TODO: MO1
    }
}


bool CPU::get_is_busy()
{
    return this->is_busy;
}
