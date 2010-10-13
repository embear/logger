#include <stdio.h>
#include "logger.h"

int main(int argc, char * argv[], char * envp[])
{
  int test = 0;
  FILE *stream;

  logger_init();

  test++;
  printf("Starting test %d -- LOGGER_MODULE_1 not enabled ....\n", test);

  logger_add_output(stdout);

  logger(LOGGER_MODULE_1, LOGGER_DEBUG  , "test %d LOGGER_MODULE_1 - LOGGER_DEBUG   in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_INFO   , "test %d LOGGER_MODULE_1 - LOGGER_INFO    in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_NOTICE , "test %d LOGGER_MODULE_1 - LOGGER_NOTICE  in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_WARNING, "test %d LOGGER_MODULE_1 - LOGGER_WARNING in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_ERR    , "test %d LOGGER_MODULE_1 - LOGGER_ERR     in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_CRIT   , "test %d LOGGER_MODULE_1 - LOGGER_CRIT    in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_ALERT  , "test %d LOGGER_MODULE_1 - LOGGER_ALERT   in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_EMERG  , "test %d LOGGER_MODULE_1 - LOGGER_EMERG   in line %d\n", test, __LINE__);

  logger_del_output(stdout);

  printf("Ending test %d ....\n", test);
  
  test++;
  printf("Starting test %d -- severity not set ....\n", test);

  logger_add_output(stdout);
  logger_enable_module(LOGGER_MODULE_1);

  logger(LOGGER_MODULE_1, LOGGER_DEBUG  , "test %d LOGGER_MODULE_1 - LOGGER_DEBUG   in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_INFO   , "test %d LOGGER_MODULE_1 - LOGGER_INFO    in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_NOTICE , "test %d LOGGER_MODULE_1 - LOGGER_NOTICE  in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_WARNING, "test %d LOGGER_MODULE_1 - LOGGER_WARNING in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_ERR    , "test %d LOGGER_MODULE_1 - LOGGER_ERR     in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_CRIT   , "test %d LOGGER_MODULE_1 - LOGGER_CRIT    in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_ALERT  , "test %d LOGGER_MODULE_1 - LOGGER_ALERT   in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_EMERG  , "test %d LOGGER_MODULE_1 - LOGGER_EMERG   in line %d\n", test, __LINE__);

  logger_del_output(stdout);

  printf("Ending test %d ....\n", test);
  
  test++;
  printf("Starting test %d -- severity set ....\n", test);

  logger_add_output(stdout);
  logger_enable_module(LOGGER_MODULE_1);
  logger_set_module_severity(LOGGER_MODULE_1, LOGGER_WARNING);

  logger(LOGGER_MODULE_1, LOGGER_DEBUG  , "test %d LOGGER_MODULE_1 - LOGGER_DEBUG   in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_INFO   , "test %d LOGGER_MODULE_1 - LOGGER_INFO    in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_NOTICE , "test %d LOGGER_MODULE_1 - LOGGER_NOTICE  in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_WARNING, "test %d LOGGER_MODULE_1 - LOGGER_WARNING in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_ERR    , "test %d LOGGER_MODULE_1 - LOGGER_ERR     in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_CRIT   , "test %d LOGGER_MODULE_1 - LOGGER_CRIT    in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_ALERT  , "test %d LOGGER_MODULE_1 - LOGGER_ALERT   in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_EMERG  , "test %d LOGGER_MODULE_1 - LOGGER_EMERG   in line %d\n", test, __LINE__);

  logger_del_output(stdout);

  printf("Ending test %d ....\n", test);
  
  test++;
  printf("Starting test %d -- LOGGER_MODULE_1 enabled, LOGGER_MODULE_2 not ....\n", test);

  logger_add_output(stdout);
  logger_enable_module(LOGGER_MODULE_1);

  logger(LOGGER_MODULE_1, LOGGER_DEBUG  , "test %d LOGGER_MODULE_1 - LOGGER_DEBUG   in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_INFO   , "test %d LOGGER_MODULE_1 - LOGGER_INFO    in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_NOTICE , "test %d LOGGER_MODULE_1 - LOGGER_NOTICE  in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_WARNING, "test %d LOGGER_MODULE_1 - LOGGER_WARNING in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_ERR    , "test %d LOGGER_MODULE_1 - LOGGER_ERR     in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_CRIT   , "test %d LOGGER_MODULE_1 - LOGGER_CRIT    in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_ALERT  , "test %d LOGGER_MODULE_1 - LOGGER_ALERT   in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_EMERG  , "test %d LOGGER_MODULE_1 - LOGGER_EMERG   in line %d\n", test, __LINE__);

  logger(LOGGER_MODULE_2, LOGGER_DEBUG  , "test %d LOGGER_MODULE_2 - LOGGER_DEBUG   in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_2, LOGGER_INFO   , "test %d LOGGER_MODULE_2 - LOGGER_INFO    in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_2, LOGGER_NOTICE , "test %d LOGGER_MODULE_2 - LOGGER_NOTICE  in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_2, LOGGER_WARNING, "test %d LOGGER_MODULE_2 - LOGGER_WARNING in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_2, LOGGER_ERR    , "test %d LOGGER_MODULE_2 - LOGGER_ERR     in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_2, LOGGER_CRIT   , "test %d LOGGER_MODULE_2 - LOGGER_CRIT    in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_2, LOGGER_ALERT  , "test %d LOGGER_MODULE_2 - LOGGER_ALERT   in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_2, LOGGER_EMERG  , "test %d LOGGER_MODULE_2 - LOGGER_EMERG   in line %d\n", test, __LINE__);

  logger_del_output(stdout);

  printf("Ending test %d ....\n", test);
  
  test++;
  printf("Starting test %d -- output to stdout and stderr ....\n", test);

  logger_add_output(stdout);
  logger_add_output(stderr);
  logger_enable_module(LOGGER_MODULE_1);
  logger_set_module_severity(LOGGER_MODULE_1, LOGGER_ERR);

  logger(LOGGER_MODULE_1, LOGGER_DEBUG  , "test %d LOGGER_MODULE_1 - LOGGER_DEBUG   in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_INFO   , "test %d LOGGER_MODULE_1 - LOGGER_INFO    in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_NOTICE , "test %d LOGGER_MODULE_1 - LOGGER_NOTICE  in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_WARNING, "test %d LOGGER_MODULE_1 - LOGGER_WARNING in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_ERR    , "test %d LOGGER_MODULE_1 - LOGGER_ERR     in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_CRIT   , "test %d LOGGER_MODULE_1 - LOGGER_CRIT    in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_ALERT  , "test %d LOGGER_MODULE_1 - LOGGER_ALERT   in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_EMERG  , "test %d LOGGER_MODULE_1 - LOGGER_EMERG   in line %d\n", test, __LINE__);

  logger_del_output(stdout);
  logger_del_output(stderr);

  printf("Ending test %d ....\n", test);
  
  test++;
  printf("Starting test %d -- output to stdout and file ....\n", test);

  stream = fopen("logfile", "a");
  logger_add_output(stdout);
  logger_add_output(stream);
  logger_enable_module(LOGGER_MODULE_1);
  logger_set_module_severity(LOGGER_MODULE_1, LOGGER_ERR);

  logger(LOGGER_MODULE_1, LOGGER_DEBUG  , "test %d LOGGER_MODULE_1 - LOGGER_DEBUG   in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_INFO   , "test %d LOGGER_MODULE_1 - LOGGER_INFO    in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_NOTICE , "test %d LOGGER_MODULE_1 - LOGGER_NOTICE  in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_WARNING, "test %d LOGGER_MODULE_1 - LOGGER_WARNING in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_ERR    , "test %d LOGGER_MODULE_1 - LOGGER_ERR     in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_CRIT   , "test %d LOGGER_MODULE_1 - LOGGER_CRIT    in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_ALERT  , "test %d LOGGER_MODULE_1 - LOGGER_ALERT   in line %d\n", test, __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_EMERG  , "test %d LOGGER_MODULE_1 - LOGGER_EMERG   in line %d\n", test, __LINE__);

  logger_del_output(stdout);
  logger_del_output(stream);
  fclose(stream);

  printf("Ending test %d ....\n", test);
  
  return(0);
}


/* vim: set cino=t0,c0,(0,u0 et fen fdm=syntax fo=tcroqnl1 sw=2 sts=2 ts=8 : */
