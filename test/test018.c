#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#include "logger.h"

int main(int  argc, char *argv[])
{
  logger_id_t     id = logger_id_unknown;
  logger_prefix_t prefix;

  assert(LOGGER_OK == logger_init());

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));

  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));

  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "id %d - LOGGER_EMERG   in line %d\n", id, __LINE__));

  prefix = logger_id_prefix_get(id);
  assert(LOGGER_OK == logger_id_prefix_set(id, (LOGGER_PFX_FUNCTION | LOGGER_PFX_LINE)));

  assert(LOGGER_OK == logger(id, LOGGER_EMERG, "id %d - LOGGER_EMERG   in line %d\n", id, __LINE__));

  assert(LOGGER_OK == logger_id_prefix_set(id, prefix));

  assert(LOGGER_OK == logger(id, LOGGER_EMERG, "id %d - LOGGER_EMERG   in line %d\n", id, __LINE__));

  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_id_release(id));

  return(0);
}
