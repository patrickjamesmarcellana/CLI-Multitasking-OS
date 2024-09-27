#include <chrono>
#include <format>
#include <functional>
#include <iostream>
#include <unordered_map>
#include <thread>
#include <vector>
#include <memory>
#include <queue>
#include <mutex>

#ifdef _WIN32
// not a proper fix, sleep_for(1ms) still does not sleep for exactly 1ms i think
// but it does not sleep for like 10ms anymore
// better to not sleep at all for small intervals
// temporary: remove once sleep is no longer used?
#include <Windows.h>
#pragma comment(lib, "winmm.lib") // thanks chatgpt
#endif

using namespace std;



using ConsoleProgram = function<void(vector<string>)>;
class Process {
public:
    const int pid;

    const std::chrono::time_point<std::chrono::system_clock> time_created;
    const int lines;
    int cur_line;

    Process(int pid, int lines) :
        pid(pid),
        time_created(std::chrono::system_clock::now()),
        lines(lines),
        cur_line(0) {
    }
};

namespace emulated_cpu {
    queue<shared_ptr<Process>> shared_queue;
    mutex queue_mutex;
    bool stop_cpus = false;
    void run_cpu(int cpu_id) {
        shared_ptr<Process> current_process;

        while (!stop_cpus) { 
            if (current_process.get() == nullptr || current_process->cur_line >= current_process->lines) {
                unique_lock queue_lock(queue_mutex);

                if (!shared_queue.empty()) {
                    current_process = shared_queue.front();
                    printf("DEBUG: CPU %d took a process\n", cpu_id);
                    shared_queue.pop();
                }
                queue_lock.unlock();
            }
            else {
                if (current_process.get() != nullptr) {
                    current_process->cur_line++;
                }
            }
            std::this_thread::sleep_for(1ms);
        }
    }
};


int main() {
#ifdef _WIN32
    // improve accuracy of sleep_for
    timeBeginPeriod(1);
#endif
    unordered_map<int, shared_ptr<Process>> processes;

    thread cpu0 = thread(emulated_cpu::run_cpu, 0);
    thread cpu1 = thread(emulated_cpu::run_cpu, 1);

    ConsoleProgram screen = [&processes](vector<string> params) {
        enum screen_operation {
            CREATE,
            REATTACH,
        };

        int pid = stoi(params[2]);
        if (params[1] == "-r") {
            auto result = processes.find(pid);
            if (result != processes.end()) {
                Process* p = result->second.get();
                cout << "Process Info:" << endl;
                cout << p->pid << endl;
                cout << std::format("{}/{}", p->cur_line, p->lines) << endl;

                auto local_time = std::chrono::current_zone()->to_local(p->time_created);
                cout << std::format("{:%m/%d/%Y %r}", round<chrono::seconds>(local_time)) << endl; // see fmt.dev for syntax
            }
        }
        else if (params[1] == "-s") {
            processes[pid] = make_shared<Process>(pid, 1000);

            unique_lock queue_lock(emulated_cpu::queue_mutex);
            emulated_cpu::shared_queue.push(processes[pid]);
            queue_lock.unlock();
        }
        };

    ConsoleProgram shell = [&screen](auto) {
        screen(vector<string>({ "screen", "-s", "11" }));
        screen(vector<string>({ "screen", "-s", "22" }));
        screen(vector<string>({ "screen", "-s", "33" }));
        screen(vector<string>({ "screen", "-s", "44" }));

        screen(vector<string>({ "screen", "-r", "11" }));
        screen(vector<string>({ "screen", "-r", "22" }));
        std::this_thread::sleep_for(5000ms);
        };

    shell({}); // todo: make optional

    emulated_cpu::stop_cpus = true;
    cpu0.join();
    cpu1.join();
}