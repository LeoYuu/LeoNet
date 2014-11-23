#include <stdio.h>
#include <stdlib.h>

#include "loginserver.h"

static void 
sig_handler()
{

}

int
main(int argc, char* argv[])
{
  sig_handler();

  g_loginserver.start_loginserver();

  g_loginserver.main_loop();

  return 0;
}
