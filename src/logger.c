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

#define LOGGER_OUTPUTS_MAX    16 /**< number of possible simultaneous outputs */
#define LOGGER_MODULES_MAX    16 /**< number of possible modules */

static FILE              *logger_outputs[LOGGER_OUTPUTS_MAX];        /**< storage for possible output streams */
static logger_bool_t     logger_modules[LOGGER_MODULES_MAX];         /**< storage for possible modules */
static logger_bool_t     logger_modules_enabled[LOGGER_MODULES_MAX];  /**< which modules are enabled */
static logger_severity_t logger_modules_severity[LOGGER_MODULES_MAX]; /**< which severity for which module */

/** ************************************************************************//**
 * \brief  initialize logger
 *
 * \return     LOGGER_OK if no error occurred, error code otherwise
 ******************************************************************************/
logger_return_t __logger_init(void)
{
  logger_return_t ret = LOGGER_OK;

  memset(logger_modules, 0, sizeof(logger_modules));
  memset(logger_modules_enabled, 0, sizeof(logger_modules_enabled));
  memset(logger_modules_severity, LOGGER_DEBUG, sizeof(logger_modules_severity));
  memset(logger_outputs, 0, sizeof(logger_outputs));

  return(ret);
}


/** ************************************************************************//**
 * \brief  register an output stream to logger
 *
 * the given file stream may be on of stdout, stderr or a file stream opened by
 * the user.
 *
 * \param[in]     stream opened file stream
 *
 * \return     LOGGER_OK if no error occurred, error code otherwise
 ******************************************************************************/
logger_return_t __logger_output_register(FILE *stream)
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

    /* found an empty slot */
    if (found == logger_true) {
      logger_outputs[index] = stream;
    }
    else {
      ret = LOGGER_ERR_OUTPUTS_FULL;
    }
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  deregister an output stream to logger
 *
 * \param[in]     stream previous registered file stream
 *
 * \return     LOGGER_OK if no error occurred, error code otherwise
 ******************************************************************************/
logger_return_t __logger_output_deregister(FILE *stream)
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

  /* found an empty slot */
  if (found == logger_true) {
    logger_outputs[index] = (FILE *)NULL;
  }
  else {
    ret = LOGGER_ERR_OUTPUT_NOT_FOUND;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  request a module from logger
 *
 * \return     module number if module is available, error code otherwise
 ******************************************************************************/
logger_module_t __logger_module_request(void)
{
  logger_return_t ret = LOGGER_OK;
  int             index;
  logger_bool_t   found;

  /* search for an available module */
  found = logger_false;
  for (index = 0 ; index < LOGGER_OUTPUTS_MAX ; index++) {
    if (logger_modules[index] == logger_false) {
      found = logger_true;
      break;
    }
  }

  /* found an empty slot */
  if (found == logger_true) {
    logger_modules[index] = logger_true;
    ret = index;
  }
  else {
    ret = LOGGER_ERR_MODULES_FULL;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  release a logger module
 *
 * \param[in]     module module to enable
 *
 * \return     LOGGER_OK if no error occurred, error code otherwise
 ******************************************************************************/
logger_return_t __logger_module_release(logger_module_t module)
{
  logger_return_t ret = LOGGER_OK;

  /* check for valid module */
  if ((module >= 0) &&
      (module < LOGGER_MODULES_MAX)) {
    if (logger_modules[module] == logger_true) {
      /* reset all module dependend values to defaults */
      logger_modules[module]         = logger_false;
      logger_modules_enabled[module]  = logger_false;
      logger_modules_severity[module] = LOGGER_DEBUG;
    }
    else {
      ret = LOGGER_ERR_MODULE_NOT_FOUND;
    }
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  enable a logger module for output
 *
 * \param[in]     module module to enable
 *
 * \return     LOGGER_OK if no error occurred, error code otherwise
 ******************************************************************************/
logger_return_t __logger_module_enable(logger_module_t module)
{
  logger_return_t ret = LOGGER_OK;

  /* check for valid module */
  if ((module >= 0) &&
      (module < LOGGER_MODULES_MAX)) {
    /* enable given module */
    logger_modules_enabled[module] = logger_true;
  }
  else {
    ret = LOGGER_ERR_MODULE_UNKNOWN;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  disable a logger module for output
 *
 * \param[in]     module module to disable
 *
 * \return     LOGGER_OK if no error occurred, error code otherwise
 ******************************************************************************/
logger_return_t __logger_module_disable(logger_module_t module)
{
  logger_return_t ret = LOGGER_OK;

  /* check for valid module */
  if ((module >= 0) &&
      (module < LOGGER_MODULES_MAX)) {
    /* disable given module */
    logger_modules_enabled[module] = logger_false;
  }
  else {
    ret = LOGGER_ERR_MODULE_UNKNOWN;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  set required minimum severity for log output
 *
 * \param[in]     module module for setting severity
 * \param[in]     severity severity to set
 *
 * \return     LOGGER_OK if no error occurred, error code otherwise
 ******************************************************************************/
logger_return_t __logger_module_severity_set(logger_module_t   module,
                                             logger_severity_t severity)
{
  logger_return_t ret = LOGGER_OK;

  /* check for valid module */
  if ((module >= 0) &&
      (module < LOGGER_MODULES_MAX)) {
    /* check for valid severity */
    if ((severity >= LOGGER_DEBUG) &&
        (severity <= LOGGER_MAX)) {
      /* set module severity */
      logger_modules_severity[module] = severity;
    }
    else {
      ret = LOGGER_ERR_SEVERITY_UNKNOWN;
    }
  }
  else {
    ret = LOGGER_ERR_MODULE_UNKNOWN;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  output log message
 *
 * \param[in]     module module outputting this message
 * \param[in]     severity severity of this message
 * \param[in]     format printf like format string
 * \param[in]     va_args argument list
 *
 * \return     LOGGER_OK if no error occurred, error code otherwise
 ******************************************************************************/
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
      (module < LOGGER_MODULES_MAX)) {
    /* check for valid severity */
    if ((severity >= LOGGER_DEBUG) &&
        (severity <= LOGGER_MAX)) {
      /* check if module is enabled and severity is high enough */
      if ((logger_modules_enabled[module] == logger_true) &&
          (logger_modules_severity[module] <= severity)) {
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
      ret = LOGGER_ERR_SEVERITY_UNKNOWN;
    }
  }
  else {
    ret = LOGGER_ERR_MODULE_UNKNOWN;
  }

  return(ret);
}


#endif /* LOGGER_DISABLE */
