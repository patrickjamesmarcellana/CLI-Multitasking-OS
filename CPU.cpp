#include <chrono>
#include <iostream>

#include "CPU.h"
using namespace std::literals::chrono_literals;


CPU::CPU(int id, Algorithm algorithm, ProcessQueue process_queue, long long int delay_per_exec) : Worker(),
id(id),
algorithm(algorithm),
process_queue(process_queue),
delay_per_exec(delay_per_exec)
//time_created(std::chrono::system_clock::now())
{
}

void CPU::loop() {
    if(algorithm == FCFS)
    {
        if (!active_process || active_process->getCurrLine() >= active_process->getTotalLines()) {
            active_process = this->process_queue->try_pop();

            if(active_process) // if CPU finally gets assigned a process
            {
                this->process_cpu_counter = 0;
            }
        }

        if (active_process && active_process->getCurrLine() < active_process->getTotalLines()) {
            this->is_busy = true;

            if(this->process_cpu_counter % delay_per_exec == 0)
            {
                // get the command from the command list that is parallel to the current line of instruction, then execute it by passing the core ID
                active_process->getCommandList()[active_process->getCurrLine()]->execute(this->id, std::chrono::system_clock::now());
            }
            
            active_process->incCurrLine();

            if(active_process->getCurrLine() > active_process->getTotalLines()) // check if incrementing curr line ends the process
            {
                this->is_busy = false;
            }
        }
    } else if(algorithm == RR)
    {
	    // TODO: MO1
    }

    this->inc_cpu_counter();
}


bool CPU::get_is_busy()
{
    return this->is_busy;
}

void CPU::inc_cpu_counter()
{
    this->process_cpu_counter++;
}