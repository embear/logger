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
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));
  assert(LOGGER_OK == logger_id_color_string_set(id, "<span style=\"font-family:Courier;color:#00FF00;white-space:pre\">", "</span>"));
  assert(LOGGER_OK == logger_color_message_enable());

  assert(LOGGER_OK == logger_color_prefix_string_set(LOGGER_UNKNOWN, "<span style=\"font-family:Courier;color:#FFFFFF;background-color:#000000;white-space:pre\">", "</span>"));
  assert(LOGGER_OK == logger_color_prefix_string_set(LOGGER_DEBUG,   "<span style=\"font-family:Courier;color:#FFFFFF;background-color:#000000;white-space:pre\">", "</span>"));
  assert(LOGGER_OK == logger_color_prefix_string_set(LOGGER_INFO,    "<span style=\"font-family:Courier;color:#000000;background-color:#FFFFFF;white-space:pre\">", "</span>"));
  assert(LOGGER_OK == logger_color_prefix_string_set(LOGGER_NOTICE,  "<span style=\"font-family:Courier;color:#000000;background-color:#0000FF;white-space:pre\">", "</span>"));
  assert(LOGGER_OK == logger_color_prefix_string_set(LOGGER_WARNING, "<span style=\"font-family:Courier;color:#000000;background-color:#00FFFF;white-space:pre\">", "</span>"));
  assert(LOGGER_OK == logger_color_prefix_string_set(LOGGER_ERR,     "<span style=\"font-family:Courier;color:#000000;background-color:#00FF00;white-space:pre\">", "</span>"));
  assert(LOGGER_OK == logger_color_prefix_string_set(LOGGER_CRIT,    "<span style=\"font-family:Courier;color:#000000;background-color:#FFFF00;white-space:pre\">", "</span>"));
  assert(LOGGER_OK == logger_color_prefix_string_set(LOGGER_ALERT,   "<span style=\"font-family:Courier;color:#000000;background-color:#FF00FF;white-space:pre\">", "</span>"));
  assert(LOGGER_OK == logger_color_prefix_string_set(LOGGER_EMERG,   "<span style=\"font-family:Courier;color:#000000;background-color:#FF0000;white-space:pre\">", "</span>"));

  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "id %d - LOGGER_DEBUG   in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_INFO,    "id %d - LOGGER_INFO    in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_NOTICE,  "id %d - LOGGER_NOTICE  in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_WARNING, "id %d - LOGGER_WARNING in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_ERR,     "id %d - LOGGER_ERR     in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_CRIT,    "id %d - LOGGER_CRIT    in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_ALERT,   "id %d - LOGGER_ALERT   in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_EMERG,   "id %d - LOGGER_EMERG   in line %d\n", id, __LINE__));

  assert(LOGGER_OK == logger_color_prefix_enable());
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "id %d - LOGGER_DEBUG   in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_INFO,    "id %d - LOGGER_INFO    in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_NOTICE,  "id %d - LOGGER_NOTICE  in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_WARNING, "id %d - LOGGER_WARNING in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_ERR,     "id %d - LOGGER_ERR     in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_CRIT,    "id %d - LOGGER_CRIT    in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_ALERT,   "id %d - LOGGER_ALERT   in line %d\n", id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_EMERG,   "id %d - LOGGER_EMERG   in line %d\n", id, __LINE__));

  assert(LOGGER_OK == logger_color_prefix_reset());
  assert(LOGGER_OK == logger_color_prefix_disable());
  assert(LOGGER_OK == logger_color_message_disable());
  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_id_release(id));

  return(0);
}
