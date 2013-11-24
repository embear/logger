#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#include "logger.h"

int main(int  argc, char *argv[])
{
  logger_id_t id  = logger_id_unknown;
  logger_id_t id2 = logger_id_unknown;

  assert(LOGGER_OK == logger_init());

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));

  id  = logger_id_request("logger_test_id");
  id2 = logger_id_request("logger_test_id");

  assert(id == id2);
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));
  assert(LOGGER_OK == logger_id_level_set(id2, LOGGER_DEBUG));

  assert(LOGGER_OK == logger_id_enable(id));

  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "id %d - LOGGER_EMERG   in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id2, LOGGER_EMERG, "id %d - LOGGER_EMERG   in line %d\n", id2, __LINE__));

  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_id_release(id));
  assert(LOGGER_OK == logger_id_release(id2));

  return(0);
}
