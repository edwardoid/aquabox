#ifndef TIMER_H
#define TIMER_H

#include <thread>
#include <chrono>
#include <atomic>
#include <functional>

namespace aquabox
{
    class Timer
    {
    public:
        using Clock = std::chrono::high_resolution_clock;
        using ClockTimePoint = std::chrono::time_point<Clock>;

        Timer(std::string name);
        ~Timer();

        const std::string& name() const;

        bool start(std::function<void(const ClockTimePoint&)>&& callback,
                   std::chrono::milliseconds timeout);
        void stop();
    protected:
        void run ();
    private:

        enum class State
        {
            Idle = 0,
            Starting,
            Running,
            Stoppping
        };

        const std::string m_name;
        std::atomic<State> m_state;
        std::chrono::milliseconds m_timeout;
        std::chrono::time_point<Clock> m_last;
        std::function<void(ClockTimePoint)> m_callback;
        std::thread m_thread;
    };
}

#endif // TIMER_H
