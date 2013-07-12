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
  int                test = 0;
  FILE               *logger_stream;
  FILE               *logger_stream2;
  logger_level_t     level;
  logger_prefix_t    prefix;
  logger_id_t        id  = logger_id_unknown;
  logger_id_t        id2 = logger_id_unknown;
  logger_text_attr_t attr;
  logger_text_bg_t   bg_color;
  logger_text_fg_t   fg_color;

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- check logger initialization test function ....\n", test);
  assert(logger_false == logger_is_initialized());
  assert(LOGGER_OK == logger_init());
  assert(logger_true == logger_is_initialized());

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
  printf("Starting test %d -- color enable functions ....\n", test);
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

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- id not registered ....\n", test);

  puts("No log message will be shown because id is not registered");

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));

  assert(LOGGER_ERR_ID_UNKNOWN == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  assert(LOGGER_ERR_ID_UNKNOWN == logger(id, LOGGER_INFO,    "test %d - id %d - LOGGER_INFO    in line %d\n", test, id, __LINE__));
  assert(LOGGER_ERR_ID_UNKNOWN == logger(id, LOGGER_NOTICE,  "test %d - id %d - LOGGER_NOTICE  in line %d\n", test, id, __LINE__));
  assert(LOGGER_ERR_ID_UNKNOWN == logger(id, LOGGER_WARNING, "test %d - id %d - LOGGER_WARNING in line %d\n", test, id, __LINE__));
  assert(LOGGER_ERR_ID_UNKNOWN == logger(id, LOGGER_ERR,     "test %d - id %d - LOGGER_ERR     in line %d\n", test, id, __LINE__));
  assert(LOGGER_ERR_ID_UNKNOWN == logger(id, LOGGER_CRIT,    "test %d - id %d - LOGGER_CRIT    in line %d\n", test, id, __LINE__));
  assert(LOGGER_ERR_ID_UNKNOWN == logger(id, LOGGER_ALERT,   "test %d - id %d - LOGGER_ALERT   in line %d\n", test, id, __LINE__));
  assert(LOGGER_ERR_ID_UNKNOWN == logger(id, LOGGER_EMERG,   "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));

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
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));
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

  puts("No log messages will be shown because id level is not set ( -> level is UNKNOWN by default)");

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));
  id = logger_id_request("logger_test_id");
  assert(logger_false == logger_id_is_enabled(id));
  assert(LOGGER_OK == logger_id_enable(id));
  assert(logger_true == logger_id_is_enabled(id));
  assert(LOGGER_OK == logger_id_disable(id));
  assert(logger_false == logger_id_is_enabled(id));
  assert(LOGGER_OK == logger_id_enable(id));
  assert(logger_true == logger_id_is_enabled(id));

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
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));
  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));

  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  assert(logger_true == logger_is_enabled());
  assert(LOGGER_OK == logger_disable());
  assert(logger_false == logger_is_enabled());
  puts("Logging disabled");
  assert(LOGGER_ERR_ID_UNKNOWN == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  assert(logger_false == logger_is_enabled());
  assert(LOGGER_OK == logger_enable());
  assert(logger_true == logger_is_enabled());
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
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));
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

  puts("Log messages of id and id2 will be shown");

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));
  id  = logger_id_request("logger_test_id");
  id2 = logger_id_request("logger_test_id");
  assert(id == id2);
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));
  assert(LOGGER_OK == logger_id_level_set(id2, LOGGER_DEBUG));

  assert(LOGGER_OK == logger_id_enable(id));

  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id2, LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));

  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_id_release(id));
  assert(LOGGER_OK == logger_id_release(id2));

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- all possible prefixes ....\n", test);

  puts("All possible prefixes will be shown");

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));
  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));
  assert(LOGGER_OK == logger_color_prefix_enable());


  puts("Format: LOGGER_PREFIX_EMPTY");
  assert(LOGGER_OK == logger_id_prefix_set(id, LOGGER_PREFIX_EMPTY));
  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));
  puts("Format: LOGGER_PREFIX_FILE_FUNCTION_LINE");
  assert(LOGGER_OK == logger_id_prefix_set(id, LOGGER_PREFIX_FILE_FUNCTION_LINE));
  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));
  puts("Format: LOGGER_PREFIX_FILE_LINE");
  assert(LOGGER_OK == logger_id_prefix_set(id, LOGGER_PREFIX_FILE_LINE));
  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));
  puts("Format: LOGGER_PREFIX_FUNCTION_LINE");
  assert(LOGGER_OK == logger_id_prefix_set(id, LOGGER_PREFIX_FUNCTION_LINE));
  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));
  puts("Format: LOGGER_PREFIX_NAME");
  assert(LOGGER_OK == logger_id_prefix_set(id, LOGGER_PREFIX_NAME));
  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));
  puts("Format: LOGGER_PREFIX_NAME_FILE_FUNCTION_LINE");
  assert(LOGGER_OK == logger_id_prefix_set(id, LOGGER_PREFIX_NAME_FILE_FUNCTION_LINE));
  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));
  puts("Format: LOGGER_PREFIX_NAME_FILE_LINE");
  assert(LOGGER_OK == logger_id_prefix_set(id, LOGGER_PREFIX_NAME_FILE_LINE));
  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));
  puts("Format: LOGGER_PREFIX_NAME_FUNCTION_LINE");
  assert(LOGGER_OK == logger_id_prefix_set(id, LOGGER_PREFIX_NAME_FUNCTION_LINE));
  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));
  puts("Format: LOGGER_PREFIX_NAME_LEVEL");
  assert(LOGGER_OK == logger_id_prefix_set(id, LOGGER_PREFIX_NAME_LEVEL));
  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));
  puts("Format: LOGGER_PREFIX_NAME_LEVEL_FILE_FUNCTION_LINE");
  assert(LOGGER_OK == logger_id_prefix_set(id, LOGGER_PREFIX_NAME_LEVEL_FILE_FUNCTION_LINE));
  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));
  puts("Format: LOGGER_PREFIX_NAME_LEVEL_FILE_LINE");
  assert(LOGGER_OK == logger_id_prefix_set(id, LOGGER_PREFIX_NAME_LEVEL_FILE_LINE));
  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));
  puts("Format: LOGGER_PREFIX_NAME_LEVEL_FUNCTION_LINE");
  assert(LOGGER_OK == logger_id_prefix_set(id, LOGGER_PREFIX_NAME_LEVEL_FUNCTION_LINE));
  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));

  assert(LOGGER_OK == logger_color_prefix_disable());
  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_id_release(id));

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- different prefixes for id and id2 ....\n", test);

  puts("Different prefixes for id and id2 will be shown");

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));
  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));
  id2 = logger_id_request("logger_test_id2");
  assert(LOGGER_OK == logger_id_enable(id2));
  assert(LOGGER_OK == logger_id_level_set(id2, LOGGER_DEBUG));
  assert(LOGGER_OK == logger_id_prefix_set(id2, LOGGER_PREFIX_FUNCTION_LINE));

  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id2, LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));

  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_id_release(id));
  assert(LOGGER_OK == logger_id_release(id2));

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- id with global and id specific output ....\n", test);

  puts("Message will be shown two times, once for the global output, once for the id specific one");

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));
  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));
  assert(LOGGER_OK == logger_id_output_register(id, stdout));
  assert(LOGGER_OK == logger_id_output_level_set(id, stdout, LOGGER_DEBUG));

  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));

  assert(LOGGER_OK == logger_id_output_deregister(id, stdout));
  assert(LOGGER_OK == logger_id_release(id));
  assert(LOGGER_OK == logger_output_deregister(stdout));

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- register test functions ....\n", test);

  puts("No output just test functions which check for registered outputs");

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

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- only id specific output for id, none for id2 ....\n", test);

  puts("No global output stream is registered and only id gets its id specific one, id2 has no output");

  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));
  assert(LOGGER_OK == logger_id_output_register(id, stdout));
  assert(LOGGER_OK == logger_id_output_level_set(id, stdout, LOGGER_DEBUG));
  id2 = logger_id_request("logger_test_id2");
  assert(LOGGER_OK == logger_id_enable(id2));
  assert(LOGGER_OK == logger_id_level_set(id2, LOGGER_DEBUG));

  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id2, LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));

  assert(LOGGER_OK == logger_id_output_deregister(id, stdout));
  assert(LOGGER_OK == logger_id_release(id));
  assert(LOGGER_OK == logger_id_release(id2));

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- modify prefixe for id ....\n", test);

  puts("Change prefix for id and restore previous value");

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));
  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));

  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));

  prefix = logger_id_prefix_get(id);
  assert(LOGGER_OK == logger_id_prefix_set(id, LOGGER_PREFIX_FUNCTION_LINE));

  assert(LOGGER_OK == logger(id, LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));

  assert(LOGGER_OK == logger_id_prefix_set(id, prefix));

  assert(LOGGER_OK == logger(id, LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));

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
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));
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
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));
  assert(LOGGER_OK == logger_output_register(stderr));
  assert(LOGGER_OK == logger_output_level_set(stderr, LOGGER_DEBUG));
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
  printf("Starting test %d -- output to stdout, global and id specific file ....\n", test);

  puts("All log messages will be printed to stdout and appended to file \"logfile\", messages to id2 additionally to \"logfile2\"");

  logger_stream = fopen("logfile", "w");
  assert(LOGGER_OK == logger_output_register(logger_stream));
  assert(LOGGER_OK == logger_output_level_set(logger_stream, LOGGER_DEBUG));

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_ERR));

  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));

  id2 = logger_id_request("logger_test_id2");
  assert(LOGGER_OK == logger_id_enable(id2));
  assert(LOGGER_OK == logger_id_level_set(id2, LOGGER_DEBUG));

  logger_stream2 = fopen("logfile2", "w");
  assert(LOGGER_OK == logger_id_output_register(id2, logger_stream2));
  assert(LOGGER_OK == logger_id_output_level_set(id2, logger_stream2, LOGGER_NOTICE));

  assert(LOGGER_OK == logger(id,  LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id,  __LINE__));
  assert(LOGGER_OK == logger(id,  LOGGER_INFO,    "test %d - id %d - LOGGER_INFO    in line %d\n", test, id,  __LINE__));
  assert(LOGGER_OK == logger(id,  LOGGER_NOTICE,  "test %d - id %d - LOGGER_NOTICE  in line %d\n", test, id,  __LINE__));
  assert(LOGGER_OK == logger(id,  LOGGER_WARNING, "test %d - id %d - LOGGER_WARNING in line %d\n", test, id,  __LINE__));
  assert(LOGGER_OK == logger(id,  LOGGER_ERR,     "test %d - id %d - LOGGER_ERR     in line %d\n", test, id,  __LINE__));
  assert(LOGGER_OK == logger(id,  LOGGER_CRIT,    "test %d - id %d - LOGGER_CRIT    in line %d\n", test, id,  __LINE__));
  assert(LOGGER_OK == logger(id,  LOGGER_ALERT,   "test %d - id %d - LOGGER_ALERT   in line %d\n", test, id,  __LINE__));
  assert(LOGGER_OK == logger(id,  LOGGER_EMERG,   "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id,  __LINE__));
  assert(LOGGER_OK == logger(id2, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id2, __LINE__));
  assert(LOGGER_OK == logger(id2, LOGGER_INFO,    "test %d - id %d - LOGGER_INFO    in line %d\n", test, id2, __LINE__));
  assert(LOGGER_OK == logger(id2, LOGGER_NOTICE,  "test %d - id %d - LOGGER_NOTICE  in line %d\n", test, id2, __LINE__));
  assert(LOGGER_OK == logger(id2, LOGGER_WARNING, "test %d - id %d - LOGGER_WARNING in line %d\n", test, id2, __LINE__));
  assert(LOGGER_OK == logger(id2, LOGGER_ERR,     "test %d - id %d - LOGGER_ERR     in line %d\n", test, id2, __LINE__));
  assert(LOGGER_OK == logger(id2, LOGGER_CRIT,    "test %d - id %d - LOGGER_CRIT    in line %d\n", test, id2, __LINE__));
  assert(LOGGER_OK == logger(id2, LOGGER_ALERT,   "test %d - id %d - LOGGER_ALERT   in line %d\n", test, id2, __LINE__));
  assert(LOGGER_OK == logger(id2, LOGGER_EMERG,   "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id2, __LINE__));

  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_output_deregister(logger_stream));
  assert(LOGGER_OK == logger_id_release(id));
  assert(LOGGER_OK == logger_id_output_deregister(id2, logger_stream2));
  assert(LOGGER_OK == logger_id_release(id2));
  fclose(logger_stream);
  fclose(logger_stream2);

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
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));
  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_WARNING));

  if (logger_id_is_enabled(id)) {
    printf("Logging level is %s\n", logger_level_name_get(logger_id_level_get(id)));
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

    printf("Logging level is %s\n", logger_level_name_get(logger_id_level_get(id)));
    assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_INFO,    "test %d - id %d - LOGGER_INFO    in line %d\n", test, id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_NOTICE,  "test %d - id %d - LOGGER_NOTICE  in line %d\n", test, id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_WARNING, "test %d - id %d - LOGGER_WARNING in line %d\n", test, id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_ERR,     "test %d - id %d - LOGGER_ERR     in line %d\n", test, id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_CRIT,    "test %d - id %d - LOGGER_CRIT    in line %d\n", test, id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_ALERT,   "test %d - id %d - LOGGER_ALERT   in line %d\n", test, id, __LINE__));
    assert(LOGGER_OK == logger(id, LOGGER_EMERG,   "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));

    assert(LOGGER_OK == logger_id_level_set(id, level));

    printf("Logging level is restored to %s\n", logger_level_name_get(logger_id_level_get(id)));
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
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));
  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));
  assert(LOGGER_OK == logger_color_set(id, LOGGER_FG_GREEN, LOGGER_BG_BLACK, LOGGER_ATTR_RESET));

  assert(LOGGER_OK == logger_color_prefix_disable());
  assert(LOGGER_OK == logger_color_message_disable());
  assert(LOGGER_OK == logger(id, LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_EMERG, " - continued 1"));
  assert(LOGGER_OK == logger(id, LOGGER_EMERG, " - continued 2"));
  assert(LOGGER_OK == logger(id, LOGGER_EMERG, " - continued 3\n"));
  assert(LOGGER_OK == logger_color_prefix_enable());
  assert(LOGGER_OK == logger_color_message_disable());
  assert(LOGGER_OK == logger(id, LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_EMERG, " - continued 1"));
  assert(LOGGER_OK == logger(id, LOGGER_EMERG, " - continued 2"));
  assert(LOGGER_OK == logger(id, LOGGER_EMERG, " - continued 3\n"));
  assert(LOGGER_OK == logger_color_prefix_disable());
  assert(LOGGER_OK == logger_color_message_enable());
  assert(LOGGER_OK == logger(id, LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_EMERG, " - continued 1"));
  assert(LOGGER_OK == logger(id, LOGGER_EMERG, " - continued 2"));
  assert(LOGGER_OK == logger(id, LOGGER_EMERG, " - continued 3\n"));
  assert(LOGGER_OK == logger_color_prefix_enable());
  assert(LOGGER_OK == logger_color_message_enable());
  assert(LOGGER_OK == logger(id, LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_EMERG, " - continued 1"));
  assert(LOGGER_OK == logger(id, LOGGER_EMERG, " - continued 2"));
  assert(LOGGER_OK == logger(id, LOGGER_EMERG, " - continued 3\n"));

  assert(LOGGER_OK == logger_color_message_disable());
  assert(LOGGER_OK == logger_color_prefix_disable());
  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_id_release(id));

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- messages with multiple '\\n' ....\n", test);

  puts("Messages will be written in multiple lines but all with correct prefix");

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));
  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));

  assert(LOGGER_OK == logger(id, LOGGER_DEBUG, "test %d - id %d - LOGGER_DEBUG   in line %d\nnext line\n\nafter an empty line\n", test, id, __LINE__));

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
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));
  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_color_message_enable());
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

  assert(LOGGER_OK == logger_color_message_disable());
  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_id_release(id));

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- show prefix colors  ....\n", test);

  puts("All messages have a different color for their prefix");

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));
  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));
  assert(LOGGER_OK == logger_color_message_enable());
  assert(LOGGER_OK == logger_color_set(id, LOGGER_FG_GREEN, LOGGER_BG_BLACK, LOGGER_ATTR_RESET));

  puts("All messages have the same color");
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_INFO,    "test %d - id %d - LOGGER_INFO    in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_NOTICE,  "test %d - id %d - LOGGER_NOTICE  in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_WARNING, "test %d - id %d - LOGGER_WARNING in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_ERR,     "test %d - id %d - LOGGER_ERR     in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_CRIT,    "test %d - id %d - LOGGER_CRIT    in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_ALERT,   "test %d - id %d - LOGGER_ALERT   in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_EMERG,   "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));
  puts("All messages have a level dependent  color for their prefix");
  assert(LOGGER_OK == logger_color_prefix_enable());
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_INFO,    "test %d - id %d - LOGGER_INFO    in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_NOTICE,  "test %d - id %d - LOGGER_NOTICE  in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_WARNING, "test %d - id %d - LOGGER_WARNING in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_ERR,     "test %d - id %d - LOGGER_ERR     in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_CRIT,    "test %d - id %d - LOGGER_CRIT    in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_ALERT,   "test %d - id %d - LOGGER_ALERT   in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id, LOGGER_EMERG,   "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));

  assert(LOGGER_OK == logger_color_prefix_disable());
  assert(LOGGER_OK == logger_color_message_disable());
  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_id_release(id));

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- enable/disable output colors per output stream ....\n", test);

  puts("Coloring of messages could be enabled/disabled for each output stream");

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));
  assert(LOGGER_OK == logger_output_register(stderr));
  assert(LOGGER_OK == logger_output_level_set(stderr, LOGGER_DEBUG));
  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));
  assert(LOGGER_OK == logger_color_message_enable());
  assert(LOGGER_OK == logger_color_set(id, LOGGER_FG_GREEN, LOGGER_BG_BLACK, LOGGER_ATTR_RESET));

  puts("Message on STDOUT and STDERR both use color");
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  puts("Only message on STDOUT uses color");
  assert(LOGGER_OK == logger_output_color_enable(stdout));
  assert(LOGGER_OK == logger_output_color_disable(stderr));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  puts("Only message on STDERR uses color");
  assert(LOGGER_OK == logger_output_color_disable(stdout));
  assert(LOGGER_OK == logger_output_color_enable(stderr));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  puts("Message on STDOUT and STDERR both use color");
  assert(LOGGER_OK == logger_output_color_enable(stdout));
  assert(LOGGER_OK == logger_output_color_enable(stderr));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));

  assert(LOGGER_OK == logger_color_prefix_disable());
  assert(LOGGER_OK == logger_color_message_disable());
  assert(LOGGER_OK == logger_output_deregister(stderr));
  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_id_release(id));

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- enable/disable output colors per id output stream ....\n", test);

  puts("Coloring of messages could be enabled/disabled for each id specific output stream");

  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));
  assert(LOGGER_OK == logger_color_message_enable());
  assert(LOGGER_OK == logger_color_set(id, LOGGER_FG_GREEN, LOGGER_BG_BLACK, LOGGER_ATTR_RESET));
  assert(LOGGER_OK == logger_id_output_register(id, stdout));
  assert(LOGGER_OK == logger_id_output_level_set(id, stdout, LOGGER_DEBUG));
  assert(LOGGER_OK == logger_id_output_register(id, stderr));
  assert(LOGGER_OK == logger_id_output_level_set(id, stderr, LOGGER_DEBUG));

  puts("Message on STDOUT and STDERR both use color");
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  puts("Only message on STDOUT uses color");
  assert(LOGGER_OK == logger_id_output_color_enable(id, stdout));
  assert(LOGGER_OK == logger_id_output_color_disable(id, stderr));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  puts("Only message on STDERR uses color");
  assert(LOGGER_OK == logger_id_output_color_disable(id, stdout));
  assert(LOGGER_OK == logger_id_output_color_enable(id, stderr));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  puts("Message on STDOUT and STDERR both use color");
  assert(LOGGER_OK == logger_id_output_color_enable(id, stdout));
  assert(LOGGER_OK == logger_id_output_color_enable(id, stderr));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));

  assert(LOGGER_OK == logger_color_prefix_disable());
  assert(LOGGER_OK == logger_color_message_disable());
  assert(LOGGER_OK == logger_id_output_deregister(id, stderr));
  assert(LOGGER_OK == logger_id_output_deregister(id, stdout));
  assert(LOGGER_OK == logger_id_release(id));

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- show color combinations\n", test);

  puts("Show all possible color combinations");

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));
  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));
  assert(LOGGER_OK == logger_color_message_enable());

  for (attr = LOGGER_ATTR_RESET ; attr <= LOGGER_ATTR_HIDDEN ; attr++)
  {
    for (bg_color = LOGGER_BG_BLACK ; bg_color <= LOGGER_BG_WHITE ; bg_color++)
    {
      for (fg_color = LOGGER_FG_BLACK ; fg_color <= LOGGER_FG_WHITE ; fg_color++)
      {
        logger_color_set(id, LOGGER_FG_WHITE, LOGGER_BG_BLACK, LOGGER_ATTR_RESET);
        logger(id, LOGGER_DEBUG, "foreground: %d, background: %d, attr: %d  >>>>", fg_color, bg_color, attr);
        logger_color_set(id, fg_color, bg_color, attr);
        logger(id, LOGGER_DEBUG, "TEST TEST TEST TEST TEST");
        logger_color_set(id, LOGGER_FG_WHITE, LOGGER_BG_BLACK, LOGGER_ATTR_RESET);
        logger(id, LOGGER_DEBUG, "<<<<\n");
      }
    }
  }

  assert(LOGGER_OK == logger_color_message_disable());
  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_id_release(id));

  printf("Ending test %d ....\n", test);

  return(0);
}


/* vim: set cino=t0,c0,(0,u0 et fen fdm=syntax fo=tcroqnl1 sw=2 sts=2 ts=8 : */
