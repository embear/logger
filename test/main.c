/***************************************************************************//**
 *     __
 *    / /___  ____ _____ ____  _____
 *   / / __ \/ __ `/ __ `/ _ \/ ___/
 *  / / /_/ / /_/ / /_/ /  __/ /
 * /_/\____/\__, /\__, /\___/_/
 *         /____//____/
 *
 * \file   main.c
 *
 * \brief  Logging facility for C.
 * \author Markus Braun
 * \date   2010-10-13
 ******************************************************************************/
#include <stdio.h>
#include <assert.h>
#include "logger.h"

int main(int  argc,
         char *argv[])
{
  int            test = 0;
  FILE           *logger_stream;
  logger_level_t level;
  logger_id_t    id  = logger_id_unknown;
  logger_id_t    id2 = logger_id_unknown;

  assert(LOGGER_OK == logger_init());

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- check logger version number %s ....\n", test, LOGGER_VERSION_STRING);
  assert(LOGGER_VERSION == logger_version());

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- id not registered ....\n", test);

  puts("No log message will be shown because id is not registered");

  assert(LOGGER_OK == logger_output_register(stdout));

  logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_INFO,    "test %d - id %d - LOGGER_INFO    in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_NOTICE,  "test %d - id %d - LOGGER_NOTICE  in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_WARNING, "test %d - id %d - LOGGER_WARNING in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_ERR,     "test %d - id %d - LOGGER_ERR     in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_CRIT,    "test %d - id %d - LOGGER_CRIT    in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_ALERT,   "test %d - id %d - LOGGER_ALERT   in line %d\n", test, id, __LINE__);
  logger(id, LOGGER_EMERG,   "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__);

  assert(LOGGER_OK == logger_output_deregister(stdout));

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- id not enabled ....\n", test);

  puts("No log message will be shown because id is not enabled");

  assert(LOGGER_OK == logger_output_register(stdout));
  id = logger_id_request("logger_test_id");

  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_INFO,    "test %d - id %d - LOGGER_INFO    in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_NOTICE,  "test %d - id %d - LOGGER_NOTICE  in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_WARNING, "test %d - id %d - LOGGER_WARNING in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_ERR,     "test %d - id %d - LOGGER_ERR     in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_CRIT,    "test %d - id %d - LOGGER_CRIT    in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_ALERT,   "test %d - id %d - LOGGER_ALERT   in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_EMERG,   "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));

  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_id_release(id));

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- level not set ....\n", test);

  puts("All log messages will be shown because id level is not set ( -> level is DEBUG by default)");

  assert(LOGGER_OK == logger_output_register(stdout));
  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));

  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_INFO,    "test %d - id %d - LOGGER_INFO    in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_NOTICE,  "test %d - id %d - LOGGER_NOTICE  in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_WARNING, "test %d - id %d - LOGGER_WARNING in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_ERR,     "test %d - id %d - LOGGER_ERR     in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_CRIT,    "test %d - id %d - LOGGER_CRIT    in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_ALERT,   "test %d - id %d - LOGGER_ALERT   in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_EMERG,   "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));

  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_id_release(id));

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- disable/enable globally ....\n", test);

  puts("Log messages are shown only if globally enabled");

  assert(LOGGER_OK == logger_output_register(stdout));
  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));

  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger_disable());
  puts("Logging disabled");
  logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__);
  assert(LOGGER_OK == logger_enable());
  puts("Logging enabled");
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));

  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_id_release(id));

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- level set to LOGGER_WARNING ....\n", test);

  puts("Only log messages with level above or equal to WARNING will be shown");

  assert(LOGGER_OK == logger_output_register(stdout));
  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_WARNING));

  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_INFO,    "test %d - id %d - LOGGER_INFO    in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_NOTICE,  "test %d - id %d - LOGGER_NOTICE  in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_WARNING, "test %d - id %d - LOGGER_WARNING in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_ERR,     "test %d - id %d - LOGGER_ERR     in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_CRIT,    "test %d - id %d - LOGGER_CRIT    in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_ALERT,   "test %d - id %d - LOGGER_ALERT   in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_EMERG,   "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));

  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_id_release(id));

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- id and id2 are the same ....\n", test);

  puts("log messages of id and id2 will be shown");

  assert(LOGGER_OK == logger_output_register(stdout));
  id = logger_id_request("logger_test_id");
  id2 = logger_id_request("logger_test_id");
  assert(id == id2);

  assert(LOGGER_OK == logger_id_enable(id));

  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id2, LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));

  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_id_release(id));

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- id enabled, id2 not ....\n", test);

  puts("Only log messages of id will be shown");

  assert(LOGGER_OK == logger_output_register(stdout));
  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));

  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id2, LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));

  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_id_release(id));

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- output to stdout and stderr ....\n", test);

  puts("All log messages will be printed to stdout and stderr");

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_register(stderr));
  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_ERR));

  assert(LOGGER_OK == logger(id, LOGGER_DEBUG, "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_ERR,   "test %d - id %d - LOGGER_ERR     in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));

  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_output_deregister(stderr));
  assert(LOGGER_OK == logger_id_release(id));

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- output to stdout and file ....\n", test);

  puts("All log messages will be printed to stdout and appended to file \"logfile\"");

  logger_stream = fopen("logfile", "w");
  assert(LOGGER_OK == logger_output_register(logger_stream));
  assert(LOGGER_OK == logger_output_level_set(logger_stream, LOGGER_DEBUG));

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_ERR));

  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));

  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_INFO,    "test %d - id %d - LOGGER_INFO    in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_NOTICE,  "test %d - id %d - LOGGER_NOTICE  in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_WARNING, "test %d - id %d - LOGGER_WARNING in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_ERR,     "test %d - id %d - LOGGER_ERR     in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_CRIT,    "test %d - id %d - LOGGER_CRIT    in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_ALERT,   "test %d - id %d - LOGGER_ALERT   in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_EMERG,   "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));

  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_output_deregister(logger_stream));
  fclose(logger_stream);
  assert(LOGGER_OK == logger_id_release(id));

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- using control functions ....\n", test);

  puts("Changing and restoring logging level");

  assert(LOGGER_OK == logger_output_register(stdout));
  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_WARNING));

  if (logger_id_is_enabled(id)) {
    puts("Logging level is WARNING");
    assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_INFO,    "test %d - id %d - LOGGER_INFO    in line %d\n", test, id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_NOTICE,  "test %d - id %d - LOGGER_NOTICE  in line %d\n", test, id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_WARNING, "test %d - id %d - LOGGER_WARNING in line %d\n", test, id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_ERR,     "test %d - id %d - LOGGER_ERR     in line %d\n", test, id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_CRIT,    "test %d - id %d - LOGGER_CRIT    in line %d\n", test, id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_ALERT,   "test %d - id %d - LOGGER_ALERT   in line %d\n", test, id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_EMERG,   "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));

    level = logger_id_level_get(id);
    assert(LOGGER_OK == logger_id_level_set(id, LOGGER_ALERT));

    puts("Logging level is changed to ALERT");
    assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_INFO,    "test %d - id %d - LOGGER_INFO    in line %d\n", test, id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_NOTICE,  "test %d - id %d - LOGGER_NOTICE  in line %d\n", test, id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_WARNING, "test %d - id %d - LOGGER_WARNING in line %d\n", test, id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_ERR,     "test %d - id %d - LOGGER_ERR     in line %d\n", test, id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_CRIT,    "test %d - id %d - LOGGER_CRIT    in line %d\n", test, id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_ALERT,   "test %d - id %d - LOGGER_ALERT   in line %d\n", test, id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_EMERG,   "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));

    assert(LOGGER_OK == logger_id_level_set(id, level));

    puts("Logging level is restored");
    assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_INFO,    "test %d - id %d - LOGGER_INFO    in line %d\n", test, id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_NOTICE,  "test %d - id %d - LOGGER_NOTICE  in line %d\n", test, id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_WARNING, "test %d - id %d - LOGGER_WARNING in line %d\n", test, id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_ERR,     "test %d - id %d - LOGGER_ERR     in line %d\n", test, id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_CRIT,    "test %d - id %d - LOGGER_CRIT    in line %d\n", test, id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_ALERT,   "test %d - id %d - LOGGER_ALERT   in line %d\n", test, id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_EMERG,   "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));
  }

  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_id_release(id));

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- continued messages ....\n", test);

  puts("Messages will be all in one line");

  assert(LOGGER_OK == logger_output_register(stdout));
  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));

  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   " - continued 1"));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   " - continued 2"));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   " - continued 3\n"));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   " - continued 1"));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   " - continued 2"));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   " - continued 3\n"));

  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_id_release(id));

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- colors in text output ....\n", test);

  puts("Print log messages in different colors");

  assert(LOGGER_OK == logger_output_register(stdout));
  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));

  assert(LOGGER_OK == logger_color_set(id, LOGGER_FG_RED, LOGGER_BG_BLACK, LOGGER_ATTR_RESET));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG, "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  puts("Other message");
  puts("Other message");
  assert(LOGGER_OK == logger_color_set(id, LOGGER_FG_RED, LOGGER_BG_BLACK, LOGGER_ATTR_BRIGHT));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG, "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  puts("Other message");
  puts("Other message");
  assert(LOGGER_OK == logger_color_set(id, LOGGER_FG_RED, LOGGER_BG_BLACK, LOGGER_ATTR_BLINK));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG, "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  puts("Other message");
  puts("Other message");
  assert(LOGGER_OK == logger_color_set(id, LOGGER_FG_RED, LOGGER_BG_BLACK, LOGGER_ATTR_REVERSE));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG, "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  puts("Other message");
  puts("Other message");
  assert(LOGGER_OK == logger_color_set(id, LOGGER_FG_BLUE, LOGGER_BG_BLACK, LOGGER_ATTR_RESET));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG, "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  puts("Other message");
  puts("Other message");
  assert(LOGGER_OK == logger_color_set(id, LOGGER_FG_YELLOW, LOGGER_BG_BLACK, LOGGER_ATTR_RESET));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG, "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  puts("Other message");
  puts("Other message");
  assert(LOGGER_OK == logger_color_set(id, LOGGER_FG_GREEN, LOGGER_BG_BLACK, LOGGER_ATTR_RESET));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG, "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  puts("Other message");
  puts("Other message");
  assert(LOGGER_OK == logger_color_reset(id));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG, "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  puts("Other message");
  puts("Other message");

  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_id_release(id));

  printf("Ending test %d ....\n", test);

  return(0);
}


/* vim: set cino=t0,c0,(0,u0 et fen fdm=syntax fo=tcroqnl1 sw=2 sts=2 ts=8 : */
