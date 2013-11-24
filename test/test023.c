#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#include "logger.h"

int main(int  argc, char *argv[])
{
  logger_id_t    id = logger_id_unknown;
  logger_level_t level;

  assert(LOGGER_OK == logger_init());

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));

  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_WARNING));

  if (logger_id_is_enabled(id)) {
    printf("Logging level is %s\n", logger_level_name_get(logger_id_level_get(id)));
    assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "id %d - LOGGER_DEBUG   in line %d\n", id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_INFO,    "id %d - LOGGER_INFO    in line %d\n", id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_NOTICE,  "id %d - LOGGER_NOTICE  in line %d\n", id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_WARNING, "id %d - LOGGER_WARNING in line %d\n", id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_ERR,     "id %d - LOGGER_ERR     in line %d\n", id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_CRIT,    "id %d - LOGGER_CRIT    in line %d\n", id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_ALERT,   "id %d - LOGGER_ALERT   in line %d\n", id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_EMERG,   "id %d - LOGGER_EMERG   in line %d\n", id, __LINE__));

    level = logger_id_level_get(id);
    assert(LOGGER_OK == logger_id_level_set(id, LOGGER_ALERT));

    printf("Logging level is %s\n", logger_level_name_get(logger_id_level_get(id)));
    assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "id %d - LOGGER_DEBUG   in line %d\n", id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_INFO,    "id %d - LOGGER_INFO    in line %d\n", id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_NOTICE,  "id %d - LOGGER_NOTICE  in line %d\n", id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_WARNING, "id %d - LOGGER_WARNING in line %d\n", id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_ERR,     "id %d - LOGGER_ERR     in line %d\n", id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_CRIT,    "id %d - LOGGER_CRIT    in line %d\n", id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_ALERT,   "id %d - LOGGER_ALERT   in line %d\n", id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_EMERG,   "id %d - LOGGER_EMERG   in line %d\n", id, __LINE__));

    assert(LOGGER_OK == logger_id_level_set(id, level));

    printf("Logging level is restored to %s\n", logger_level_name_get(logger_id_level_get(id)));
    assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "id %d - LOGGER_DEBUG   in line %d\n", id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_INFO,    "id %d - LOGGER_INFO    in line %d\n", id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_NOTICE,  "id %d - LOGGER_NOTICE  in line %d\n", id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_WARNING, "id %d - LOGGER_WARNING in line %d\n", id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_ERR,     "id %d - LOGGER_ERR     in line %d\n", id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_CRIT,    "id %d - LOGGER_CRIT    in line %d\n", id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_ALERT,   "id %d - LOGGER_ALERT   in line %d\n", id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_EMERG,   "id %d - LOGGER_EMERG   in line %d\n", id, __LINE__));
  }

  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_id_release(id));

  return(0);
}
