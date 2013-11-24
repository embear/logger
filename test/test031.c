#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#include "logger.h"

int main(int  argc, char *argv[])
{
  logger_id_t id = logger_id_unknown;

  assert(LOGGER_OK == logger_init());

  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));
  assert(LOGGER_OK == logger_id_color_console_set(id, LOGGER_FG_GREEN, LOGGER_BG_UNCHANGED, LOGGER_ATTR_RESET));

  assert(LOGGER_OK == logger_id_output_register(id, stdout));
  assert(LOGGER_OK == logger_id_output_level_set(id, stdout, LOGGER_DEBUG));

  assert(LOGGER_OK == logger_id_output_register(id, stderr));
  assert(LOGGER_OK == logger_id_output_level_set(id, stderr, LOGGER_DEBUG));

  assert(LOGGER_OK == logger_color_message_enable());

  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "id %d - LOGGER_DEBUG   in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger_id_output_color_enable(id, stdout));
  assert(LOGGER_OK == logger_id_output_color_disable(id, stderr));
  assert(logger_true == logger_id_output_color_is_enabled(id, stdout));
  assert(logger_false == logger_id_output_color_is_enabled(id, stderr));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "id %d - LOGGER_DEBUG   in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger_id_output_color_disable(id, stdout));
  assert(LOGGER_OK == logger_id_output_color_enable(id, stderr));
  assert(logger_false == logger_id_output_color_is_enabled(id, stdout));
  assert(logger_true == logger_id_output_color_is_enabled(id, stderr));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "id %d - LOGGER_DEBUG   in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger_id_output_color_enable(id, stdout));
  assert(LOGGER_OK == logger_id_output_color_enable(id, stderr));
  assert(logger_true == logger_id_output_color_is_enabled(id, stdout));
  assert(logger_true == logger_id_output_color_is_enabled(id, stderr));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "id %d - LOGGER_DEBUG   in line %d\n", id, __LINE__));

  assert(LOGGER_OK == logger_color_prefix_disable());
  assert(LOGGER_OK == logger_color_message_disable());
  assert(LOGGER_OK == logger_id_output_deregister(id, stderr));
  assert(LOGGER_OK == logger_id_output_deregister(id, stdout));
  assert(LOGGER_OK == logger_id_release(id));

  return(0);
}
