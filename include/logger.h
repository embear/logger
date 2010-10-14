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

typedef unsigned char  logger_bool_t;                       /**< logger boolean type */
static const logger_bool_t logger_true  = (logger_bool_t)1; /**< logger boolean true */
static const logger_bool_t logger_false = (logger_bool_t)0; /**< logger boolean false */


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


typedef int  logger_module_t;                                             /**< logger module type */
static const logger_module_t logger_module_unknown = (logger_module_t)-1; /**< unknown logger module */


typedef enum logger_return_e {
  LOGGER_OK                    = 0,  /**< ok */
  LOGGER_ERR_OUTPUTS_FULL      = -1, /**< all available outputs are used */
  LOGGER_ERR_OUTPUT_REGISTERED = -2, /**< output already registered */
  LOGGER_ERR_OUTPUT_NOT_FOUND  = -3, /**< output not registered */
  LOGGER_ERR_MODULES_FULL      = -4, /**< all available modules are used */
  LOGGER_ERR_MODULE_NOT_FOUND  = -5, /**< module not registered */
  LOGGER_ERR_MODULE_UNKNOWN    = -6, /**< module is unknown */
  LOGGER_ERR_SEVERITY_UNKNOWN  = -7, /**< severity is unknown */
} logger_return_t;


#ifdef LOGGER_DISABLE
#define logger_init()                                        ((void)(0))
#define logger_output_register(stream)                       ((void)(0))
#define logger_output_deregister(stream)                     ((void)(0))
#define logger_module_request()                              ((void)(0))
#define logger_module_release(module)                        ((void)(0))
#define logger_module_enable(module)                         ((void)(0))
#define logger_module_disable(module)                        ((void)(0))
#define logger_module_severity_set(module, severity)         ((void)(0))
#define logger(module, severity, format,...)                 ((void)(0))
#define logger_verbose(module, severity, format, args...)    ((void)(0))
#else  /* LOGGER_ENABLE */
#define LOGGER_STRINGIFY_(x)                                 # x
#define LOGGER_STRINGIFY(x)                                  LOGGER_STRINGIFY_(x)
#define logger_init()                                        __logger_init()
#define logger_output_register(stream)                       __logger_output_register(stream)
#define logger_output_deregister(stream)                     __logger_output_deregister(stream)
#define logger_module_request()                              __logger_module_request()
#define logger_module_release(module)                        __logger_module_release(module)
#define logger_module_enable(module)                         __logger_module_enable(module)
#define logger_module_disable(module)                        __logger_module_disable(module)
#define logger_module_severity_set(module, severity)         __logger_module_severity_set(module, severity)
#define logger(module, severity, format, args...)            __logger(module, severity, format, ##args)
#define logger_verbose(module, severity, format, args...)    __logger(module, severity, LOGGER_STRINGIFY(module) ":" LOGGER_STRINGIFY(severity) ":" __FILE__ ":" LOGGER_STRINGIFY(__LINE__) ": " format, ##args)

logger_return_t __logger_init(void);
logger_return_t __logger_output_register(FILE *stream);
logger_return_t __logger_output_deregister(FILE *stream);
logger_module_t __logger_module_request(void);
logger_return_t __logger_module_release(logger_module_t module);
logger_return_t __logger_module_enable(logger_module_t module);
logger_return_t __logger_module_disable(logger_module_t module);
logger_return_t __logger_module_severity_set(logger_module_t   module,
                                             logger_severity_t severity);
logger_return_t __logger(logger_module_t   module,
                         logger_severity_t severity,
                         const char        *format,
                         ...);
#endif /* LOGGER_ENABLE */

#endif /* end of include guard: __LOGGERGER_H__ */
