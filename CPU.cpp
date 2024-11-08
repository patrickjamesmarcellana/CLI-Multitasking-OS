#include <chrono>
#include <iostream>

#include "CPU.h"
#include <shared_mutex>
using namespace std::literals::chrono_literals;


CPU::CPU(int id, Algorithm algorithm, ProcessQueue process_queue, std::shared_mutex& process_map_lock, long long int quantum_cycles, long long int delay_per_exec, FlatMemoryAllocator& flat_memory_allocator) : Worker(),
id(id),
algorithm(algorithm),
process_queue(process_queue),
process_map_lock(process_map_lock),
quantum_cycles(quantum_cycles),
delay_per_exec(delay_per_exec),
flat_memory_allocator(flat_memory_allocator)
//time_created(std::chrono::system_clock::now())
{
}

void CPU::loop() {
    // prevents screen -ls/report-util from running until all CPUs release this
    std::shared_lock prevent_lock_entire_process_map(process_map_lock);

    this->handle_finished_processes();   // for processes that are either done executing or used up quantum slices
    this->handle_reception_of_process(); 
    this->handle_execution_of_process();

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

void CPU::deallocate_memory_of_active_process()
{
    this->flat_memory_allocator.deallocate(this->active_process->get_memory_address(), this->active_process->get_memory_required());
}

void CPU::handle_finished_processes()
{
    if (this->active_process) { // safety check: make sure there is an active process in the CPU

        if (active_process->getCurrLine() >= active_process->getTotalLines()) // if current process finishes executing
        {
            this->active_process = nullptr;
            this->is_busy = false;
            this->deallocate_memory_of_active_process();
        }
        else if (algorithm == RR && active_process->getCurrLine() >= this->active_process_time_slice_expiry) // if time slice is used up when the algorithm is RR
        {
            this->active_process->set_assigned_core_id(-1);
            process_queue->push(this->active_process);
            this->active_process = nullptr;
            this->is_busy = false;
            this->deallocate_memory_of_active_process();
        }
    }
}

void CPU::handle_reception_of_process()
{
    if (!active_process) {

        std::shared_ptr<Process> process_in_front = this->process_queue->peek_front();

        // try looking for memory space
        void* memory = nullptr;
        if(!process_in_front)
        {
             memory = this->flat_memory_allocator.allocate(process_in_front->get_memory_required());
        }

        if (memory == nullptr) // no memory space available
        {
            this->active_process = nullptr;
        }
        else // memory space successfully allocated
        {
            this->active_process = this->process_queue->try_pop();
            this->active_process->set_memory_address(memory);
        }

        if (active_process) // if CPU finally gets assigned a process
        {
            this->is_busy = true;
            this->process_cpu_counter = 0LL;
            this->active_process->set_assigned_core_id(this->id);
            this->active_process_time_slice_expiry = this->active_process->getCurrLine() + this->quantum_cycles;
        }
    }
}

void CPU::handle_execution_of_process()
{
    if (active_process && active_process->getCurrLine() < active_process->getTotalLines()) {
        if (this->process_cpu_counter % (this->delay_per_exec + 1) == 0)
        {
            this->process_cpu_counter = 0;

            // get the command from the command list that is parallel to the current line of instruction, then execute it by passing the core ID
            auto time_executed = std::chrono::system_clock::now();
            active_process->set_time_executed(time_executed);
            active_process->getCommandList()[active_process->getCurrLine()]->execute(this->id, time_executed);
            active_process->incCurrLine();
        }

    }
}


