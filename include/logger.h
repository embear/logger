/***************************************************************************//**
 * \file   $Id$
 *
 * \brief  logging facility for C
 * \author Markus Braun
 * \date   2010-10-13
 * \note   TODO: additional notes
 ******************************************************************************/
#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdio.h>

#define LOGGER_OUTPUTS_MAX    4

typedef unsigned char  logger_bool_t;
static const logger_bool_t logger_true  = (logger_bool_t)1;
static const logger_bool_t logger_false = (logger_bool_t)0;

typedef enum logger_severity_e {
  LOGGER_DEBUG   = 0, /**< debug-level message */
  LOGGER_INFO    = 1, /**< informational message */
  LOGGER_NOTICE  = 2, /**< normal, but significant, condition */
  LOGGER_WARNING = 3, /**< warning conditions */
  LOGGER_ERR     = 4, /**< error conditions */
  LOGGER_CRIT    = 5, /**< critical conditions */
  LOGGER_ALERT   = 6, /**< action must be taken immediately */
  LOGGER_EMERG   = 7, /**< system is unusable */
  LOGGER_MAX     = 8  /**< last entry, always! */
} logger_severity_t;


typedef enum logger_module_e {
  LOGGER_MODULE_ALL = 0, /**< enables all modules */
  LOGGER_MODULE_1,       /**< enable module 1 */
  LOGGER_MODULE_2,       /**< enable module 2 */
  LOGGER_MODULE_MAX      /**< last entry, always! */
} logger_module_t;


typedef enum logger_return_e {
  LOGGER_OK = 0,                /**< ok */
  LOGGER_ERR_OUTPUTS_FULL,      /**< all available otputs are used */
  LOGGER_ERR_OUTPUT_REGISTERED, /**< output already registered */
  LOGGER_ERR_OUTPUT_NOT_FOUND,  /**< output not registered */
  LOGGER_ERR_UNKNOWN_MODULE,    /**< module is unknown */
  LOGGER_ERR_UNKNOWN_SEVERITY,  /**< severity is unknown */
  LOGGER_ERR_MAX                /**< last entry, always! */
} logger_return_t;


#ifdef LOGGER_DISABLE
#define logger_init()                                   ((void)(0))
#define logger_add_output(stream)                       ((void)(0))
#define logger_del_output(stream)                       ((void)(0))
#define logger_enable_module(module)                    ((void)(0))
#define logger_disable_module(module)                   ((void)(0))
#define logger_set_module_severity(module, severity)    ((void)(0))
#define logger(module, severity, format, ...)           ((void)(0))
#else  /* LOGGER_ENABLE */
#define logger_init()                                   __logger_init()
#define logger_add_output(stream)                       __logger_add_output(stream)
#define logger_del_output(stream)                       __logger_del_output(stream)
#define logger_enable_module(module)                    __logger_enable_module(module)
#define logger_disable_module(module)                   __logger_disable_module(module)
#define logger_set_module_severity(module, severity)    __logger_set_module_severity(module, severity)
#define logger(module, severity, format, args...)      __logger(module, severity, format, ## args)
#endif /* LOGGER_ENABLE */

logger_return_t __logger_init(void);
logger_return_t __logger_add_output(FILE *stream);
logger_return_t __logger_del_output(FILE *stream);
logger_return_t __logger_enable_module(logger_module_t module);
logger_return_t __logger_disable_module(logger_module_t module);
logger_return_t __logger_set_module_severity(logger_module_t   module,
                                             logger_severity_t severity);
logger_return_t __logger(logger_module_t   module,
                         logger_severity_t severity,
                         const char        *format,
                         ...);

#endif /* end of include guard: __LOGGERGER_H__ */
