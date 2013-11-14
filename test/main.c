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
#include <stdint.h>
#include <inttypes.h>
#include "logger.h"

void test_printer(const char * string)
{
  printf("test_printer: %s", string);
}
logger_return_t logger_wrapper(logger_id_t    id,
                               logger_level_t level,
                               const char     *format,
                               ...)
{
  logger_return_t ret;
  va_list         argp;

  va_start(argp, format);
  ret = logger_va(id, level, format, argp);
  va_end(argp);

  return(ret);
}



int main(int  argc,
         char *argv[])
{
  int                test = 0;
  uint32_t           tmp_uint32 = 32;
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
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
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
  printf("Starting test %d -- level mask set to LOGGER_WARNING|LOGGER_ERR|LOGGER_CRIT ....\n", test);

  puts("Only log messages with level WARNING or ERR or CRIT will be shown");

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));
  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_mask_set(id, LOGGER_WARNING|LOGGER_ERR|LOGGER_CRIT));

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
  printf("Starting test %d -- output to stdout and stderr with mask ....\n", test);

  puts("Message with severity INFO is printed to stdout, message with severity ERR is printed to stderr\n");

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_mask_set(stdout, LOGGER_INFO));
  assert(LOGGER_OK == logger_output_register(stderr));
  assert(LOGGER_OK == logger_output_level_mask_set(stderr, LOGGER_ERR));
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
  assert(LOGGER_OK == logger_output_deregister(stderr));
  assert(LOGGER_OK == logger_id_release(id));

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- check output generation ....\n", test);

  puts("Check if an ID / level combination would generate an output");

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_WARNING));
  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_mask_set(id, LOGGER_DEBUG|LOGGER_ERR|LOGGER_CRIT));

  assert(logger_false == logger_id_generates_output(id, LOGGER_DEBUG));
  assert(logger_false == logger_id_generates_output(id, LOGGER_INFO));
  assert(logger_false == logger_id_generates_output(id, LOGGER_NOTICE));
  assert(logger_false == logger_id_generates_output(id, LOGGER_WARNING));
  assert(logger_true  == logger_id_generates_output(id, LOGGER_ERR));
  assert(logger_true  == logger_id_generates_output(id, LOGGER_CRIT));
  assert(logger_false == logger_id_generates_output(id, LOGGER_ALERT));
  assert(logger_false == logger_id_generates_output(id, LOGGER_EMERG));

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
  assert(LOGGER_OK == logger(id2, LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id2, __LINE__));

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


  puts("Format: \"\"");
  assert(LOGGER_OK == logger_id_prefix_set(id, LOGGER_PFX_EMPTY));
  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));

  puts("Format: FUNCTION");
  assert(LOGGER_OK == logger_id_prefix_set(id, (LOGGER_PFX_FUNCTION)));
  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));

  puts("Format: FUNCTION:LINE");
  assert(LOGGER_OK == logger_id_prefix_set(id, (LOGGER_PFX_FUNCTION | LOGGER_PFX_LINE)));
  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));

  puts("Format: FILE:FUNCTION:LINE");
  assert(LOGGER_OK == logger_id_prefix_set(id, (LOGGER_PFX_FILE | LOGGER_PFX_FUNCTION | LOGGER_PFX_LINE)));
  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));

  puts("Format: LEVEL:FILE:FUNCTION:LINE");
  assert(LOGGER_OK == logger_id_prefix_set(id, (LOGGER_PFX_LEVEL | LOGGER_PFX_FILE | LOGGER_PFX_FUNCTION | LOGGER_PFX_LINE)));
  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));

  puts("Format: NAME:LEVEL:FILE:FUNCTION:LINE");
  assert(LOGGER_OK == logger_id_prefix_set(id, (LOGGER_PFX_NAME | LOGGER_PFX_LEVEL | LOGGER_PFX_FILE | LOGGER_PFX_FUNCTION | LOGGER_PFX_LINE)));
  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));

  puts("Format: DATE:NAME:LEVEL:FILE:FUNCTION:LINE");
  assert(LOGGER_OK == logger_id_prefix_set(id, (LOGGER_PFX_DATE | LOGGER_PFX_NAME | LOGGER_PFX_LEVEL | LOGGER_PFX_FILE | LOGGER_PFX_FUNCTION | LOGGER_PFX_LINE)));
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
  assert(LOGGER_OK == logger_id_prefix_set(id2, (LOGGER_PFX_FUNCTION | LOGGER_PFX_LINE)));

  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id2, LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id2, __LINE__));

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

  puts("Message will be shown only once because outputs are merged and made unique");

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
  assert(LOGGER_OK == logger(id2, LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id2, __LINE__));

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
  assert(LOGGER_OK == logger_id_prefix_set(id, (LOGGER_PFX_FUNCTION | LOGGER_PFX_LINE)));

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
  assert(LOGGER_ERR_ID_UNKNOWN == logger(id2, LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id2, __LINE__));

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
  assert(LOGGER_OK == logger_id_color_console_set(id, LOGGER_FG_GREEN, LOGGER_BG_UNCHANGED, LOGGER_ATTR_RESET));

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

  assert(LOGGER_OK == logger_id_color_console_set(id, LOGGER_FG_RED, LOGGER_BG_UNCHANGED, LOGGER_ATTR_RESET));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG, "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  puts("Other message");
  puts("Other message");
  assert(LOGGER_OK == logger_id_color_console_set(id, LOGGER_FG_RED, LOGGER_BG_UNCHANGED, LOGGER_ATTR_BRIGHT));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG, "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  puts("Other message");
  puts("Other message");
  assert(LOGGER_OK == logger_id_color_console_set(id, LOGGER_FG_RED, LOGGER_BG_UNCHANGED, LOGGER_ATTR_BLINK));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG, "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  puts("Other message");
  puts("Other message");
  assert(LOGGER_OK == logger_id_color_console_set(id, LOGGER_FG_RED, LOGGER_BG_UNCHANGED, LOGGER_ATTR_REVERSE));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG, "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  puts("Other message");
  puts("Other message");
  assert(LOGGER_OK == logger_id_color_console_set(id, LOGGER_FG_BLUE, LOGGER_BG_UNCHANGED, LOGGER_ATTR_RESET));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG, "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  puts("Other message");
  puts("Other message");
  assert(LOGGER_OK == logger_id_color_console_set(id, LOGGER_FG_YELLOW, LOGGER_BG_UNCHANGED, LOGGER_ATTR_RESET));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG, "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  puts("Other message");
  puts("Other message");
  assert(LOGGER_OK == logger_id_color_console_set(id, LOGGER_FG_GREEN, LOGGER_BG_UNCHANGED, LOGGER_ATTR_RESET));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG, "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  puts("Other message");
  puts("Other message");
  assert(LOGGER_OK == logger_id_color_reset(id));
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
  assert(LOGGER_OK == logger_id_color_console_set(id, LOGGER_FG_GREEN, LOGGER_BG_UNCHANGED, LOGGER_ATTR_RESET));
  assert(LOGGER_OK == logger_color_message_enable());

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
  printf("Starting test %d -- show prefix colors with custom color strings  ....\n", test);

  puts("All messages have a different color for their prefix");

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));
  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));
  assert(LOGGER_OK == logger_id_color_console_set(id, LOGGER_FG_GREEN, LOGGER_BG_UNCHANGED, LOGGER_ATTR_RESET));
  assert(LOGGER_OK == logger_color_message_enable());
  assert(LOGGER_OK == logger_color_prefix_console_set(LOGGER_UNKNOWN, LOGGER_FG_UNCHANGED,  LOGGER_BG_UNCHANGED, LOGGER_ATTR_RESET));
  assert(LOGGER_OK == logger_color_prefix_console_set(LOGGER_DEBUG,   LOGGER_FG_UNCHANGED,  LOGGER_BG_UNCHANGED, LOGGER_ATTR_DIM));
  assert(LOGGER_OK == logger_color_prefix_console_set(LOGGER_INFO,    LOGGER_FG_UNCHANGED,  LOGGER_BG_UNCHANGED, LOGGER_ATTR_RESET));
  assert(LOGGER_OK == logger_color_prefix_console_set(LOGGER_NOTICE,  LOGGER_FG_GREEN,      LOGGER_BG_UNCHANGED, LOGGER_ATTR_RESET));
  assert(LOGGER_OK == logger_color_prefix_console_set(LOGGER_WARNING, LOGGER_FG_YELLOW,     LOGGER_BG_UNCHANGED, LOGGER_ATTR_RESET));
  assert(LOGGER_OK == logger_color_prefix_console_set(LOGGER_ERR,     LOGGER_FG_RED,        LOGGER_BG_UNCHANGED, LOGGER_ATTR_RESET));
  assert(LOGGER_OK == logger_color_prefix_console_set(LOGGER_CRIT,    LOGGER_FG_RED,        LOGGER_BG_UNCHANGED, LOGGER_ATTR_BRIGHT));
  assert(LOGGER_OK == logger_color_prefix_console_set(LOGGER_ALERT,   LOGGER_FG_RED,        LOGGER_BG_UNCHANGED, LOGGER_ATTR_BRIGHT | LOGGER_ATTR_REVERSE));
  assert(LOGGER_OK == logger_color_prefix_console_set(LOGGER_EMERG,   LOGGER_FG_RED,        LOGGER_BG_UNCHANGED, LOGGER_ATTR_BRIGHT | LOGGER_ATTR_REVERSE | LOGGER_ATTR_BLINK));

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

  assert(LOGGER_OK == logger_color_prefix_reset());
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
  printf("Starting test %d -- show prefix colors with custom color strings  ....\n", test);

  puts("All messages have a different color for their prefix");

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

  assert(LOGGER_OK == logger_color_prefix_reset());
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
  assert(LOGGER_OK == logger_id_color_console_set(id, LOGGER_FG_GREEN, LOGGER_BG_UNCHANGED, LOGGER_ATTR_RESET));
  assert(LOGGER_OK == logger_color_message_enable());

  puts("Message on STDOUT and STDERR both use color");
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  puts("Only message on STDOUT uses color");
  assert(LOGGER_OK == logger_output_color_enable(stdout));
  assert(LOGGER_OK == logger_output_color_disable(stderr));
  assert(logger_true == logger_output_color_is_enabled(stdout));
  assert(logger_false == logger_output_color_is_enabled(stderr));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  puts("Only message on STDERR uses color");
  assert(LOGGER_OK == logger_output_color_disable(stdout));
  assert(LOGGER_OK == logger_output_color_enable(stderr));
  assert(logger_false == logger_output_color_is_enabled(stdout));
  assert(logger_true == logger_output_color_is_enabled(stderr));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  puts("Message on STDOUT and STDERR both use color");
  assert(LOGGER_OK == logger_output_color_enable(stdout));
  assert(LOGGER_OK == logger_output_color_enable(stderr));
  assert(logger_true == logger_output_color_is_enabled(stdout));
  assert(logger_true == logger_output_color_is_enabled(stderr));
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
  assert(LOGGER_OK == logger_id_color_console_set(id, LOGGER_FG_GREEN, LOGGER_BG_UNCHANGED, LOGGER_ATTR_RESET));
  assert(LOGGER_OK == logger_id_output_register(id, stdout));
  assert(LOGGER_OK == logger_id_output_level_set(id, stdout, LOGGER_DEBUG));
  assert(LOGGER_OK == logger_id_output_register(id, stderr));
  assert(LOGGER_OK == logger_id_output_level_set(id, stderr, LOGGER_DEBUG));
  assert(LOGGER_OK == logger_color_message_enable());

  puts("Message on STDOUT and STDERR both use color");
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  puts("Only message on STDOUT uses color");
  assert(LOGGER_OK == logger_id_output_color_enable(id, stdout));
  assert(LOGGER_OK == logger_id_output_color_disable(id, stderr));
  assert(logger_true == logger_id_output_color_is_enabled(id, stdout));
  assert(logger_false == logger_id_output_color_is_enabled(id, stderr));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  puts("Only message on STDERR uses color");
  assert(LOGGER_OK == logger_id_output_color_disable(id, stdout));
  assert(LOGGER_OK == logger_id_output_color_enable(id, stderr));
  assert(logger_false == logger_id_output_color_is_enabled(id, stdout));
  assert(logger_true == logger_id_output_color_is_enabled(id, stderr));
  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));
  puts("Message on STDOUT and STDERR both use color");
  assert(LOGGER_OK == logger_id_output_color_enable(id, stdout));
  assert(LOGGER_OK == logger_id_output_color_enable(id, stderr));
  assert(logger_true == logger_id_output_color_is_enabled(id, stdout));
  assert(logger_true == logger_id_output_color_is_enabled(id, stderr));
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
  printf("Starting test %d -- user defined function for global output ....\n", test);

  puts("User defined function to handle global output of formatted message");

  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));
  assert(LOGGER_OK == logger_id_color_console_set(id, LOGGER_FG_GREEN, LOGGER_BG_UNCHANGED, LOGGER_ATTR_RESET));
  assert(LOGGER_OK == logger_color_message_enable());
  assert(LOGGER_OK == logger_output_function_register(test_printer));
  assert(logger_true == logger_output_function_is_registered(test_printer));
  assert(LOGGER_OK == logger_output_function_level_set(test_printer, LOGGER_DEBUG));

  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));

  assert(LOGGER_OK == logger_output_function_deregister(test_printer));
  assert(LOGGER_OK == logger_id_release(id));

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- user defined function for id specific output ....\n", test);

  puts("User defined function to handle id specific output of formatted message");

  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));
  assert(LOGGER_OK == logger_id_color_console_set(id, LOGGER_FG_GREEN, LOGGER_BG_UNCHANGED, LOGGER_ATTR_RESET));
  assert(LOGGER_OK == logger_color_message_enable());
  assert(LOGGER_OK == logger_id_output_function_register(id, test_printer));
  assert(logger_true == logger_id_output_function_is_registered(id, test_printer));
  assert(LOGGER_OK == logger_id_output_function_level_set(id, test_printer, LOGGER_DEBUG));

  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d\n", test, id, __LINE__));

  assert(LOGGER_OK == logger_id_output_function_deregister(id, test_printer));
  assert(LOGGER_OK == logger_id_release(id));

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- modify standard prefix ....\n", test);

  puts("Change the standard prefix and revert back");

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));
  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_DEBUG));
  id2 = logger_id_request("logger_test_id2");
  assert(LOGGER_OK == logger_id_enable(id2));
  assert(LOGGER_OK == logger_id_level_set(id2, LOGGER_DEBUG));

  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id2, LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id2, __LINE__));

  prefix = logger_prefix_get();
  assert (LOGGER_OK == logger_prefix_set(LOGGER_PFX_DATE | LOGGER_PFX_FILE | LOGGER_PFX_LINE));
  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id2, LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id2, __LINE__));

  assert(LOGGER_OK == logger_prefix_set(LOGGER_PFX_UNSET));
  assert(LOGGER_OK == logger(id,  LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id, __LINE__));
  assert(LOGGER_OK == logger(id2, LOGGER_EMERG, "test %d - id %d - LOGGER_EMERG   in line %d\n", test, id2, __LINE__));
  assert(prefix == logger_prefix_get());

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
  printf("Starting test %d -- test inttypes macros ....\n", test);

  puts("Test if inttypes macros are useable in format strings");

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));
  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_WARNING));

  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d - value %"PRIu32"\n", test, id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger(id, LOGGER_INFO,    "test %d - id %d - LOGGER_INFO    in line %d - value %"PRIu32"\n", test, id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger(id, LOGGER_NOTICE,  "test %d - id %d - LOGGER_NOTICE  in line %d - value %"PRIu32"\n", test, id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger(id, LOGGER_WARNING, "test %d - id %d - LOGGER_WARNING in line %d - value %"PRIu32"\n", test, id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger(id, LOGGER_ERR,     "test %d - id %d - LOGGER_ERR     in line %d - value %"PRIu32"\n", test, id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger(id, LOGGER_CRIT,    "test %d - id %d - LOGGER_CRIT    in line %d - value %"PRIu32"\n", test, id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger(id, LOGGER_ALERT,   "test %d - id %d - LOGGER_ALERT   in line %d - value %"PRIu32"\n", test, id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger(id, LOGGER_EMERG,   "test %d - id %d - LOGGER_EMERG   in line %d - value %"PRIu32"\n", test, id, __LINE__, tmp_uint32));

  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_id_release(id));

  printf("Ending test %d ....\n", test);

  test++;
  puts("");
  puts("****************************************************************************");
  puts("* TEST *********************************************************************");
  puts("****************************************************************************");
  puts("");
  printf("Starting test %d -- test variable argument list ....\n", test);

  puts("Test logger variant with variable argument list");

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));
  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_WARNING));

  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d - value %"PRIu32"\n", test, id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger(id, LOGGER_INFO,    "test %d - id %d - LOGGER_INFO    in line %d - value %"PRIu32"\n", test, id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger(id, LOGGER_NOTICE,  "test %d - id %d - LOGGER_NOTICE  in line %d - value %"PRIu32"\n", test, id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger(id, LOGGER_WARNING, "test %d - id %d - LOGGER_WARNING in line %d - value %"PRIu32"\n", test, id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger(id, LOGGER_ERR,     "test %d - id %d - LOGGER_ERR     in line %d - value %"PRIu32"\n", test, id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger(id, LOGGER_CRIT,    "test %d - id %d - LOGGER_CRIT    in line %d - value %"PRIu32"\n", test, id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger(id, LOGGER_ALERT,   "test %d - id %d - LOGGER_ALERT   in line %d - value %"PRIu32"\n", test, id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger(id, LOGGER_EMERG,   "test %d - id %d - LOGGER_EMERG   in line %d - value %"PRIu32"\n", test, id, __LINE__, tmp_uint32));

  assert(LOGGER_OK == logger_wrapper(id, LOGGER_DEBUG,   "test %d - id %d - LOGGER_DEBUG   in line %d - value %"PRIu32"\n", test, id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger_wrapper(id, LOGGER_INFO,    "test %d - id %d - LOGGER_INFO    in line %d - value %"PRIu32"\n", test, id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger_wrapper(id, LOGGER_NOTICE,  "test %d - id %d - LOGGER_NOTICE  in line %d - value %"PRIu32"\n", test, id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger_wrapper(id, LOGGER_WARNING, "test %d - id %d - LOGGER_WARNING in line %d - value %"PRIu32"\n", test, id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger_wrapper(id, LOGGER_ERR,     "test %d - id %d - LOGGER_ERR     in line %d - value %"PRIu32"\n", test, id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger_wrapper(id, LOGGER_CRIT,    "test %d - id %d - LOGGER_CRIT    in line %d - value %"PRIu32"\n", test, id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger_wrapper(id, LOGGER_ALERT,   "test %d - id %d - LOGGER_ALERT   in line %d - value %"PRIu32"\n", test, id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger_wrapper(id, LOGGER_EMERG,   "test %d - id %d - LOGGER_EMERG   in line %d - value %"PRIu32"\n", test, id, __LINE__, tmp_uint32));

  assert(LOGGER_OK == logger_output_deregister(stdout));
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

  printf("Ending test %d ....\n", test);

  return(0);
}


/* vim: set cino=t0,c0,(0,u0 et fen fdm=syntax fo=tcroqnl1 sw=2 sts=2 ts=8 : */
