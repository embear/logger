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
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_WARNING));

  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_mask_set(id, LOGGER_DEBUG|LOGGER_ERR|LOGGER_CRIT));

  assert(logger_false == logger_id_generates_output(id, LOGGER_DEBUG));
  assert(logger_false == logger_id_generates_output(id, LOGGER_INFO));
  assert(logger_false == logger_id_generates_output(id, LOGGER_NOTICE));
  assert(logger_false == logger_id_generates_output(id, LOGGER_WARNING));
  assert(logger_true  == logger_id_generates_output(id, LOGGER_ERR));
  assert(logger_true  == logger_id_generates_output(id, LOGGER_CRIT));
  assert(logger_false == logger_id_generates_output(id, LOGGER_ALERT));
  assert(logger_false == logger_id_generates_output(id, LOGGER_EMERG));

  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_id_release(id));

  return(0);
}
