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
#include <sys/types.h>
#include <unistd.h>

using namespace aquabox;

void print_trace(int signum) {
    auto ss = boost::stacktrace::stacktrace();
    std::cerr << "Signal: " << signum << "\n" << ss;
    int pid = ::getpid();

    std::string sstr = boost::stacktrace::detail::to_string(&ss.as_vector()[0], ss.size());

    char fname[128], thread[128];
    sprintf(fname, "/stack_%d.txt", pid);
    FILE *f = ::fopen(fname, "a+");
    if (f != NULL) {
        ::pthread_getname_np(::pthread_self(), thread, 128);
        ::fprintf(f, "\n--------- BEGIN ---------\n");
        ::fprintf(f, "Signal: %d\nPID: %d\nThread: %s\nStacktrace:\n", signum, pid, thread);
        ::fwrite(sstr.c_str(), sstr.size(), 1, f);
        ::fprintf(f, "\n---------- END ----------");
        ::fflush(f);
        ::fclose(f);
    }



    ::signal(signum, SIG_DFL);
    ::raise(SIGQUIT);
}

StackTrace::StackTrace()
{
    for(auto s: std::vector<int>({
        SIGABRT,
        SIGBUS,
        SIGILL,
        SIGSEGV,
        SIGSYS,
        SIGXCPU,
        SIGXFSZ,
        SIGSTKFLT,
        SIGFPE
    })) {
        ::signal(s, print_trace);
    }
}

std::vector<std::string> StackTrace::st()
{
    std::vector<std::string> trace;
    for(auto& f : boost::stacktrace::stacktrace())
    {
        trace.push_back(boost::stacktrace::to_string(f));
    }
    return trace;
}
