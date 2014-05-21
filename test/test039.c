#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#include "logger.h"

int main(int  argc, char *argv[])
{
  logger_id_t id1  = logger_id_unknown;
  logger_id_t id2  = logger_id_unknown;
  int i;

  assert(LOGGER_OK == logger_init());

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));

  id1 = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id1));
  assert(LOGGER_OK == logger_id_level_set(id1, LOGGER_DEBUG));

  id2 = logger_id_request("logger_test_id2");
  assert(LOGGER_OK == logger_id_enable(id2));
  assert(LOGGER_OK == logger_id_level_set(id2, LOGGER_DEBUG));

  /* message repeated */
  for (i = 0; i < 2; i++) {
    assert(LOGGER_OK == logger(id1,  LOGGER_EMERG, "id %d - LOGGER_EMERG repeated message\n", id1));
  }

  /* message changed, only one repeat */
  assert(LOGGER_OK == logger(id1,  LOGGER_EMERG, "id %d - LOGGER_EMERG new message\n", id1));

  /* messages repeated */
  for (i = 0; i < 10; i++) {
    assert(LOGGER_OK == logger(id1,  LOGGER_EMERG, "id %d - LOGGER_EMERG repeated message\n", id1));
  }

  /* message changed */
  assert(LOGGER_OK == logger(id1,  LOGGER_EMERG, "id %d - LOGGER_EMERG new message\n", id1));

  /* messages repeated */
  for (i = 0; i < 10; i++) {
    assert(LOGGER_OK == logger(id1,  LOGGER_EMERG, "LOGGER_EMERG message\n"));
  }

  /* id changed */
  assert(LOGGER_OK == logger(id2,  LOGGER_EMERG, "LOGGER_EMERG message\n"));

  /* messages repeated */
  for (i = 0; i < 10; i++) {
    assert(LOGGER_OK == logger(id1,  LOGGER_EMERG, "id %d - LOGGER_EMERG repeated message\n", id1));
  }

  /* outputs changed */
  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_id_release(id2));
  assert(LOGGER_OK == logger_id_release(id1));

  return(0);
}
