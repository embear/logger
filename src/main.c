#include <stdio.h>
#include "logger.h"

int main(int  argc,
         char *argv[])
{
  int             test = 0;
  FILE            *stream;
  logger_module_t module1 = logger_module_unknown;
  logger_module_t module2 = logger_module_unknown;

  logger_init();

  test++;
  printf("Starting test %d -- module1 not registered ....\n", test);

  logger_output_register(stdout);

  logger(module1, LOGGER_DEBUG, "test %d module1 - LOGGER_DEBUG   in line %d\n", test, __LINE__);
  logger(module1, LOGGER_INFO, "test %d module1 - LOGGER_INFO    in line %d\n", test, __LINE__);
  logger(module1, LOGGER_NOTICE, "test %d module1 - LOGGER_NOTICE  in line %d\n", test, __LINE__);
  logger(module1, LOGGER_WARNING, "test %d module1 - LOGGER_WARNING in line %d\n", test, __LINE__);
  logger(module1, LOGGER_ERR, "test %d module1 - LOGGER_ERR     in line %d\n", test, __LINE__);
  logger(module1, LOGGER_CRIT, "test %d module1 - LOGGER_CRIT    in line %d\n", test, __LINE__);
  logger(module1, LOGGER_ALERT, "test %d module1 - LOGGER_ALERT   in line %d\n", test, __LINE__);
  logger(module1, LOGGER_EMERG, "test %d module1 - LOGGER_EMERG   in line %d\n", test, __LINE__);

  logger_output_deregister(stdout);

  printf("Ending test %d ....\n", test);

  test++;
  printf("Starting test %d -- module1 not enabled ....\n", test);

  logger_output_register(stdout);
  module1 = logger_module_request();

  logger(module1, LOGGER_DEBUG, "test %d module1 - LOGGER_DEBUG   in line %d\n", test, __LINE__);
  logger(module1, LOGGER_INFO, "test %d module1 - LOGGER_INFO    in line %d\n", test, __LINE__);
  logger(module1, LOGGER_NOTICE, "test %d module1 - LOGGER_NOTICE  in line %d\n", test, __LINE__);
  logger(module1, LOGGER_WARNING, "test %d module1 - LOGGER_WARNING in line %d\n", test, __LINE__);
  logger(module1, LOGGER_ERR, "test %d module1 - LOGGER_ERR     in line %d\n", test, __LINE__);
  logger(module1, LOGGER_CRIT, "test %d module1 - LOGGER_CRIT    in line %d\n", test, __LINE__);
  logger(module1, LOGGER_ALERT, "test %d module1 - LOGGER_ALERT   in line %d\n", test, __LINE__);
  logger(module1, LOGGER_EMERG, "test %d module1 - LOGGER_EMERG   in line %d\n", test, __LINE__);

  logger_output_deregister(stdout);
  logger_module_release(module1);

  printf("Ending test %d ....\n", test);

  test++;
  printf("Starting test %d -- severity not set ....\n", test);

  logger_output_register(stdout);
  module1 = logger_module_request();
  logger_module_enable(module1);

  logger(module1, LOGGER_DEBUG, "test %d module1 - LOGGER_DEBUG   in line %d\n", test, __LINE__);
  logger(module1, LOGGER_INFO, "test %d module1 - LOGGER_INFO    in line %d\n", test, __LINE__);
  logger(module1, LOGGER_NOTICE, "test %d module1 - LOGGER_NOTICE  in line %d\n", test, __LINE__);
  logger(module1, LOGGER_WARNING, "test %d module1 - LOGGER_WARNING in line %d\n", test, __LINE__);
  logger(module1, LOGGER_ERR, "test %d module1 - LOGGER_ERR     in line %d\n", test, __LINE__);
  logger(module1, LOGGER_CRIT, "test %d module1 - LOGGER_CRIT    in line %d\n", test, __LINE__);
  logger(module1, LOGGER_ALERT, "test %d module1 - LOGGER_ALERT   in line %d\n", test, __LINE__);
  logger(module1, LOGGER_EMERG, "test %d module1 - LOGGER_EMERG   in line %d\n", test, __LINE__);

  logger_output_deregister(stdout);
  logger_module_release(module1);

  printf("Ending test %d ....\n", test);

  test++;
  printf("Starting test %d -- severity set to LOGGER_WARNING ....\n", test);

  logger_output_register(stdout);
  module1 = logger_module_request();
  logger_module_enable(module1);
  logger_module_severity_set(module1, LOGGER_WARNING);

  logger(module1, LOGGER_DEBUG, "test %d module1 - LOGGER_DEBUG   in line %d\n", test, __LINE__);
  logger(module1, LOGGER_INFO, "test %d module1 - LOGGER_INFO    in line %d\n", test, __LINE__);
  logger(module1, LOGGER_NOTICE, "test %d module1 - LOGGER_NOTICE  in line %d\n", test, __LINE__);
  logger(module1, LOGGER_WARNING, "test %d module1 - LOGGER_WARNING in line %d\n", test, __LINE__);
  logger(module1, LOGGER_ERR, "test %d module1 - LOGGER_ERR     in line %d\n", test, __LINE__);
  logger(module1, LOGGER_CRIT, "test %d module1 - LOGGER_CRIT    in line %d\n", test, __LINE__);
  logger(module1, LOGGER_ALERT, "test %d module1 - LOGGER_ALERT   in line %d\n", test, __LINE__);
  logger(module1, LOGGER_EMERG, "test %d module1 - LOGGER_EMERG   in line %d\n", test, __LINE__);

  logger_output_deregister(stdout);
  logger_module_release(module1);

  printf("Ending test %d ....\n", test);

  test++;
  printf("Starting test %d -- module1 enabled, module2 not ....\n", test);

  logger_output_register(stdout);
  module1 = logger_module_request();
  logger_module_enable(module1);

  logger(module1, LOGGER_DEBUG, "test %d module1 - LOGGER_DEBUG   in line %d\n", test, __LINE__);
  logger(module1, LOGGER_INFO, "test %d module1 - LOGGER_INFO    in line %d\n", test, __LINE__);
  logger(module1, LOGGER_NOTICE, "test %d module1 - LOGGER_NOTICE  in line %d\n", test, __LINE__);
  logger(module1, LOGGER_WARNING, "test %d module1 - LOGGER_WARNING in line %d\n", test, __LINE__);
  logger(module1, LOGGER_ERR, "test %d module1 - LOGGER_ERR     in line %d\n", test, __LINE__);
  logger(module1, LOGGER_CRIT, "test %d module1 - LOGGER_CRIT    in line %d\n", test, __LINE__);
  logger(module1, LOGGER_ALERT, "test %d module1 - LOGGER_ALERT   in line %d\n", test, __LINE__);
  logger(module1, LOGGER_EMERG, "test %d module1 - LOGGER_EMERG   in line %d\n", test, __LINE__);

  logger(module2, LOGGER_DEBUG, "test %d module2 - LOGGER_DEBUG   in line %d\n", test, __LINE__);
  logger(module2, LOGGER_INFO, "test %d module2 - LOGGER_INFO    in line %d\n", test, __LINE__);
  logger(module2, LOGGER_NOTICE, "test %d module2 - LOGGER_NOTICE  in line %d\n", test, __LINE__);
  logger(module2, LOGGER_WARNING, "test %d module2 - LOGGER_WARNING in line %d\n", test, __LINE__);
  logger(module2, LOGGER_ERR, "test %d module2 - LOGGER_ERR     in line %d\n", test, __LINE__);
  logger(module2, LOGGER_CRIT, "test %d module2 - LOGGER_CRIT    in line %d\n", test, __LINE__);
  logger(module2, LOGGER_ALERT, "test %d module2 - LOGGER_ALERT   in line %d\n", test, __LINE__);
  logger(module2, LOGGER_EMERG, "test %d module2 - LOGGER_EMERG   in line %d\n", test, __LINE__);

  logger_output_deregister(stdout);
  logger_module_release(module1);

  printf("Ending test %d ....\n", test);

  test++;
  printf("Starting test %d -- output to stdout and stderr ....\n", test);

  logger_output_register(stdout);
  logger_output_register(stderr);
  module1 = logger_module_request();
  logger_module_enable(module1);
  logger_module_severity_set(module1, LOGGER_ERR);

  logger(module1, LOGGER_DEBUG, "test %d module1 - LOGGER_DEBUG   in line %d\n", test, __LINE__);
  logger(module1, LOGGER_INFO, "test %d module1 - LOGGER_INFO    in line %d\n", test, __LINE__);
  logger(module1, LOGGER_NOTICE, "test %d module1 - LOGGER_NOTICE  in line %d\n", test, __LINE__);
  logger(module1, LOGGER_WARNING, "test %d module1 - LOGGER_WARNING in line %d\n", test, __LINE__);
  logger(module1, LOGGER_ERR, "test %d module1 - LOGGER_ERR     in line %d\n", test, __LINE__);
  logger(module1, LOGGER_CRIT, "test %d module1 - LOGGER_CRIT    in line %d\n", test, __LINE__);
  logger(module1, LOGGER_ALERT, "test %d module1 - LOGGER_ALERT   in line %d\n", test, __LINE__);
  logger(module1, LOGGER_EMERG, "test %d module1 - LOGGER_EMERG   in line %d\n", test, __LINE__);

  logger_output_deregister(stdout);
  logger_output_deregister(stderr);
  logger_module_release(module1);

  printf("Ending test %d ....\n", test);

  test++;
  printf("Starting test %d -- output to stdout and file ....\n", test);

  stream = fopen("logfile", "a");
  logger_output_register(stdout);
  logger_output_register(stream);
  module1 = logger_module_request();
  logger_module_enable(module1);
  logger_module_severity_set(module1, LOGGER_ERR);

  logger(module1, LOGGER_DEBUG, "test %d module1 - LOGGER_DEBUG   in line %d\n", test, __LINE__);
  logger(module1, LOGGER_INFO, "test %d module1 - LOGGER_INFO    in line %d\n", test, __LINE__);
  logger(module1, LOGGER_NOTICE, "test %d module1 - LOGGER_NOTICE  in line %d\n", test, __LINE__);
  logger(module1, LOGGER_WARNING, "test %d module1 - LOGGER_WARNING in line %d\n", test, __LINE__);
  logger(module1, LOGGER_ERR, "test %d module1 - LOGGER_ERR     in line %d\n", test, __LINE__);
  logger(module1, LOGGER_CRIT, "test %d module1 - LOGGER_CRIT    in line %d\n", test, __LINE__);
  logger(module1, LOGGER_ALERT, "test %d module1 - LOGGER_ALERT   in line %d\n", test, __LINE__);
  logger(module1, LOGGER_EMERG, "test %d module1 - LOGGER_EMERG   in line %d\n", test, __LINE__);

  logger_output_deregister(stdout);
  logger_output_deregister(stream);
  fclose(stream);
  logger_module_release(module1);

  printf("Ending test %d ....\n", test);

  test++;
  printf("Starting test %d -- verbose logging ....\n", test);

  logger_output_register(stdout);
  module1 = logger_module_request();
  logger_module_enable(module1);
  logger_module_severity_set(module1, LOGGER_DEBUG);

  logger_verbose(module1, LOGGER_DEBUG, "test %d module1 - LOGGER_DEBUG   in line %d\n", test, __LINE__);
  logger_verbose(module1, LOGGER_INFO, "test %d module1 - LOGGER_INFO    in line %d\n", test, __LINE__);
  logger_verbose(module1, LOGGER_NOTICE, "test %d module1 - LOGGER_NOTICE  in line %d\n", test, __LINE__);
  logger_verbose(module1, LOGGER_WARNING, "test %d module1 - LOGGER_WARNING in line %d\n", test, __LINE__);
  logger_verbose(module1, LOGGER_ERR, "test %d module1 - LOGGER_ERR     in line %d\n", test, __LINE__);
  logger_verbose(module1, LOGGER_CRIT, "test %d module1 - LOGGER_CRIT    in line %d\n", test, __LINE__);
  logger_verbose(module1, LOGGER_ALERT, "test %d module1 - LOGGER_ALERT   in line %d\n", test, __LINE__);
  logger_verbose(module1, LOGGER_EMERG, "test %d module1 - LOGGER_EMERG   in line %d\n", test, __LINE__);

  logger_output_deregister(stdout);
  logger_module_release(module1);

  printf("Ending test %d ....\n", test);

  return(0);
}


/* vim: set cino=t0,c0,(0,u0 et fen fdm=syntax fo=tcroqnl1 sw=2 sts=2 ts=8 : */
