#include <iostream>
#include <fstream>
#include <functional>
#include <map>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "BackingStore.h"
#include "CPU.h"
#include "FlatMemoryAllocator.h"
#include "PagingAllocator.h"
#include "ProcessManager.h"
#include "Scheduler.h"
#include "Screen.h"

namespace os_config
{
    std::unordered_map<std::string, CPU::Algorithm> algorithm_map = {
        {"fcfs", CPU::FCFS},
        {"rr", CPU::RR}
    };

    // Default values
    int num_cpu = 4;
    CPU::Algorithm scheduler = CPU::FCFS;
    long long int quantum_cycles = 100LL;
    long long int batch_process_freq = 100LL;
    long long int min_ins = 100LL;
    long long int max_ins = 100LL;
    long long int delays_per_exec = 100000LL;

    long long int max_overall_mem = 1LL;
    long long int mem_per_frame = 1LL;
    long long int min_mem_per_proc = 1LL;
    long long int max_mem_per_proc = 1LL;


    void loadConfig(const std::string& configFile) {
        std::ifstream file(configFile);
        std::string line;

        if (file.is_open()) {
            while (getline(file, line)) {
                std::istringstream iss(line);
                std::string key, value;

                if (!(iss >> key >> value)) {
                    continue;
                }

                value = str_parsing_methods::remove_quotes(value);

                if (key == "num-cpu") {
                    num_cpu = std::stoi(value);
                }
                else if (key == "scheduler") {
                    scheduler = algorithm_map[value];
                }
                else if (key == "quantum-cycles") {
                    quantum_cycles = std::stoll(value);
                }
                else if (key == "batch-process-freq") {
                    batch_process_freq = std::stoll(value);
                }
                else if (key == "min-ins") {
                    min_ins = std::stoll(value);
                }
                else if (key == "max-ins") {
                    max_ins = std::stoll(value);
                }
                else if (key == "delays-per-exec" || key == "delay-per-exec") {
                    delays_per_exec = std::stoll(value);
                }
            	else if (key == "max-overall-mem")
                {
                    max_overall_mem = std::stoll(value) * 1024;
                }
                else if (key == "mem-per-frame")
                {
                    mem_per_frame = std::stoll(value) * 1024;
                }
            	else if (key == "min-mem-per-proc")
                {
                    min_mem_per_proc = std::stoll(value) * 1024;
                }
                else if (key == "max-mem-per-proc")
                {
                    max_mem_per_proc = std::stoll(value) * 1024;
                }
                else if (key == "mem-per-proc")
                {
                    min_mem_per_proc = max_mem_per_proc = std::stoll(value);
                }
            }
            file.close();
        }
        else {
            std::cerr << "Unable to open config file: " << configFile << std::endl;
        }
    }

    void printConfig() {
        std::cout << "Current Configuration:" << std::endl;
        std::cout << "Number of CPUs: " << num_cpu << std::endl;

        std::string scheduler_name = (scheduler == CPU::FCFS) ? "FCFS" : "RR";
        std::cout << "Scheduler: " << scheduler_name << std::endl;

        std::cout << "Quantum Cycles: " << quantum_cycles << std::endl;
        std::cout << "Batch Process Frequency: " << batch_process_freq << std::endl;
        std::cout << "Min Instructions: " << min_ins << std::endl;
        std::cout << "Max Instructions: " << max_ins << std::endl;
        std::cout << "Delays Per Execution: " << delays_per_exec << std::endl;
        std::cout << "Maximum Memory Available: " << max_overall_mem << std::endl;
        std::cout << "Memory Size Per Page: " << mem_per_frame << std::endl;
        std::cout << "Minimum Memory Required Per Process: " << min_mem_per_proc << std::endl;
        std::cout << "Maximum Memory Required Per Process: " << max_mem_per_proc << std::endl;
    }

}

namespace global_objects
{
    std::unordered_map<std::string, std::shared_ptr<Process>> process_map;
    std::shared_mutex process_map_lock;
    std::shared_ptr<ConcurrentPtrQueue<Process>> process_queue = std::make_shared<ConcurrentPtrQueue<Process>>();
    CPUClockSource clock_source;
    ProcessManager process_manager = ProcessManager(process_map, process_map_lock, process_queue, clock_source, os_config::min_ins, os_config::max_ins, os_config::batch_process_freq, os_config::min_mem_per_proc, os_config::max_mem_per_proc);
    std::unique_ptr<IMemoryAllocator> memory_allocator; // temporarily wrap in unique_ptr because we cannot copy/move objects containing a mutex
    BackingStore backing_store = BackingStore(process_map);

    std::unique_ptr<Scheduler> scheduler;
}


namespace shell_commands {
    constexpr auto GREEN_TEXT = "\033[38;5;40m";
    constexpr auto LIGHT_YELLOW_TEXT = "\033[38;5;229m";
    constexpr auto NORMAL_TEXT = "\033[0m";
    constexpr auto CLEAR_SCREEN = "\033[2J";
    constexpr auto MOVE_CURSOR_TO_HOME = "\033[H";

    typedef std::string String;
    typedef std::function<void(std::vector<String>)> command_handler;

    void clear_screen() {
        std::cout << CLEAR_SCREEN << MOVE_CURSOR_TO_HOME;
    }

    void draw_header() {
        std::cout << R"(
 .d8888b.   .d8888b.   .d88888b.  8888888b.  8888888888  .d8888b.  Y88b   d88P 
d88P  Y88b d88P  Y88b d88P" "Y88b 888   Y88b 888        d88P  Y88b  Y88b d88P  
888    888 Y88b.      888     888 888    888 888        Y88b.        Y88o88P   
888         "Y888b.   888     888 888   d88P 8888888     "Y888b.      Y888P    
888            "Y88b. 888     888 8888888P"  888            "Y88b.     888     
888    888       "888 888     888 888        888              "888     888     
Y88b  d88P Y88b  d88P Y88b. .d88P 888        888        Y88b  d88P     888     
 "Y8888P"   "Y8888P"   "Y88888P"  888        8888888888  "Y8888P"      888     
            )" << "\n" <<
            GREEN_TEXT << "Hello, Welcome to CSOPESY commandline!\n" << 
            LIGHT_YELLOW_TEXT << "Type 'exit' to quit, 'clear' to clear the screen\n"; 
    }

    void stub(std::vector<String> command_tokens) {
        std::cout << command_tokens[0] << " command recognized. Doing something.\n" << std::endl;
    }

    String get_command() {
        String command_string;

        std::cout << NORMAL_TEXT << "Enter a command: ";
        std::getline(std::cin, command_string);

        return command_string;
    }

    std::string format_time(const std::chrono::time_point<std::chrono::system_clock>& time_executed) {
        std::time_t time_t_time_executed = std::chrono::system_clock::to_time_t(time_executed);
        std::tm local_time_executed;

        localtime_s(&local_time_executed, &time_t_time_executed);

        std::ostringstream oss;
        oss << "(" << std::put_time(&local_time_executed, "%m/%d/%Y %I:%M:%S%p") << ")";

        return oss.str();
    }


    void dump_state_to_stream(std::ostream &stream) {
        std::unique_lock lock_entire_process_map(global_objects::process_map_lock);

        stream << "CPU utilization: " << global_objects::scheduler->get_cpu_utilization() << "%" << std::endl;
        stream << "Cores used: " << global_objects::scheduler->get_cores_used() << std::endl;
        stream << "Cores available: " << global_objects::scheduler->get_cores_available() << std::endl;
        //stream << "Number of Process in Map: " << global_objects::process_map.size() << std::endl;
        stream << "\n" << "---------------------------------------------" << std::endl;
        stream << "Running processes:" << std::endl;
        for(auto process : global_objects::process_map)
        {
            if (!process.second->is_done_executing() && process.second->get_assigned_core_id() != -1)
            {
                stream << process.second->getProcessName() << "   \t" << format_time(process.second->get_time_executed()) << "\t "
                    << "Core: " << process.second->get_assigned_core_id() << " \t" << process.second->getCurrLine() << " / "
                    << process.second->getTotalLines() << std::endl;
            }
        }

        stream << "\nFinished processes:" << std::endl;
        for (auto process : global_objects::process_map)
        {
            if (process.second->is_done_executing())
            {
                stream << process.second->getProcessName() << "   \t" << format_time(process.second->get_time_executed()) << "\t "
                    << "Finished" << "\t" << process.second->getCurrLine() << " / "
                    << process.second->getTotalLines() << std::endl;
            }
        }
        stream << std::endl;
    }

    void dump_state_to_stream_v2(std::ostream& stream) {
        std::unique_lock lock_entire_process_map(global_objects::process_map_lock);

        stream << "--------------------------------------------" << std::endl;
        stream << "| PROCESS-SMI V01.00 Driver Version: 01.00 |" << std::endl;
        stream << "--------------------------------------------" << std::endl;
        stream << "CPU-Util: " << global_objects::scheduler->get_cpu_utilization() << "%" << std::endl;

        size_t active_memory = global_objects::memory_allocator->get_active_memory();
        stream << "Memory Usage: " << active_memory / 1024 << "KiB / " << os_config::max_overall_mem / 1024 << "KiB" << std::endl;
        stream << "Memory Util: " << 100 * active_memory / os_config::max_overall_mem << "%" << std::endl;

        stream << std::endl;
        stream << "============================================" << std::endl;
        stream << "Running processes and memory usage:" << std::endl;
        stream << "--------------------------------------------" << std::endl;
        for (auto& process : global_objects::process_map)
        {
            if (!process.second->is_done_executing() && process.second->get_assigned_core_id() != -1)
            {
                stream << process.second->getProcessName() << " " << process.second->get_memory_required() / 1024 << "KiB" << std::endl;
            }
        }
        stream << "--------------------------------------------" << std::endl;
    }

    void route_screen(std::vector<String> command_tokens) {

        if (command_tokens[1] == "-r") {
            String processName = command_tokens[2];
            auto foundProcess = global_objects::process_map.find(processName);

            if (foundProcess != global_objects::process_map.end() && foundProcess->second->getCurrLine() < foundProcess->second->getTotalLines())
            {
                clear_screen();
                Screen screen = Screen(foundProcess->second);
                screen.onEnabled();
                clear_screen();
                draw_header();
            } else
            {
                std::cout << "Screen attach failed. Process " << processName << " not found. You may need to initialize via screen -s <process name> command.\n";
            }
        } else if (command_tokens[1] == "-s") {
            clear_screen();
            String processName = command_tokens[2];
            auto foundProcess = global_objects::process_map.find(processName);

            if(foundProcess != global_objects::process_map.end())
            {
                Screen screen = Screen(foundProcess->second);
                screen.onEnabled();
                clear_screen();
                draw_header();
            } else
            {
                std::shared_ptr<Process> new_process = global_objects::process_manager.save_process(processName);
                Screen screen = Screen(new_process);
                screen.onEnabled();
                clear_screen();
                draw_header();
            }
        } else if (command_tokens[1] == "-ls")
        {
            dump_state_to_stream(std::cout);
            //global_objects::memory_allocator->visualize_memory(-420);
        }
    }

    const std::map<std::string, command_handler> command_map = {
        {"clear", [](auto) { clear_screen(); draw_header(); }},
        {"exit",  [](auto) { 
            global_objects::clock_source.shutdown();
            exit(0);
        }},
        {"initialize", [](auto) {
            os_config::loadConfig("config.txt");
        	//os_config::printConfig();
            global_objects::clock_source.setCount(os_config::num_cpu);
            if (os_config::max_overall_mem == os_config::mem_per_frame) {
                global_objects::memory_allocator = std::make_unique<FlatMemoryAllocator>(os_config::max_overall_mem, FlatMemoryAllocator::FIRST_FIT, global_objects::backing_store);
            }
            else {
                global_objects::memory_allocator = std::make_unique<PagingAllocator>(os_config::max_overall_mem, os_config::mem_per_frame, global_objects::backing_store);
            }
            
            global_objects::process_manager.update_configuration(os_config::min_ins, os_config::max_ins, os_config::batch_process_freq, os_config::min_mem_per_proc, os_config::max_mem_per_proc);
            global_objects::scheduler = std::make_unique<Scheduler>(os_config::num_cpu, os_config::scheduler, global_objects::process_queue, global_objects::clock_source, global_objects::process_map_lock, os_config::quantum_cycles, os_config::delays_per_exec,
																	*global_objects::memory_allocator);
            global_objects::scheduler->runScheduler();
        }},
        {"screen", route_screen},
        {"scheduler-test", [](auto) {
            if(global_objects::process_manager.is_generating_processes()) {
                std::cout << "scheduler-test already started." << std::endl;
                return;
            }
            
            global_objects::clock_source.togglePMThread(true);
            global_objects::process_manager.scheduler_test_thread();
        }},
        {"scheduler-stop", [](auto) {
            if(!global_objects::process_manager.is_generating_processes()) {
                std::cout << "scheduler-test already stopped." << std::endl;
                return;
            }
            
            global_objects::clock_source.togglePMThread(false);
            // it will now be running without syncing to cpu threads, but that's okay as we are stopping it anyway
            global_objects::process_manager.scheduler_test_thread_stop();
        }},
        {"report-util", [](auto) {
            std::ofstream stream("csopesy-log.txt", std::ios::trunc);
            dump_state_to_stream(stream);
        }},
        {"vmstat", [](auto) {
            const size_t WIDTH = 12;
            size_t active_memory = global_objects::memory_allocator->get_active_memory();
            auto cpu_idle_active_ticks = global_objects::scheduler->get_idle_active_ticks();
            std::cout << "Total memory     " << std::setw(WIDTH) << std::to_string(os_config::max_overall_mem / 1000) + "KB" << std::endl;
            std::cout << "Used memory      " << std::setw(WIDTH) << std::to_string(active_memory / 1000) + "KB" << std::endl;
            std::cout << "Free memory      " << std::setw(WIDTH) << std::to_string((os_config::max_overall_mem - active_memory) / 1000) + "KB" << std::endl;
            std::cout << "Idle cpu ticks   " << std::setw(WIDTH) << cpu_idle_active_ticks.first << std::endl;
            std::cout << "Active cpu ticks " << std::setw(WIDTH) << cpu_idle_active_ticks.second << std::endl;
            std::cout << "Total cpu ticks  " << std::setw(WIDTH) << cpu_idle_active_ticks.first + cpu_idle_active_ticks.second << std::endl;
            std::cout << "Num paged in     " << std::setw(WIDTH) << global_objects::backing_store.page_in << std::endl;
            std::cout << "Num paged out    " << std::setw(WIDTH) << global_objects::backing_store.page_out << std::endl;
        }},
        {"process-smi", [](auto) {
            dump_state_to_stream_v2(std::cout);
        }},
        {"mem-debug", [](auto) {
            global_objects::memory_allocator->visualize_memory(0);
        }},
    };
}

int main() {
    shell_commands::draw_header();

    while (true) {
        std::string command_string = shell_commands::get_command();
        std::vector<std::string> command_tokens = str_parsing_methods::split_string_by_space(command_string);
        if (command_tokens.size() > 0) {
            std::string command_name = command_tokens[0];

            auto found_command = shell_commands::command_map.find(command_name);
            if (found_command != shell_commands::command_map.end()) {
                if(global_objects::scheduler || command_name == "initialize" || command_name == "exit" ) {
                    found_command->second(command_tokens);
                } else {
                    std::cout << "Processor configuration not yet initialized. Run the \"initialize\" command first." << std::endl;
                }
            }
            else {
                std::cout << "Command not recognized.\n" << std::endl;
            }
        }
    }
}