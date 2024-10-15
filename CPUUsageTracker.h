#include <chrono>
#include <mutex>
#include <deque>
#include <shared_mutex>
using namespace std::literals::chrono_literals;

class CPUTracker {
    public:
        const std::chrono::milliseconds stats_window = 1s;
        const size_t EVENT_LIMIT = 1'000'000;
        void setIdle() {
            std::unique_lock lock(lock_cpu_stats);
            if(cpu_event_log.empty() || cpu_event_log.back().second != IDLE) { // prevent redundant events
                cpu_event_log.push_back({std::chrono::steady_clock::now(), IDLE});
            }
            ensure_interval();
            
        }
        void setActive() {
            std::unique_lock lock(lock_cpu_stats);
            if(cpu_event_log.empty() || cpu_event_log.back().second != ACTIVE) { 
                cpu_event_log.push_back({std::chrono::steady_clock::now(), ACTIVE});
            }
            ensure_interval();
        }
        float getUsage() {
            std::shared_lock lock(lock_cpu_stats);
            ensure_interval();

            std::chrono::duration<float, std::nano> cpu_active_ns = 0ns;
            std::chrono::duration<float, std::nano> cpu_total_ns = 0ns;

            auto cpu_event = cpu_event_log.begin();
            while(cpu_event != cpu_event_log.end()) {
                // calculate start time from current event
                auto event_start_time = cpu_event->first;
                auto event_type = cpu_event->second;
                
                // calculate end time from start time of next event
                cpu_event++; 
                auto event_end_time = cpu_event != cpu_event_log.end() ? cpu_event->first : std::chrono::steady_clock::now() /* event still in progress - use current time instead */;
                auto event_duration = event_end_time - event_start_time;

                // increment counters
                if(event_type == ACTIVE) {
                    cpu_active_ns += event_duration;
                }
                cpu_total_ns += event_duration;
            }

            if(cpu_total_ns == 0ns) return 0;
            return cpu_active_ns * 100.0 / cpu_total_ns; 
        }
    private:
        using time_point = std::chrono::time_point<std::chrono::steady_clock>;
        enum event_type {
            ACTIVE,
            IDLE,
        };
        std::deque<std::pair<time_point, event_type>> cpu_event_log;
        std::shared_mutex lock_cpu_stats;
        void ensure_interval() {
            auto current_time = std::chrono::steady_clock::now();

            // keep the latest "outside of time window" event to ensure that we always have at least one full second of data
            auto second_event = cpu_event_log.size() >= 2 ? std::next(cpu_event_log.begin()) : cpu_event_log.end();
            while(second_event != cpu_event_log.end() && current_time - second_event->first >= stats_window) {
                cpu_event_log.pop_front();
                second_event = std::next(second_event);
            }

            // enforce size limit
            while(cpu_event_log.size() > EVENT_LIMIT) {
                cpu_event_log.pop_front();
            }
        }
};