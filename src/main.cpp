#include "aquabox.h"
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void handler(int sig) {
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
      asService = true;
    if(strcmp(argv[i], "--nodraw") == 0)
      noDraw = true;
  }
  signal(SIGSEGV, handler);
  return aquabox::AquaBox::i().start(asService, !noDraw);
}