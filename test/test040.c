#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <time.h>
#include "logger.h"

int main(int  argc, char *argv[])
{
  logger_id_t id  = logger_id_unknown;
  int i;
  time_t current_time;

  assert(LOGGER_OK == logger_init());

  assert(0 == logger_rate_limit_get());
  assert(LOGGER_OK == logger_rate_limit_set(10));
  assert(10 == logger_rate_limit_get());

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));

  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));

  current_time = time(NULL);
  while (current_time == time(NULL));

  for (i = 0; i < 10; i++) {
    assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "id %d - LOGGER_EMERG message %d\n", id, i));
  }
  for (i = 0; i < 1; i++) {
    assert(LOGGER_ERR_RATE_LIMIT == logger(id,  LOGGER_EMERG, "id %d - LOGGER_EMERG message %d\n", id, i));
  }

  current_time = time(NULL);
  while (current_time == time(NULL));

  for (i = 0; i < 10; i++) {
    assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "id %d - LOGGER_EMERG message %d\n", id, i));
  }
  for (i = 0; i < 10; i++) {
    assert(LOGGER_ERR_RATE_LIMIT == logger(id,  LOGGER_EMERG, "id %d - LOGGER_EMERG message %d\n", id, i));
  }

  current_time = time(NULL);
  while (current_time == time(NULL));

  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_id_release(id));

  return(0);
}
