#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#include "logger.h"

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


int main(int  argc, char *argv[])
{
  logger_id_t id         = logger_id_unknown;
  uint32_t    tmp_uint32 = 32;

  assert(LOGGER_OK == logger_init());

  assert(LOGGER_OK == logger_output_register(stdout));
  assert(LOGGER_OK == logger_output_level_set(stdout, LOGGER_DEBUG));

  id = logger_id_request("logger_test_id");
  assert(LOGGER_OK == logger_id_enable(id));
  assert(LOGGER_OK == logger_id_level_set(id, LOGGER_WARNING));

  assert(LOGGER_OK == logger(id, LOGGER_DEBUG,   "id %d - LOGGER_DEBUG   in line %d - value %"PRIu32"\n", id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger(id, LOGGER_INFO,    "id %d - LOGGER_INFO    in line %d - value %"PRIu32"\n", id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger(id, LOGGER_NOTICE,  "id %d - LOGGER_NOTICE  in line %d - value %"PRIu32"\n", id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger(id, LOGGER_WARNING, "id %d - LOGGER_WARNING in line %d - value %"PRIu32"\n", id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger(id, LOGGER_ERR,     "id %d - LOGGER_ERR     in line %d - value %"PRIu32"\n", id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger(id, LOGGER_CRIT,    "id %d - LOGGER_CRIT    in line %d - value %"PRIu32"\n", id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger(id, LOGGER_ALERT,   "id %d - LOGGER_ALERT   in line %d - value %"PRIu32"\n", id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger(id, LOGGER_EMERG,   "id %d - LOGGER_EMERG   in line %d - value %"PRIu32"\n", id, __LINE__, tmp_uint32));

  assert(LOGGER_OK == logger_wrapper(id, LOGGER_DEBUG,   "id %d - LOGGER_DEBUG   in line %d - value %"PRIu32"\n", id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger_wrapper(id, LOGGER_INFO,    "id %d - LOGGER_INFO    in line %d - value %"PRIu32"\n", id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger_wrapper(id, LOGGER_NOTICE,  "id %d - LOGGER_NOTICE  in line %d - value %"PRIu32"\n", id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger_wrapper(id, LOGGER_WARNING, "id %d - LOGGER_WARNING in line %d - value %"PRIu32"\n", id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger_wrapper(id, LOGGER_ERR,     "id %d - LOGGER_ERR     in line %d - value %"PRIu32"\n", id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger_wrapper(id, LOGGER_CRIT,    "id %d - LOGGER_CRIT    in line %d - value %"PRIu32"\n", id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger_wrapper(id, LOGGER_ALERT,   "id %d - LOGGER_ALERT   in line %d - value %"PRIu32"\n", id, __LINE__, tmp_uint32));
  assert(LOGGER_OK == logger_wrapper(id, LOGGER_EMERG,   "id %d - LOGGER_EMERG   in line %d - value %"PRIu32"\n", id, __LINE__, tmp_uint32));

  assert(LOGGER_OK == logger_output_deregister(stdout));
  assert(LOGGER_OK == logger_id_release(id));

  return(0);
}
