#ifndef LOCKFILE_H
#define LOCKFILE_H

namespace aquabox
{
    class Lockfile
    {
    public:
        Lockfile();
        ~Lockfile();
        Lockfile(const Lockfile&) = delete;
        Lockfile(Lockfile&&) = delete;

        bool create();
        void remove();
    private:
        int m_fd = -1;
    };
}

#endif // LOCKFILE_H