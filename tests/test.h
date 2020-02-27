/*
 * AquaBox
 * Copyright (C) 2019  Eduard Sargsyan
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

#ifndef TEST_FW_H
#define TEST_FW_H
#include <vector>
#include <string>
#include <memory>
#include <iostream>

#define AS_STR(X) #X
#define CONCAT(X, Y) X##Y
#define ASSERT(X) std::cout << ( (X) ? "\033[1;32m OK  \033[0m " : "\033[1;31mFAIL\033[0m " ) << AS_STR(X) << "\t\t\tat " << __FILE__ << ":" << __LINE__ <<std::endl;
#define REPORT(X) std::cout << "\033[1;32m EXP  \033[0m " << AS_STR(X) << " = " << (X) << "\t\t" << __FILE__ << ":" << __LINE__ << std::endl;

struct Suite
{
    Suite() {}
    std::vector<class Test*> m_tests;
    static Suite& instance() { static Suite instance; return instance;  }
    static void add(class Test* test);
    static void run();
};

struct Test
{
    Test(const char* n) : name(n) {
        Suite::add(this);
    }
    const char* name;
    virtual void run() = 0;
};


void Suite::add(class Test* test)
{
    instance().m_tests.push_back(test);
}

void Suite::run()
{
    for (Test* t : instance().m_tests)
    {
        std::cout << "\033[1;33m\t\t\tRunning\033[0m\t" << t->name << std::endl;
        t->run();
        std::cout << "\033[1;33m\t\t\tDone\033[0m\t" << t->name << std::endl;
    }
}

#define TEST(Name) struct CONCAT(Test, Name) : public Test \
                   {                                           \
                        CONCAT(Test, Name) () : Test(AS_STR(Name)) {} \
                        void run() override;                                      \
                   };                                                             \
                   static CONCAT(Test, Name) CONCAT(test, Name);                   \
                   void CONCAT(Test, Name) ::run()

#endif // TEST_FW_H