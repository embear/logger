#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#include "logger.h"

int main(int  argc,
         char *argv[])
{
  FILE               *logger_stream;
  logger_id_t        id  = logger_id_unknown;

  assert(LOGGER_OK == logger_init());
  assert(LOGGER_OK == logger_color_message_enable());
  assert(LOGGER_OK == logger_color_prefix_enable());

  logger_stream = fopen("test38.out", "w");
  assert(LOGGER_OK == logger_output_register(logger_stream));
  assert(LOGGER_OK == logger_output_level_set(logger_stream, LOGGER_DEBUG));

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_ERR));

  assert(LOGGER_OK == logger_output_register(stderr));
  assert(LOGGER_OK == logger_output_level_set(stderr, LOGGER_ALERT));

  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));

  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test - id %d - LOGGER_DEBUG   in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_INFO,    "test - id %d - LOGGER_INFO    in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_NOTICE,  "test - id %d - LOGGER_NOTICE  in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_WARNING, "test - id %d - LOGGER_WARNING in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_ERR,     "test - id %d - LOGGER_ERR     in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_CRIT,    "test - id %d - LOGGER_CRIT    in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_ALERT,   "test - id %d - LOGGER_ALERT   in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_EMERG,   "test - id %d - LOGGER_EMERG   in line %d\n", id, __LINE__));

  assert(LOGGER_OK == logger_output_deregister(stderr));
  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_output_deregister(logger_stream));
  fclose(logger_stream);
  assert(LOGGER_OK == logger_id_release(id));

  (void)id;

  return(0);
}


/* vim: set cino=t0,c0,(0,u0 et fen fdm=syntax fo=tcroqnl1 sw=2 sts=2 ts=8 : */
