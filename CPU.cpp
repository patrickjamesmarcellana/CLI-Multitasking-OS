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

    this->handle_finished_processes();  
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

void* CPU::try_allocating_memory_for_new_process(std::shared_ptr<Process> process)
{
    // try looking for memory space
    void* memory = nullptr;
    if (process)
    {
        memory = this->flat_memory_allocator.allocate(process->get_memory_required(), process->getProcessName());
        if (memory != nullptr) {
            return memory;
        }
    }
    return nullptr;
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
            this->deallocate_memory_of_active_process();
            this->active_process = nullptr;
            this->is_busy = false;
            this->flat_memory_allocator.dec_processes_in_memory();
        }
        else if (algorithm == RR && active_process->getCurrLine() >= this->active_process_time_slice_expiry) // if time slice is used up when the algorithm is RR
        {
            this->active_process->set_assigned_core_id(-1);
            process_queue->push(this->active_process);
            //this->deallocate_memory_of_active_process();
            this->active_process = nullptr;
            this->is_busy = false;
            //this->flat_memory_allocator.dec_processes_in_memory();
        }
    }
}

std::shared_ptr<Process> CPU::get_process_from_queue()
{
    std::shared_ptr<Process> process_in_front = this->process_queue->try_pop();
    if (process_in_front)
    {
        if (process_in_front->get_memory_address() == nullptr) {
            void *memory = this->try_allocating_memory_for_new_process(process_in_front);
            if (memory == nullptr)
            {
                // go back to queue
                this->process_queue->push(process_in_front);
                return nullptr;
            }
            else
            {
                // let's roll
                process_in_front->set_memory_address(memory);
                this->flat_memory_allocator.inc_processes_in_memory();
                return process_in_front;
            }
        }
        else 
        {
            // no allocation needed (e.g. round robin process returns after pre-emption)
            return process_in_front;
        }

    }
    return nullptr;
}

void CPU::handle_reception_of_process()
{
    if (!active_process) {
        active_process = this->get_process_from_queue();


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
            //active_process->getCommandList()[active_process->getCurrLine()]->execute(this->id, time_executed);
            active_process->incCurrLine();
        }

    }
}


