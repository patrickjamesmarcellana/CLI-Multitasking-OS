#include <chrono>
#include <iostream>

#include "CPU.h"
#include <shared_mutex>
using namespace std::literals::chrono_literals;


CPU::CPU(int id, Algorithm algorithm, ProcessQueue process_queue, long long int delay_per_exec, CPUSemaphores& semaphores) : Worker(),
id(id),
algorithm(algorithm),
process_queue(process_queue),
delay_per_exec(delay_per_exec),
semaphores(semaphores)
//time_created(std::chrono::system_clock::now())
{
}

void CPU::loop() {
    semaphores.waitUntilCycleStart();

    if(algorithm == FCFS)
    {
        if (!active_process || active_process->getCurrLine() >= active_process->getTotalLines()) {
            this->is_busy = false;
            active_process = this->process_queue->try_pop();

            if(active_process) // if CPU finally gets assigned a process
            {
                this->is_busy = true;
                this->process_cpu_counter = 0LL;
                this->active_process->set_assigned_core_id(this->id);
            }
        }

        if (active_process && active_process->getCurrLine() < active_process->getTotalLines()) {
            if(this->process_cpu_counter % this->delay_per_exec == 0)
            {
                this->process_cpu_counter = 0;
                // get the command from the command list that is parallel to the current line of instruction, then execute it by passing the core ID
                auto time_executed = std::chrono::system_clock::now();
                active_process->set_time_executed(time_executed);
                active_process->getCommandList()[active_process->getCurrLine()]->execute(this->id, time_executed);
                active_process->incCurrLine();
            }

            if(active_process->getCurrLine() > active_process->getTotalLines()) // check if incrementing curr line ends the process
            {
                this->is_busy = false;
            }
            cpu_usage.setActive();
        } else {
            // todo: do not set every cycle
            cpu_usage.setIdle();
        }

        //sleep(100ms);
    } else if(algorithm == RR)
    {
	    // TODO: MO1
        // Note, when returning back to ready queue -> set core id of process to -1
    }

    semaphores.notifyDone();
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

float CPU::get_cpu_usage() {
    return this->cpu_usage.getUsage();
}
