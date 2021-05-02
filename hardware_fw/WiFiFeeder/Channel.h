#ifndef CHANNEL_H
#define CHANNEL_H

template<int Num>
struct Channel
{
    class Times<Num> TimesProperty;
};

#endif // CHANNEL_H