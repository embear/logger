/***************************************************************************//**
 * \file   $Id$
 *
 * \brief  logging facility for C
 * \author Markus Braun
 * \date   2010-10-13
 * \note   TODO: additional notes
 ******************************************************************************/
#include <string.h>
#include <stdarg.h>
#include "logger.h"

#ifndef LOGGER_DISABLE
static logger_bool_t     logger_module_enabled[LOGGER_MODULE_MAX];
static logger_severity_t logger_module_severity[LOGGER_MODULE_MAX];
static FILE              *logger_outputs[LOGGER_OUTPUTS_MAX];

logger_return_t __logger_init(void)
{
  logger_return_t ret = LOGGER_OK;

  memset(logger_module_enabled, 0, sizeof(logger_module_enabled));
  memset(logger_module_severity, LOGGER_DEBUG, sizeof(logger_module_severity));
  memset(logger_outputs, 0, sizeof(logger_outputs));

  return(ret);
}


logger_return_t __logger_add_output(FILE *stream)
{
  logger_return_t ret = LOGGER_OK;
  int             index;
  logger_bool_t   found;

  /* check if this output is already registered */
  found = logger_false;
  for (index = 0 ; index < LOGGER_OUTPUTS_MAX ; index++) {
    if (logger_outputs[index] == stream) {
      found = logger_true;
      break;
    }
    else {
      ret = LOGGER_ERR_OUTPUT_REGISTERED;
    }
  }

  /* stream is not registered */
  if (found == logger_false) {
    /* search for an empty slot */
    found = logger_false;
    for (index = 0 ; index < LOGGER_OUTPUTS_MAX ; index++) {
      if (logger_outputs[index] == NULL) {
        found = logger_true;
        break;
      }
    }

    /* found a empty slot */
    if (found == logger_true) {
      logger_outputs[index] = stream;
    }
    else {
      ret = LOGGER_ERR_OUTPUTS_FULL;
    }
  }

  return(ret);
}


logger_return_t __logger_del_output(FILE *stream)
{
  logger_return_t ret = LOGGER_OK;
  int             index;
  logger_bool_t   found;

  /* check if this output is already registered */
  found = logger_false;
  for (index = 0 ; index < LOGGER_OUTPUTS_MAX ; index++) {
    if (logger_outputs[index] == stream) {
      found = logger_true;
      break;
    }
  }

  /* found a empty slot */
  if (found == logger_true) {
    logger_outputs[index] = (FILE *)NULL;
  }
  else {
    ret = LOGGER_ERR_OUTPUT_NOT_FOUND;
  }

  return(ret);
}


logger_return_t __logger_enable_module(logger_module_t module)
{
  logger_return_t ret = LOGGER_OK;

  /* check for valid module */
  if ((module >= 0) &&
      (module < LOGGER_MODULE_MAX)) {
    /* enable given module */
    logger_module_enabled[module] = logger_true;
  }
  else {
    ret = LOGGER_ERR_UNKNOWN_MODULE;
  }

  return(ret);
}


logger_return_t __logger_disable_module(logger_module_t module)
{
  logger_return_t ret = LOGGER_OK;

  /* check for valid module */
  if ((module >= 0) &&
      (module < LOGGER_MODULE_MAX)) {
    /* disable given module */
    logger_module_enabled[module] = logger_false;
  }
  else {
    ret = LOGGER_ERR_UNKNOWN_MODULE;
  }

  return(ret);
}


logger_return_t __logger_set_module_severity(logger_module_t   module,
                                             logger_severity_t severity)
{
  logger_return_t ret = LOGGER_OK;

  /* check for valid module */
  if ((module >= 0) &&
      (module < LOGGER_MODULE_MAX)) {
    /* check for valid severity */
    if ((severity >= LOGGER_DEBUG) &&
        (severity <= LOGGER_MAX)) {
      /* set module severity */
      logger_module_severity[module] = severity;
    }
    else {
      ret = LOGGER_ERR_UNKNOWN_SEVERITY;
    }
  }
  else {
    ret = LOGGER_ERR_UNKNOWN_MODULE;
  }

  return(ret);
}


logger_return_t __logger(logger_module_t   module,
                         logger_severity_t severity,
                         const char        *format,
                         ...)
{
  logger_return_t ret = LOGGER_OK;
  va_list         argp;
  int             index;

  /* check for valid module */
  if ((module >= 0) &&
      (module < LOGGER_MODULE_MAX)) {
    /* check for valid severity */
    if ((severity >= LOGGER_DEBUG) &&
        (severity <= LOGGER_MAX)) {
      /* check if module is enabled and severity is high enough */
      if ((logger_module_enabled[module] == logger_true) &&
          (logger_module_severity[module] <= severity)) {
        va_start(argp, format);

        /* loop over all possibe outputs */
        for (index = 0 ; index < LOGGER_OUTPUTS_MAX ; index++) {
          if (logger_outputs[index] != NULL) {
            (void)vfprintf(logger_outputs[index], format, argp);
          }
        }
        va_end(argp);
      }
    }
    else {
      ret = LOGGER_ERR_UNKNOWN_SEVERITY;
    }
  }
  else {
    ret = LOGGER_ERR_UNKNOWN_MODULE;
  }

  return(ret);
}


#endif /* LOGGER_DISABLE */
