#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#include "logger.h"

int main(int  argc, char *argv[])
{
  logger_id_t id  = logger_id_unknown;
  logger_id_t id2 = logger_id_unknown;
  FILE        *logger_stream;
  FILE        *logger_stream2;

  assert(LOGGER_OK == logger_init());

  logger_stream = fopen("test022.out", "w");
  assert(LOGGER_OK == logger_output_register(logger_stream));
  assert(LOGGER_OK == logger_output_level_set(logger_stream, LOGGER_DEBUG));

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_ERR));

  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));

  id2 = logger_id_request("logger_test_id2");
  assert(LOGGER_OK == logger_id_enable(id2));
  assert(LOGGER_OK == logger_id_level_set(id2, LOGGER_DEBUG));

  logger_stream2 = fopen("test022.out2", "w");
  assert(LOGGER_OK == logger_id_output_register(id2, logger_stream2));
  assert(LOGGER_OK == logger_id_output_level_set(id2, logger_stream2, LOGGER_NOTICE));

  assert(LOGGER_OK == logger(id,  LOGGER_DEBUG,   "id %d - LOGGER_DEBUG   in line %d\n", id,  __LINE__));
  assert(LOGGER_OK == logger(id,  LOGGER_INFO,    "id %d - LOGGER_INFO    in line %d\n", id,  __LINE__));
  assert(LOGGER_OK == logger(id,  LOGGER_NOTICE,  "id %d - LOGGER_NOTICE  in line %d\n", id,  __LINE__));
  assert(LOGGER_OK == logger(id,  LOGGER_WARNING, "id %d - LOGGER_WARNING in line %d\n", id,  __LINE__));
  assert(LOGGER_OK == logger(id,  LOGGER_ERR,     "id %d - LOGGER_ERR     in line %d\n", id,  __LINE__));
  assert(LOGGER_OK == logger(id,  LOGGER_CRIT,    "id %d - LOGGER_CRIT    in line %d\n", id,  __LINE__));
  assert(LOGGER_OK == logger(id,  LOGGER_ALERT,   "id %d - LOGGER_ALERT   in line %d\n", id,  __LINE__));
  assert(LOGGER_OK == logger(id,  LOGGER_EMERG,   "id %d - LOGGER_EMERG   in line %d\n", id,  __LINE__));
  assert(LOGGER_OK == logger(id2, LOGGER_DEBUG,   "id %d - LOGGER_DEBUG   in line %d\n", id2, __LINE__));
  assert(LOGGER_OK == logger(id2, LOGGER_INFO,    "id %d - LOGGER_INFO    in line %d\n", id2, __LINE__));
  assert(LOGGER_OK == logger(id2, LOGGER_NOTICE,  "id %d - LOGGER_NOTICE  in line %d\n", id2, __LINE__));
  assert(LOGGER_OK == logger(id2, LOGGER_WARNING, "id %d - LOGGER_WARNING in line %d\n", id2, __LINE__));
  assert(LOGGER_OK == logger(id2, LOGGER_ERR,     "id %d - LOGGER_ERR     in line %d\n", id2, __LINE__));
  assert(LOGGER_OK == logger(id2, LOGGER_CRIT,    "id %d - LOGGER_CRIT    in line %d\n", id2, __LINE__));
  assert(LOGGER_OK == logger(id2, LOGGER_ALERT,   "id %d - LOGGER_ALERT   in line %d\n", id2, __LINE__));
  assert(LOGGER_OK == logger(id2, LOGGER_EMERG,   "id %d - LOGGER_EMERG   in line %d\n", id2, __LINE__));

  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_output_deregister(logger_stream));
  assert(LOGGER_OK == logger_id_release(id));
  assert(LOGGER_OK == logger_id_output_deregister(id2, logger_stream2));
  assert(LOGGER_OK == logger_id_release(id2));
  fclose(logger_stream);
  fclose(logger_stream2);

  return(0);
}
