#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#include "logger.h"

int main(int  argc, char *argv[])
{
  logger_id_t id = logger_id_unknown;
  FILE        *logger_stream;

  assert(LOGGER_OK == logger_init());

  logger_stream = fopen("test021.out", "w");
  assert(LOGGER_OK == logger_output_register(logger_stream));
  assert(LOGGER_OK == logger_output_level_set(logger_stream, LOGGER_DEBUG));

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_ERR));

  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));

  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "id %d - LOGGER_DEBUG   in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_INFO,    "id %d - LOGGER_INFO    in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_NOTICE,  "id %d - LOGGER_NOTICE  in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_WARNING, "id %d - LOGGER_WARNING in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_ERR,     "id %d - LOGGER_ERR     in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_CRIT,    "id %d - LOGGER_CRIT    in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_ALERT,   "id %d - LOGGER_ALERT   in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_EMERG,   "id %d - LOGGER_EMERG   in line %d\n", id, __LINE__));

  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_output_deregister(logger_stream));
  fclose(logger_stream);
  assert(LOGGER_OK == logger_id_release(id));

  return(0);
}
