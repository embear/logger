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
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));

  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));

  assert(LOGGER_OK == logger_color_message_enable());
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));

  assert(LOGGER_OK == logger_id_color_console_set(id, LOGGER_FG_RED, LOGGER_BG_UNCHANGED, LOGGER_ATTR_RESET));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG, "id %d - LOGGER_DEBUG   in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger_id_color_console_set(id, LOGGER_FG_RED, LOGGER_BG_UNCHANGED, LOGGER_ATTR_BRIGHT));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG, "id %d - LOGGER_DEBUG   in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger_id_color_console_set(id, LOGGER_FG_RED, LOGGER_BG_UNCHANGED, LOGGER_ATTR_BLINK));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG, "id %d - LOGGER_DEBUG   in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger_id_color_console_set(id, LOGGER_FG_RED, LOGGER_BG_UNCHANGED, LOGGER_ATTR_REVERSE));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG, "id %d - LOGGER_DEBUG   in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger_id_color_console_set(id, LOGGER_FG_BLUE, LOGGER_BG_UNCHANGED, LOGGER_ATTR_RESET));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG, "id %d - LOGGER_DEBUG   in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger_id_color_console_set(id, LOGGER_FG_YELLOW, LOGGER_BG_UNCHANGED, LOGGER_ATTR_RESET));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG, "id %d - LOGGER_DEBUG   in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger_id_color_console_set(id, LOGGER_FG_GREEN, LOGGER_BG_UNCHANGED, LOGGER_ATTR_RESET));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG, "id %d - LOGGER_DEBUG   in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger_id_color_reset(id));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG, "id %d - LOGGER_DEBUG   in line %d\n", id, __LINE__));

  assert(LOGGER_OK == logger_color_message_disable());
  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_id_release(id));

  return(0);
}
