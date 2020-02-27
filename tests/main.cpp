#include "test.h"
#include <time.h>

#include "aquabox/time_details.h"
#include <vector>
#include <map>
#include <algorithm>

void testRule(std::vector<int> points, int n, int expectToWin)
{
    time_t rawtime;

    time (&rawtime);
    struct tm * now = localtime (&rawtime);
    now->tm_hour = n;

    aquabox::TimeDetails tdNow(mktime(now));

    static int testNum = 0;
    //std::cout << "\n\n\nTest " << ++testNum << "\n";

    std::map<int /*delta*/, int /* point */> pending;
    for( auto h : points)
    {
        tm p = *now;
        p.tm_hour = h;
        int d = tdNow.from(aquabox::TimeDetails(mktime(&p)));
        pending.insert({ d, h });
    }

    auto lb = pending.lower_bound(0);

    REPORT(expectToWin);
    if (lb == pending.begin()) {
        REPORT(pending.rbegin()->second);
        ASSERT(pending.rbegin()->second == expectToWin);
    } else {
        if (lb->first != 0) {
            --lb;
        }
        REPORT(lb->second);
        ASSERT(lb->second == expectToWin);
    }
}

TEST(TimeDetails) {
    testRule({15, 20, 22, 23}, 1, 23);

    testRule({15, 20, 22, 23}, 14, 23);

    testRule({15, 20, 22, 23}, 16, 15);

    testRule({15, 20, 22, 23}, 18, 15);
    
    testRule({15, 20, 22, 23}, 21, 20);

    testRule({15, 20, 22, 23}, 23, 23);

    testRule({15, 22}, 0, 22);

    testRule({23, 1}, 0, 23);
    testRule({23, 1}, 23, 23);
    testRule({15, 22}, 23, 22);
}

int main()
{
    Suite::run();
    return 0;
}