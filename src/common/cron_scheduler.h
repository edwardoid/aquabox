#pragma once
#include <functional>
#include <stdint.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <map>
#include <queue>
#include <croncpp.h>

#include <log.h>

namespace aquabox
{
    template <typename TaskData>
    class Scheduler
    {
        using clock_t = std::chrono::high_resolution_clock;
        using time_point_t = clock_t::time_point;
        using duration_t = clock_t::duration;

        class Sleep
        {
        public:
            void sleep_for(duration_t duration)
            {
                std::unique_lock<std::mutex> ul(m_m);
                m_cv.wait_for(ul, duration, [this] { return m_interrupted; });
                m_interrupted = false;
            }

            void sleep_until(time_point_t time)
            {
                std::unique_lock<std::mutex> ul(m_m);
                m_cv.wait_until(ul, time, [this] { return m_interrupted; });
                m_interrupted = false;
            }

            void sleep()
            {
                std::unique_lock<std::mutex> ul(m_m);
                m_cv.wait(ul, [this] { return m_interrupted; });
                m_interrupted = false;
            }

            void interrupt()
            {
                std::lock_guard<std::mutex> lg(m_m);
                m_interrupted = true;
                m_cv.notify_one();
            }

        private:
            bool m_interrupted = false;
            std::mutex m_m;
            std::condition_variable m_cv;
        };

    private:
        struct TaskInfo
        {
            cron::cronexpr when;
            TaskData data;
        };
        struct TaskRef
        {
            int id;
            std::time_t when;
        };
        struct Cmp : std::less<TaskRef>
        {
            inline bool operator()(const TaskRef &l, const TaskRef &r) const { return l.when > r.when; }
        };

    public:
        Scheduler() = default;
        void start();
        int add(std::string cronexpr, TaskData data, std::time_t from = clock_t::to_time_t(clock_t::now()));
        void remove(int id);
        void removeIf(std::function<bool(const int&, TaskData&)> cb);

        virtual bool ready(const int& id, TaskData& data) = 0;
        virtual void begin() = 0;
        virtual void finish() = 0;

    private:
        void loop();
        void exec(int id);

    private:
        int m_id = 0;
        std::thread m_loop;
        std::mutex m_mx;
        std::map<int, TaskInfo> m_tasks;
        std::priority_queue<TaskRef, std::vector<TaskRef>, Cmp> m_queue;
        Sleep m_sleep;
    };

    template <typename TaskData>
    void Scheduler<TaskData>::start()
    {
        m_loop = std::thread(std::bind(&Scheduler::loop, this));
        m_loop.detach();
    }

    template <typename TaskData>
    int Scheduler<TaskData>::add(std::string cronexpr, TaskData data, std::time_t from)
    {
        try
        {
            TaskInfo info = {cron::make_cron(cronexpr), data};
            int id = ++m_id;
            TaskRef ref = {id, cron::cron_next(info.when, from)};
            std::lock_guard<std::mutex> lock(m_mx);
            m_queue.push(ref);
            m_tasks.insert({id, info});
            m_sleep.interrupt();
            return ref.id;
        }
        catch (const cron::bad_cronexpr & e)
        {
            LOG_ERROR << e.what();
        }
        catch (const std::runtime_error & e)
        {
            LOG_ERROR << e.what();
        }
        catch (const std::exception & e)
        {
            LOG_ERROR << e.what();
        }

        return 0;
    }

    template <typename TaskData>
    void Scheduler<TaskData>::remove(int id)
    {
        std::lock_guard<std::mutex> lock(m_mx);
        m_tasks.erase(id);
        m_sleep.interrupt();
    }

    template <typename TaskData>
    void Scheduler<TaskData>::removeIf(std::function<bool(const int&, TaskData&)> cb)
    {
        std::lock_guard<std::mutex> lock(m_mx);
        std::vector<int> idsToRemove;
        for (auto& t : m_tasks) {
            if (cb(t.first, t.second.data)) {
                idsToRemove.push_back(t.first);
            }
        }

        for(auto id : idsToRemove) {
            m_tasks.erase(id);
        };
        m_sleep.interrupt();
    }

    template <typename TaskData>
    void Scheduler<TaskData>::loop()
    {
        while (1)
        {
            if (!m_queue.empty()) {
                std::tm* moment = std::localtime(&m_queue.top().when);
                LOG_DEBUG << "Sleeping till " << moment->tm_hour << ":" << moment->tm_min << ":" << moment->tm_sec;
                m_sleep.sleep_until(clock_t::from_time_t(m_queue.top().when));
            } else {
                LOG_DEBUG << "Sleeping...";
                m_sleep.sleep();
            }

            LOG_DEBUG << "Working... Queue size: " << m_queue.size();
            if (!m_queue.empty()) {
                std::tm* moment = std::localtime(&m_queue.top().when);
                LOG_DEBUG << "Next action expected on " << moment->tm_hour << ":" << moment->tm_min << ":" << moment->tm_sec;
            }

            bool emit = false;
            {
                std::lock_guard<std::mutex> lock(m_mx);
                LOG_DEBUG << "Checking...";
                while (!m_queue.empty() && m_queue.top().when <= clock_t::to_time_t(clock_t::now())) {
                    if (!emit) {
                        begin();
                        emit = true;
                    }
                    int id = m_queue.top().id;
                    LOG_DEBUG << "Task " << id << " is ready";
                    m_queue.pop();
                    exec(id);
                }
            }

            if(emit) {
                finish();
            } else {
                LOG_DEBUG << "Nothing is ready.";
            }
        }
    }

    template <typename TaskData>
    void Scheduler<TaskData>::exec(int id)
    {
        if (m_tasks.count(id))
        {
            TaskInfo &t = m_tasks[id];
            try
            {
                if (ready(id, t.data)) {
                    std::time_t when = cron::cron_next(t.when, clock_t::to_time_t(clock_t::now()));
                    TaskRef ref = {id, when};
                    m_queue.push(ref);
                }
                else {
                    m_tasks.erase(id);
                }
            }
            catch (...)
            {}
        }
    }
}

