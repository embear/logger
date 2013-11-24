#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#include "logger.h"

int main(int  argc, char *argv[])
{
  logger_id_t        id = logger_id_unknown;
  logger_text_attr_t attr;
  logger_text_bg_t   bg_color;
  logger_text_fg_t   fg_color;

  assert(LOGGER_OK == logger_init());

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));

  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));
  assert(LOGGER_OK == logger_color_message_enable());

  for (attr = LOGGER_ATTR_RESET ; attr <= LOGGER_ATTR_HIDDEN ; attr <<= 1)
  {
    for (bg_color = LOGGER_BG_BLACK ; bg_color <= LOGGER_BG_WHITE ; bg_color++)
    {
      for (fg_color = LOGGER_FG_BLACK ; fg_color <= LOGGER_FG_WHITE ; fg_color++)
      {
        logger_id_color_console_set(id, LOGGER_FG_UNCHANGED, LOGGER_BG_UNCHANGED, LOGGER_ATTR_RESET);
        logger(id, LOGGER_DEBUG, "foreground: %2d, background: %2d, attr: %2d  >>>>", fg_color, bg_color, attr);
        logger_id_color_console_set(id, fg_color, bg_color, attr);
        logger(id, LOGGER_DEBUG, "TEST TEST TEST TEST TEST");
        logger_id_color_console_set(id, LOGGER_FG_UNCHANGED, LOGGER_BG_UNCHANGED, LOGGER_ATTR_RESET);
        logger(id, LOGGER_DEBUG, "<<<<\n");
      }
    }
  }

  assert(LOGGER_OK == logger_color_message_disable());
  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_id_release(id));

  return(0);
}
