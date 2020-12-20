/*
 * AquaBox
 * Copyright (C) 2019-2020  Eduard Sargsyan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "stacktrace.h"
#include <iostream>
#include <vector>
#include <signal.h>
#include <boost/stacktrace.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <sys/types.h>
#include <unistd.h>

#define LAST_DUMP "/backtrace.dump"
#define LAST_DUMP_INFO "/backtrace.txt"

using namespace aquabox;

void print_trace(int signum)
{
    ::signal(signum, SIG_DFL);
    
    boost::stacktrace::safe_dump_to(LAST_DUMP);

    //std::ifstream ifs(LAST_DUMP, std::fstream::in);
    //boost::stacktrace::stacktrace ss = boost::stacktrace::stacktrace::from_dump(ifs);
    int pid = ::getpid();
    char thread[128];
    ::pthread_getname_np(::pthread_self(), thread, 128);

    std::ofstream info(LAST_DUMP_INFO, std::fstream::out);
    info << "Signal: " << signal << "\n";
    info << "PID: " << pid << "\n";
    info << "Thread: " << thread << "\n";

    //info << "Dirty: \n";
    //for(auto f : ss)
    //{
    //    info << f << "\n";
    //}

    ::raise(SIGABRT);
}

StackTrace::StackTrace()
{
    if (boost::filesystem::exists(LAST_DUMP))
    {
        // there is a backtrace
        std::ifstream ifs(LAST_DUMP);

        boost::stacktrace::stacktrace ss = boost::stacktrace::stacktrace::from_dump(ifs);

        std::ofstream os(LAST_DUMP_INFO, std::fstream::out | std::fstream::app);
        os << "Clean: \n";
        for(auto f : ss)
        {
            os << f << "\n";
        }

        // cleaning up
        ifs.close();
        boost::filesystem::remove(LAST_DUMP);
    }
    for (auto s : std::vector<int>({SIGABRT,
                                    SIGBUS,
                                    SIGILL,
                                    SIGSEGV,
                                    SIGSYS,
                                    SIGXCPU,
                                    SIGXFSZ,
                                    SIGSTKFLT,
                                    SIGFPE}))
    {
        ::signal(s, print_trace);
    }
}

std::vector<std::string> StackTrace::st()
{
    std::vector<std::string> trace;
    for (auto &f : boost::stacktrace::stacktrace())
    {
        trace.push_back(boost::stacktrace::to_string(f));
    }
    return trace;
}
