#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#include "logger.h"

int main(int  argc, char *argv[])
{
  logger_id_t id = logger_id_unknown;

  assert(LOGGER_OK == logger_init());

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));

  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));
  assert(LOGGER_OK == logger_id_output_register(id, stdout));
  assert(LOGGER_OK == logger_id_output_level_set(id, stdout, LOGGER_DEBUG));

  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "id %d - LOGGER_EMERG   in line %d\n", id, __LINE__));

  assert(LOGGER_OK == logger_id_output_deregister(id, stdout));
  assert(LOGGER_OK == logger_id_release(id));
  assert(LOGGER_OK == logger_output_deregister(stdout));

  return(0);
}
