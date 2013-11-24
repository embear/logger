#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#include "logger.h"

void test_printer(const char * string)
{
  printf("test_printer: %s", string);
}


int main(int  argc, char *argv[])
{
  logger_id_t id = logger_id_unknown;

  assert(LOGGER_OK == logger_init());

  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));
  assert(LOGGER_OK == logger_id_color_console_set(id, LOGGER_FG_GREEN, LOGGER_BG_UNCHANGED, LOGGER_ATTR_RESET));
  assert(LOGGER_OK == logger_color_message_enable());

  assert(LOGGER_OK == logger_id_output_function_register(id, test_printer));
  assert(logger_true == logger_id_output_function_is_registered(id, test_printer));
  assert(LOGGER_OK == logger_id_output_function_level_set(id, test_printer, LOGGER_DEBUG));

  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "id %d - LOGGER_DEBUG   in line %d\n", id, __LINE__));

  assert(LOGGER_OK == logger_id_output_function_deregister(id, test_printer));
  assert(LOGGER_OK == logger_id_release(id));

  return(0);
}
