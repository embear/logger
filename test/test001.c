#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#include "logger.h"

int main(int  argc, char *argv[])
{
  assert(logger_false == logger_is_initialized());
  assert(LOGGER_OK == logger_init());
  assert(logger_true == logger_is_initialized());

  return(0);
}
