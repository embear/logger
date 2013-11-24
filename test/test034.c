#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#include "logger.h"
#include <time.h>

time_t time(time_t *t)
{
  return(1234567890);
}

int main(int  argc, char *argv[])
{
  logger_id_t     id  = logger_id_unknown;
  logger_id_t     id2 = logger_id_unknown;
  logger_prefix_t prefix;

  assert(LOGGER_OK == logger_init());

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));

  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));

  id2 = logger_id_request("logger_test_id2");
  assert(LOGGER_OK == logger_id_enable(id2));
  assert(LOGGER_OK == logger_id_level_set(id2, LOGGER_DEBUG));

  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "id %d - LOGGER_EMERG   in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id2, LOGGER_EMERG, "id %d - LOGGER_EMERG   in line %d\n", id2, __LINE__));

  prefix = logger_prefix_get();
  assert (LOGGER_OK == logger_prefix_set(LOGGER_PFX_DATE | LOGGER_PFX_FILE | LOGGER_PFX_LINE));
  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "id %d - LOGGER_EMERG   in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id2, LOGGER_EMERG, "id %d - LOGGER_EMERG   in line %d\n", id2, __LINE__));

  assert(LOGGER_OK == logger_prefix_set(LOGGER_PFX_UNSET));
  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "id %d - LOGGER_EMERG   in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id2, LOGGER_EMERG, "id %d - LOGGER_EMERG   in line %d\n", id2, __LINE__));
  assert(prefix == logger_prefix_get());

  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_id_release(id));
  assert(LOGGER_OK == logger_id_release(id2));

  return(0);
}
