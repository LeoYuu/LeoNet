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
 
  login_server::instance()->start_loginserver();
  
  login_server::instance()->main_loop();

  return 0;
}
