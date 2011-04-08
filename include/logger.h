/***************************************************************************//**
 * \file   $Id$
 *
 * \brief  logging facility for C
 * \author Markus Braun
 * \date   2010-10-13
 ******************************************************************************/

/***************************************************************************//**
 * \mainpage
 *
 * \section sec_intro Introduction
 *
 * Logger is a simple logging facility for the C language.
 * It is possible to output logging information to different output stream and to use different logging severities.
 *
 * \section sec_usage Usage
 *
 * Logger needs to be enabled using the global define LOGGER_ENABLE.
 * Otherwise all functions will be excluded from compilation.
 *
 * Basic usage of logger:
 *
 * \code
 * logger_id_t    id  = logger_id_unknown;
 *
 * logger_output_register(stdout);
 * id = logger_id_request();
 * logger_id_enable(id);
 * logger_id_level_set(id, LOGGER_WARNING);
 *
 * logger(id, LOGGER_WARNING, "test %d - id %d - LOGGER_WARNING in line %d\n", test, id, __LINE__);
 *
 * logger_output_deregister(stdout);
 * logger_id_release(id);
 * \endcode
 */
#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

typedef unsigned char  logger_bool_t;                       /**< logger boolean type */
static const logger_bool_t logger_true  = (logger_bool_t)1; /**< logger boolean true */
static const logger_bool_t logger_false = (logger_bool_t)0; /**< logger boolean false */


typedef enum logger_level_e {
  LOGGER_UNKNOWN = 0,  /**< unknown level */
  LOGGER_DEBUG   = 1,  /**< debug-level message */
  LOGGER_INFO    = 2,  /**< informational message */
  LOGGER_NOTICE  = 3,  /**< normal, but significant, condition */
  LOGGER_WARNING = 4,  /**< warning conditions */
  LOGGER_ERR     = 5,  /**< error conditions */
  LOGGER_CRIT    = 6,  /**< critical conditions */
  LOGGER_ALERT   = 7,  /**< action must be taken immediately */
  LOGGER_EMERG   = 8,  /**< system is unusable */
  LOGGER_MAX     = 9   /**< last entry, always! */
} logger_level_t;


typedef int  logger_id_t;                                     /**< logger id type */
static const logger_id_t logger_id_unknown = (logger_id_t)-1; /**< unknown logger id */


typedef enum logger_return_e {
  LOGGER_OK                    = 0,  /**< ok */
  LOGGER_ERR_OUTPUT_INVALID    = -1, /**< given output stream is invalid */
  LOGGER_ERR_OUTPUTS_FULL      = -2, /**< all available outputs are used */
  LOGGER_ERR_OUTPUT_REGISTERED = -3, /**< output already registered */
  LOGGER_ERR_OUTPUT_NOT_FOUND  = -4, /**< output not registered */
  LOGGER_ERR_IDS_FULL          = -5, /**< all available ids are used */
  LOGGER_ERR_ID_NOT_FOUND      = -6, /**< id not registered */
  LOGGER_ERR_ID_UNKNOWN        = -7, /**< id is unknown */
  LOGGER_ERR_LEVEL_UNKNOWN     = -8, /**< level is unknown */
} logger_return_t;

/* macros for color text output */
typedef enum logger_text_attr_e {
  LOGGER_ATTR_RESET     = 0, /**< reset attributes */
  LOGGER_ATTR_BRIGHT    = 1, /**< bright attribute */
  LOGGER_ATTR_DIM       = 2, /**< dim attribute */
  LOGGER_ATTR_UNDERLINE = 3, /**< underline attribute */
  LOGGER_ATTR_BLINK     = 5, /**< blink attribute */
  LOGGER_ATTR_REVERSE   = 7, /**< reverse attribute */
  LOGGER_ATTR_HIDDEN    = 8  /**< hidden attribute */
} logger_text_attr_t;

typedef enum logger_text_bg_e {
  LOGGER_BG_BLACK   = 40, /**< black background color */
  LOGGER_BG_RED     = 41, /**< red background color */
  LOGGER_BG_GREEN   = 42, /**< green background color */
  LOGGER_BG_YELLOW  = 43, /**< yellow background color */
  LOGGER_BG_BLUE    = 44, /**< blue background color */
  LOGGER_BG_MAGENTA = 45, /**< magenta background color */
  LOGGER_BG_CYAN    = 46, /**< cyan background color */
  LOGGER_BG_WHITE   = 47  /**< white background color */
} logger_text_bg_t;

typedef enum logger_text_fg_e {
  LOGGER_FG_BLACK   = 30, /**< black background color */
  LOGGER_FG_RED     = 31, /**< red background color */
  LOGGER_FG_GREEN   = 32, /**< green background color */
  LOGGER_FG_YELLOW  = 33, /**< yellow background color */
  LOGGER_FG_BLUE    = 34, /**< blue background color */
  LOGGER_FG_MAGENTA = 35, /**< magenta background color */
  LOGGER_FG_CYAN    = 36, /**< cyan background color */
  LOGGER_FG_WHITE   = 37  /**< white background color */
} logger_text_fg_t;


#ifdef LOGGER_ENABLE
#define LOGGER_STRINGIFY_(x)                           # x
#define LOGGER_STRINGIFY(x)                            LOGGER_STRINGIFY_(x)
#define logger_init()                                  __logger_init()
#define logger_output_register(stream)                 __logger_output_register(stream)
#define logger_output_deregister(stream)               __logger_output_deregister(stream)
#define logger_id_request()                            __logger_id_request()
#define logger_id_release(id)                          __logger_id_release(id)
#define logger_id_enable(id)                           __logger_id_enable(id)
#define logger_id_disable(id)                          __logger_id_disable(id)
#define logger_id_is_enabled(id)                       __logger_id_is_enabled(id)
#define logger_id_level_set(id, level)                 __logger_id_level_set(id, level)
#define logger_id_level_get(id)                        __logger_id_level_get(id)
#ifdef LOGGER_COLORS
#define logger_text_color_set(fg, bg)                  __logger_text_color_set(fg, bg)
#define logger_text_attr_set(attr)                     __logger_text_attr_set(attr)
#define logger_text_reset()                            __logger_text_reset()
#else /* LOGGER_COLORS */
#define logger_text_color_set(fg, bg)                  ((void)(0))
#define logger_text_attr_set(attr)                     ((void)(0))
#define logger_text_reset()                            ((void)(0))
#endif /* LOGGER_COLORS */
#define logger(id, level, format, args ...)            __logger(id, level, format, ## args)
#define logger_verbose(id, level, format, args ...)    __logger(id, level, LOGGER_STRINGIFY(id) ":" LOGGER_STRINGIFY(level) ":" __FILE__ ":" LOGGER_STRINGIFY(__LINE__) ": " format, ## args)

logger_return_t __logger_init(void);
logger_return_t __logger_output_register(FILE *stream);
logger_return_t __logger_output_deregister(FILE *stream);
logger_id_t __logger_id_request(void);
logger_return_t __logger_id_release(logger_id_t id);
logger_return_t __logger_id_enable(logger_id_t id);
logger_return_t __logger_id_disable(logger_id_t id);
logger_bool_t __logger_id_is_enabled(logger_id_t id);
logger_return_t __logger_id_level_set(logger_id_t    id,
                                      logger_level_t level);
logger_level_t __logger_id_level_get(logger_id_t id);
logger_return_t __logger_text_color_set(logger_text_fg_t fg,
                                        logger_text_bg_t bg);
logger_return_t __logger_text_attr_set(logger_text_attr_t attr);
logger_return_t __logger_text_reset(void);
logger_return_t __logger(logger_id_t    id,
                         logger_level_t level,
                         const char     *format,
                         ...);

#else  /* LOGGER_ENABLE */
#define logger_init()                                  ((void)(0))
#define logger_output_register(stream)                 ((void)(0))
#define logger_output_deregister(stream)               ((void)(0))
#define logger_id_request()                            LOGGER_ERR_ID_UNKNOWN
#define logger_id_release(id)                          ((void)(0))
#define logger_id_enable(id)                           ((void)(0))
#define logger_id_disable(id)                          ((void)(0))
#define logger_id_is_enabled(id)                       logger_false
#define logger_id_level_set(id, level)                 ((void)(0))
#define logger_id_level_get(id)                        LOGGER_ERR_LEVEL_UNKNOWN
#define logger_text_color_set(fg, bg)                  ((void)(0))
#define logger_text_attr_set(attr)                     ((void)(0))
#define logger_text_reset()                            ((void)(0))
#define logger(id, level, format, ...)                 ((void)(0))
#define logger_verbose(id, level, format, args ...)    ((void)(0))
#endif /* LOGGER_ENABLE */

#endif /* end of include guard: LOGGER_H */
