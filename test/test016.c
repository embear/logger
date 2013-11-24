#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#include "logger.h"

int main(int  argc, char *argv[])
{
  logger_id_t id = logger_id_unknown;

  assert(LOGGER_OK == logger_init());

  assert(logger_false == logger_output_is_registered(stdout));
  assert(LOGGER_OK == logger_output_register(stdout));
  assert(logger_true == logger_output_is_registered(stdout));
  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(logger_false == logger_output_is_registered(stdout));

  id = logger_id_request("logger_test_id");
  assert(logger_false == logger_id_output_is_registered(id, stdout));
  assert(LOGGER_OK == logger_id_output_register(id, stdout));
  assert(logger_true == logger_id_output_is_registered(id, stdout));
  assert(LOGGER_OK == logger_id_output_deregister(id, stdout));
  assert(logger_false == logger_id_output_is_registered(id, stdout));
  assert(LOGGER_OK == logger_id_release(id));

  return(0);
}
