#include "aquabox.h"
#include "options.h"
#include "lock_file.h"
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

void handler(int sig) {
  aquabox::Lockfile lock;
  lock.remove();
  void *array[20];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 20);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

int main(int argc, char** argv)
{
  bool asService = false;
  bool noDraw = false;
  for(int i = 1; i < argc; ++i)
  {
    if(strcmp(argv[i], "--service") == 0)
      aquabox::Options::Service = true;
    else if(strcmp(argv[i], "--nodraw") == 0)
      aquabox::Options::Render = false;
    else if(strcmp(argv[i], "--ignore-lock") == 0)
      aquabox::Options::Lock = false;
    else if(strcmp(argv[i], "--disable-access-point") == 0)
      aquabox::Options::AccessPoint = false;
    else if(strcmp(argv[i], "--standalone") == 0)
      aquabox::Options::EnableCloud = false;
    else if(strcmp(argv[i], "--help") == 0) {
      std::cout << "aquabox [OPTIONS]\n"
                << " --service                Applications will be runned as part of Linux service\n"
                << " --nodraw                 Applications will not render on display\n"
                << " --ignore-lock            Do not create lock/runfiles\n"
                << " --disable-access-point   Do not switch to AP mode if there is no internet connection\n"
                << " --standalone             Do not connect to remote service\n"
                << " --help                   Show this help"
                << std::endl;
      return 0;
    }
  }
  signal(SIGSEGV, handler);
  return aquabox::AquaBox::i().start();
}