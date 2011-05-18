#include <stdio.h>
#include "logger.h"

int main(int  argc,
         char *argv[])
{
  int            test = 0;
  FILE           *stream;
  logger_level_t level;
  logger_id_t    id  = logger_id_unknown;
  logger_id_t    id2 = logger_id_unknown;

  logger_init();

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- id not registered ....\n", test);

  puts("No log message will be shown because id is not registered");

  logger_output_register(stdout);

  logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_INFO,    "test %d - id %d - LOGGER_INFO    in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_NOTICE,  "test %d - id %d - LOGGER_NOTICE  in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_WARNING, "test %d - id %d - LOGGER_WARNING in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_ERR,     "test %d - id %d - LOGGER_ERR     in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_CRIT,    "test %d - id %d - LOGGER_CRIT    in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_ALERT,   "test %d - id %d - LOGGER_ALERT   in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_EMERG,   "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__);

  logger_output_deregister(stdout);

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- id not enabled ....\n", test);

  puts("No log message will be shown because id is not enabled");

  logger_output_register(stdout);
  id = logger_id_request();

  logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_INFO,    "test %d - id %d - LOGGER_INFO    in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_NOTICE,  "test %d - id %d - LOGGER_NOTICE  in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_WARNING, "test %d - id %d - LOGGER_WARNING in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_ERR,     "test %d - id %d - LOGGER_ERR     in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_CRIT,    "test %d - id %d - LOGGER_CRIT    in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_ALERT,   "test %d - id %d - LOGGER_ALERT   in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_EMERG,   "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__);

  logger_output_deregister(stdout);
  logger_id_release(id);

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- level not set ....\n", test);

  puts("All log messages will be shown because id level is not set ( -> level is DEBUG by default)");

  logger_output_register(stdout);
  id = logger_id_request();
  logger_id_enable(id);

  logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_INFO,    "test %d - id %d - LOGGER_INFO    in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_NOTICE,  "test %d - id %d - LOGGER_NOTICE  in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_WARNING, "test %d - id %d - LOGGER_WARNING in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_ERR,     "test %d - id %d - LOGGER_ERR     in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_CRIT,    "test %d - id %d - LOGGER_CRIT    in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_ALERT,   "test %d - id %d - LOGGER_ALERT   in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_EMERG,   "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__);

  logger_output_deregister(stdout);
  logger_id_release(id);

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- level set to LOGGER_WARNING ....\n", test);

  puts("Only log messages with level above or equal to WARNING will be shown");

  logger_output_register(stdout);
  id = logger_id_request();
  logger_id_enable(id);
  logger_id_level_set(id, LOGGER_WARNING);

  logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_INFO,    "test %d - id %d - LOGGER_INFO    in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_NOTICE,  "test %d - id %d - LOGGER_NOTICE  in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_WARNING, "test %d - id %d - LOGGER_WARNING in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_ERR,     "test %d - id %d - LOGGER_ERR     in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_CRIT,    "test %d - id %d - LOGGER_CRIT    in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_ALERT,   "test %d - id %d - LOGGER_ALERT   in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_EMERG,   "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__);

  logger_output_deregister(stdout);
  logger_id_release(id);

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- id enabled, id2 not ....\n", test);

  puts("Only log messages of id will be shown");

  logger_output_register(stdout);
  id = logger_id_request();
  logger_id_enable(id);

  logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__);
  logger(id2, LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__);

  logger_output_deregister(stdout);
  logger_id_release(id);

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- output to stdout and stderr ....\n", test);

  puts("All log messages will be printed to stdout and stderr");

  logger_output_register(stdout);
  logger_output_register(stderr);
  id = logger_id_request();
  logger_id_enable(id);
  logger_id_level_set(id, LOGGER_ERR);

  logger(id, LOGGER_DEBUG, "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_ERR,   "test %d - id %d - LOGGER_ERR     in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__);

  logger_output_deregister(stdout);
  logger_output_deregister(stderr);
  logger_id_release(id);

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- output to stdout and file ....\n", test);

  puts("All log messages will be printed to stdout and appended to file \"logfile\"");

  stream = fopen("logfile", "w");
  logger_output_register(stdout);
  logger_output_register(stream);
  id = logger_id_request();
  logger_id_enable(id);
  logger_id_level_set(id, LOGGER_ERR);

  logger(id, LOGGER_DEBUG, "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_ERR,   "test %d - id %d - LOGGER_ERR     in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__);

  logger_output_deregister(stdout);
  logger_output_deregister(stream);
  fclose(stream);
  logger_id_release(id);

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- using control functions ....\n", test);

  puts("Changing and restoring logging level");

  logger_output_register(stdout);
  id = logger_id_request();
  logger_id_enable(id);
  logger_id_level_set(id, LOGGER_WARNING);

  if (logger_id_is_enabled(id)) {
    puts("Logging level is WARNING");
    logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__);
    logger(id, LOGGER_INFO,    "test %d - id %d - LOGGER_INFO    in line %d\n", test, id, __LINE__);
    logger(id, LOGGER_NOTICE,  "test %d - id %d - LOGGER_NOTICE  in line %d\n", test, id, __LINE__);
    logger(id, LOGGER_WARNING, "test %d - id %d - LOGGER_WARNING in line %d\n", test, id, __LINE__);
    logger(id, LOGGER_ERR,     "test %d - id %d - LOGGER_ERR     in line %d\n", test, id, __LINE__);
    logger(id, LOGGER_CRIT,    "test %d - id %d - LOGGER_CRIT    in line %d\n", test, id, __LINE__);
    logger(id, LOGGER_ALERT,   "test %d - id %d - LOGGER_ALERT   in line %d\n", test, id, __LINE__);
    logger(id, LOGGER_EMERG,   "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__);

    level = logger_id_level_get(id);
    logger_id_level_set(id, LOGGER_ALERT);

    puts("Logging level is changed to ALERT");
    logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__);
    logger(id, LOGGER_INFO,    "test %d - id %d - LOGGER_INFO    in line %d\n", test, id, __LINE__);
    logger(id, LOGGER_NOTICE,  "test %d - id %d - LOGGER_NOTICE  in line %d\n", test, id, __LINE__);
    logger(id, LOGGER_WARNING, "test %d - id %d - LOGGER_WARNING in line %d\n", test, id, __LINE__);
    logger(id, LOGGER_ERR,     "test %d - id %d - LOGGER_ERR     in line %d\n", test, id, __LINE__);
    logger(id, LOGGER_CRIT,    "test %d - id %d - LOGGER_CRIT    in line %d\n", test, id, __LINE__);
    logger(id, LOGGER_ALERT,   "test %d - id %d - LOGGER_ALERT   in line %d\n", test, id, __LINE__);
    logger(id, LOGGER_EMERG,   "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__);

    logger_id_level_set(id, level);

    puts("Logging level is restored");
    logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__);
    logger(id, LOGGER_INFO,    "test %d - id %d - LOGGER_INFO    in line %d\n", test, id, __LINE__);
    logger(id, LOGGER_NOTICE,  "test %d - id %d - LOGGER_NOTICE  in line %d\n", test, id, __LINE__);
    logger(id, LOGGER_WARNING, "test %d - id %d - LOGGER_WARNING in line %d\n", test, id, __LINE__);
    logger(id, LOGGER_ERR,     "test %d - id %d - LOGGER_ERR     in line %d\n", test, id, __LINE__);
    logger(id, LOGGER_CRIT,    "test %d - id %d - LOGGER_CRIT    in line %d\n", test, id, __LINE__);
    logger(id, LOGGER_ALERT,   "test %d - id %d - LOGGER_ALERT   in line %d\n", test, id, __LINE__);
    logger(id, LOGGER_EMERG,   "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__);
  }

  logger_output_deregister(stdout);
  logger_id_release(id);

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- continued messages ....\n", test);

  puts("Messages will be all in one line");

  logger_output_register(stdout);
  id = logger_id_request();
  logger_id_enable(id);
  logger_id_level_set(id, LOGGER_DEBUG);

  logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d", test, id, __LINE__);
  logger(id, LOGGER_DEBUG,   " - continued 1");
  logger(id, LOGGER_DEBUG,   " - continued 2");
  logger(id, LOGGER_DEBUG,   " - continued 3\n");
  logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d", test, id, __LINE__);
  logger(id, LOGGER_DEBUG,   " - continued 1");
  logger(id, LOGGER_DEBUG,   " - continued 2");
  logger(id, LOGGER_DEBUG,   " - continued 3\n");

  logger_output_deregister(stdout);
  logger_id_release(id);

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- colors in text output ....\n", test);

  puts("Print log messages in different colors");

  logger_output_register(stdout);
  id = logger_id_request();
  logger_id_enable(id);
  logger_id_level_set(id, LOGGER_DEBUG);

  logger_color_set(id, LOGGER_FG_RED, LOGGER_BG_YELLOW, LOGGER_ATTR_RESET);
  logger(id, LOGGER_DEBUG, "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__);
  puts("Other message");
  puts("Other message");
  logger_color_set(id, LOGGER_FG_RED, LOGGER_BG_YELLOW, LOGGER_ATTR_BRIGHT);
  logger(id, LOGGER_DEBUG, "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__);
  puts("Other message");
  puts("Other message");
  logger_color_set(id, LOGGER_FG_RED, LOGGER_BG_YELLOW, LOGGER_ATTR_BLINK);
  logger(id, LOGGER_DEBUG, "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__);
  puts("Other message");
  puts("Other message");
  logger_color_set(id, LOGGER_FG_RED, LOGGER_BG_YELLOW, LOGGER_ATTR_REVERSE);
  logger(id, LOGGER_DEBUG, "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__);
  puts("Other message");
  puts("Other message");
  logger_color_set(id, LOGGER_FG_BLUE, LOGGER_BG_GREEN, LOGGER_ATTR_RESET);
  logger(id, LOGGER_DEBUG, "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__);
  puts("Other message");
  puts("Other message");
  logger_color_set(id, LOGGER_FG_YELLOW, LOGGER_BG_BLACK, LOGGER_ATTR_RESET);
  logger(id, LOGGER_DEBUG, "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__);
  puts("Other message");
  puts("Other message");
  logger_color_set(id, LOGGER_FG_GREEN, LOGGER_BG_WHITE, LOGGER_ATTR_RESET);
  logger(id, LOGGER_DEBUG, "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__);
  puts("Other message");
  puts("Other message");
  logger_color_reset(id);
  logger(id, LOGGER_DEBUG, "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__);
  puts("Other message");
  puts("Other message");

  logger_output_deregister(stdout);
  logger_id_release(id);

  printf("Ending test %d ....\n", test);

  return(0);
}


/* vim: set cino=t0,c0,(0,u0 et fen fdm=syntax fo=tcroqnl1 sw=2 sts=2 ts=8 : */
