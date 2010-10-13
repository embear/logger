#include <stdio.h>
#include "logger.h"

int main(int argc, char * argv[], char * envp[])
{
  FILE *stream;

  logger_init();

  puts("Starting test 1 ....");

  logger_add_output(stdout);

  logger(LOGGER_MODULE_1, LOGGER_DEBUG  , "test 1 LOGGER_MODULE_1 - LOGGER_DEBUG   in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_INFO   , "test 1 LOGGER_MODULE_1 - LOGGER_INFO    in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_NOTICE , "test 1 LOGGER_MODULE_1 - LOGGER_NOTICE  in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_WARNING, "test 1 LOGGER_MODULE_1 - LOGGER_WARNING in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_ERR    , "test 1 LOGGER_MODULE_1 - LOGGER_ERR     in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_CRIT   , "test 1 LOGGER_MODULE_1 - LOGGER_CRIT    in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_ALERT  , "test 1 LOGGER_MODULE_1 - LOGGER_ALERT   in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_EMERG  , "test 1 LOGGER_MODULE_1 - LOGGER_EMERG   in line %d\n", __LINE__);

  logger_del_output(stdout);

  puts("Ending test 1 ....");
  
  puts("Starting test 2 ....");

  logger_add_output(stdout);
  logger_enable_module(LOGGER_MODULE_1);
  logger_set_module_severity(LOGGER_MODULE_1, LOGGER_DEBUG);

  logger(LOGGER_MODULE_1, LOGGER_DEBUG  , "test 2 LOGGER_MODULE_1 - LOGGER_DEBUG   in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_INFO   , "test 2 LOGGER_MODULE_1 - LOGGER_INFO    in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_NOTICE , "test 2 LOGGER_MODULE_1 - LOGGER_NOTICE  in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_WARNING, "test 2 LOGGER_MODULE_1 - LOGGER_WARNING in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_ERR    , "test 2 LOGGER_MODULE_1 - LOGGER_ERR     in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_CRIT   , "test 2 LOGGER_MODULE_1 - LOGGER_CRIT    in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_ALERT  , "test 2 LOGGER_MODULE_1 - LOGGER_ALERT   in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_EMERG  , "test 2 LOGGER_MODULE_1 - LOGGER_EMERG   in line %d\n", __LINE__);

  logger_del_output(stdout);

  puts("Ending test 2 ....");
  
  puts("Starting test 3 ....");

  logger_add_output(stdout);
  logger_enable_module(LOGGER_MODULE_1);
  logger_set_module_severity(LOGGER_MODULE_1, LOGGER_DEBUG);

  logger(LOGGER_MODULE_1, LOGGER_DEBUG  , "test 3 LOGGER_MODULE_1 - LOGGER_DEBUG   in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_INFO   , "test 3 LOGGER_MODULE_1 - LOGGER_INFO    in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_NOTICE , "test 3 LOGGER_MODULE_1 - LOGGER_NOTICE  in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_WARNING, "test 3 LOGGER_MODULE_1 - LOGGER_WARNING in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_ERR    , "test 3 LOGGER_MODULE_1 - LOGGER_ERR     in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_CRIT   , "test 3 LOGGER_MODULE_1 - LOGGER_CRIT    in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_ALERT  , "test 3 LOGGER_MODULE_1 - LOGGER_ALERT   in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_EMERG  , "test 3 LOGGER_MODULE_1 - LOGGER_EMERG   in line %d\n", __LINE__);

  logger_del_output(stdout);

  puts("Ending test 3 ....");
  
  puts("Starting test 4 ....");

  logger_add_output(stdout);
  logger_add_output(stderr);
  logger_enable_module(LOGGER_MODULE_1);
  logger_set_module_severity(LOGGER_MODULE_1, LOGGER_ERR);

  logger(LOGGER_MODULE_1, LOGGER_DEBUG  , "test 4 LOGGER_MODULE_1 - LOGGER_DEBUG   in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_INFO   , "test 4 LOGGER_MODULE_1 - LOGGER_INFO    in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_NOTICE , "test 4 LOGGER_MODULE_1 - LOGGER_NOTICE  in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_WARNING, "test 4 LOGGER_MODULE_1 - LOGGER_WARNING in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_ERR    , "test 4 LOGGER_MODULE_1 - LOGGER_ERR     in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_CRIT   , "test 4 LOGGER_MODULE_1 - LOGGER_CRIT    in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_ALERT  , "test 4 LOGGER_MODULE_1 - LOGGER_ALERT   in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_EMERG  , "test 4 LOGGER_MODULE_1 - LOGGER_EMERG   in line %d\n", __LINE__);

  logger_del_output(stdout);
  logger_del_output(stderr);

  puts("Ending test 4 ....");
  
  puts("Starting test 5 ....");

  stream = fopen("logfile", "a");
  logger_add_output(stdout);
  logger_add_output(stream);
  logger_enable_module(LOGGER_MODULE_1);
  logger_set_module_severity(LOGGER_MODULE_1, LOGGER_ERR);

  logger(LOGGER_MODULE_1, LOGGER_DEBUG  , "test 5 LOGGER_MODULE_1 - LOGGER_DEBUG   in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_INFO   , "test 5 LOGGER_MODULE_1 - LOGGER_INFO    in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_NOTICE , "test 5 LOGGER_MODULE_1 - LOGGER_NOTICE  in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_WARNING, "test 5 LOGGER_MODULE_1 - LOGGER_WARNING in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_ERR    , "test 5 LOGGER_MODULE_1 - LOGGER_ERR     in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_CRIT   , "test 5 LOGGER_MODULE_1 - LOGGER_CRIT    in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_ALERT  , "test 5 LOGGER_MODULE_1 - LOGGER_ALERT   in line %d\n", __LINE__);
  logger(LOGGER_MODULE_1, LOGGER_EMERG  , "test 5 LOGGER_MODULE_1 - LOGGER_EMERG   in line %d\n", __LINE__);

  logger_del_output(stdout);
  logger_del_output(stream);
  fclose(stream);

  puts("Ending test 5 ....");
  
  return(0);
}


/* vim: set cino=t0,c0,(0,u0 et fen fdm=syntax fo=tcroqnl1 sw=2 sts=2 ts=8 : */
