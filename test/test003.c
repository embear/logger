#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#include "logger.h"

int main(int  argc, char *argv[])
{
  assert(LOGGER_OK == logger_init());

  assert(logger_false == logger_color_prefix_is_enabled());
  assert(logger_false == logger_color_message_is_enabled());

  assert(LOGGER_OK == logger_color_message_enable());
  assert(LOGGER_OK == logger_color_prefix_enable());

  assert(logger_true == logger_color_prefix_is_enabled());
  assert(logger_true == logger_color_message_is_enabled());

  assert(LOGGER_OK == logger_color_prefix_disable());
  assert(LOGGER_OK == logger_color_message_disable());

  assert(logger_false == logger_color_prefix_is_enabled());
  assert(logger_false == logger_color_message_is_enabled());

  return(0);
}
