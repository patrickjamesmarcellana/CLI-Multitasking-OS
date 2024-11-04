#include <chrono>
#include <iostream>

#include "CPU.h"
#include <shared_mutex>
using namespace std::literals::chrono_literals;


CPU::CPU(int id, Algorithm algorithm, ProcessQueue process_queue, std::shared_mutex& process_map_lock, long long int quantum_cycles, long long int delay_per_exec) : Worker(),
id(id),
algorithm(algorithm),
process_queue(process_queue),
process_map_lock(process_map_lock),
quantum_cycles(quantum_cycles),
delay_per_exec(delay_per_exec)
//time_created(std::chrono::system_clock::now())
{
}

void CPU::loop() {
    // prevents screen -ls/report-util from running until all CPUs release this
    std::shared_lock prevent_lock_entire_process_map(process_map_lock);

    if (active_process) {
        if (active_process->getCurrLine() >= active_process->getTotalLines())
        {
            this->active_process = nullptr;
        }
        else if (algorithm == RR && active_process->getCurrLine() >= this->active_process_time_slice_expiry) // otherwise, check if its time slice is expired (if RR)
        {
            // Note, when returning back to ready queue -> set core id of process to -1
            this->active_process->set_assigned_core_id(-1);
            process_queue->push(this->active_process);
            this->active_process = nullptr;
        }
    }

    if (!active_process || active_process->getCurrLine() >= active_process->getTotalLines()) {
        this->is_busy = false;
        active_process = this->process_queue->try_pop();

        if(active_process) // if CPU finally gets assigned a process
        {
            this->is_busy = true;
            this->process_cpu_counter = 0LL;
            this->active_process->set_assigned_core_id(this->id);
            this->active_process_time_slice_expiry = this->active_process->getCurrLine() + this->quantum_cycles;
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

            if(active_process->getCurrLine() > active_process->getTotalLines()) // check if incrementing curr line ends the process
            {
                this->is_busy = false;
            }
        }

        //sleep(100ms);
        cpu_usage.setActive();
    } else {
        // todo: do not set every cycle
        cpu_usage.setIdle();
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

float CPU::get_cpu_usage() {
    return this->cpu_usage.getUsage();
}
