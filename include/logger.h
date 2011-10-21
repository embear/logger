/***************************************************************************//**
 *     __
 *    / /___  ____ _____ ____  _____
 *   / / __ \/ __ `/ __ `/ _ \/ ___/
 *  / / /_/ / /_/ / /_/ /  __/ /
 * /_/\____/\__, /\__, /\___/_/
 *         /____//____/
 *
 * \file   logger.h
 *
 * \brief  Logging facility for C.
 * \author Markus Braun
 * \date   2010-10-13
 ******************************************************************************/

/***************************************************************************//**
 * \mainpage
 *
 * \section sec_intro Introduction
 *
 * Logger is a simple logging facility for the C language.
 * It is possible to output logging information to different output stream and
 * to use different logging severities.
 *
 * \section sec_usage Usage
 *
 * Logger needs to be enabled using the global define \c LOGGER_ENABLE.
 * Otherwise all functions will be excluded from compilation.
 *
 * To get color support \c LOGGER_COLORS needs to be defined.
 *
 * Basic usage of logger:
 *
 * \code
 * #include "logger.h"
 *
 * int main(void)
 * {
 *   FILE        *stream;
 *   logger_id_t id = logger_id_unknown;
 *
 *   // check logger version
 *   if(LOGGER_VERSION != logger_version()) {
 *     return(1);
 *   }
 *
 *   // initialize logger
 *   logger_init();
 *
 *   // open a file as output for all messages
 *   stream = fopen("logfile", "w");
 *   logger_output_register(stream);
 *   logger_output_level_set(stream, LOGGER_DEBUG);
 *
 *   // open stdout as output for messages above LOGGER_ERR
 *   logger_output_register(stdout);
 *   logger_output_level_set(stdout, LOGGER_ERR);
 *
 *   // get a logging id, enable it and set log level
 *   id = logger_id_request();
 *   logger_id_enable(id);
 *   logger_id_level_set(id, LOGGER_DEBUG);
 *
 *   // do the logging
 *   logger(id, LOGGER_DEBUG,    "id %d - LOGGER_DEBUG   in line %d\n", id, __LINE__);
 *   logger(id, LOGGER_INFO,     "id %d - LOGGER_INFO    in line %d\n", id, __LINE__);
 *   logger(id, LOGGER_NOTICE,   "id %d - LOGGER_NOTICE  in line %d\n", id, __LINE__);
 *   logger(id, LOGGER_WARNING,  "id %d - LOGGER_WARNING in line %d\n", id, __LINE__);
 *   logger(id, LOGGER_ERR,      "id %d - LOGGER_ERR     in line %d\n", id, __LINE__);
 *   logger(id, LOGGER_CRIT,     "id %d - LOGGER_CRIT    in line %d\n", id, __LINE__);
 *   logger(id, LOGGER_ALERT,    "id %d - LOGGER_ALERT   in line %d\n", id, __LINE__);
 *   logger(id, LOGGER_EMERG,    "id %d - LOGGER_EMERG   in line %d\n", id, __LINE__);
 *
 *   // release id
 *   logger_id_release(id);
 *
 *   // deregister stdout output
 *   logger_output_deregister(stdout);
 *
 *   // deregister file output
 *   logger_output_deregister(stream);
 *
 *   // close file
 *   fclose(stream);
 *
 *   return(0);
 * }
 * \endcode
 *
 * The format of the output could be changed by setting on of the following defines on compile time:
 *
 *   - \c LOGGER_FORMAT_SIMPLE (default)
 *   - \c LOGGER_FORMAT_FULL
 *   - \c LOGGER_FORMAT_FILE
 *   - \c LOGGER_FORMAT_FUNCTION
 *
 */
#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define LOGGER_VERSION_MAJOR              (1)
#define LOGGER_VERSION_MINOR              (0)
#define LOGGER_VERSION_PATCH              (1)
#define LOGGER_VERSION                    (((LOGGER_VERSION_MAJOR << 16) & 0x00FF0000) | ((LOGGER_VERSION_MINOR << 8) & 0x0000FF00) | ((LOGGER_VERSION_PATCH) & 0x000000FF))

typedef unsigned int logger_version_t; /**< Logger version type. */


typedef unsigned char  logger_bool_t;                       /**< Logger boolean type. */
static const logger_bool_t logger_true  = (logger_bool_t)1; /**< Logger boolean true. */
static const logger_bool_t logger_false = (logger_bool_t)0; /**< Logger boolean false. */


typedef enum logger_level_e {
  LOGGER_UNKNOWN = 0,  /**< Unknown level. */
  LOGGER_DEBUG   = 1,  /**< Debug-level message. */
  LOGGER_INFO    = 2,  /**< Informational message. */
  LOGGER_NOTICE  = 3,  /**< Normal, but significant, condition. */
  LOGGER_WARNING = 4,  /**< Warning conditions. */
  LOGGER_ERR     = 5,  /**< Error conditions. */
  LOGGER_CRIT    = 6,  /**< Critical conditions. */
  LOGGER_ALERT   = 7,  /**< Action must be taken immediately. */
  LOGGER_EMERG   = 8,  /**< System is unusable. */
  LOGGER_MAX     = 9   /**< Last entry, always! */
} logger_level_t;


typedef int  logger_id_t;                                     /**< Logger id type. */
static const logger_id_t logger_id_unknown = (logger_id_t)-1; /**< Unknown logger id. */


typedef enum logger_return_e {
  LOGGER_OK                    = 0,  /**< Ok. */
  LOGGER_ERR_OUTPUT_INVALID    = -1, /**< Given output stream is invalid. */
  LOGGER_ERR_OUTPUTS_FULL      = -2, /**< All available outputs are used. */
  LOGGER_ERR_OUTPUT_REGISTERED = -3, /**< Output already registered. */
  LOGGER_ERR_OUTPUT_NOT_FOUND  = -4, /**< Output not registered. */
  LOGGER_ERR_IDS_FULL          = -5, /**< All available ids are used. */
  LOGGER_ERR_ID_NOT_FOUND      = -6, /**< Id not registered. */
  LOGGER_ERR_ID_UNKNOWN        = -7, /**< Id is unknown. */
  LOGGER_ERR_LEVEL_UNKNOWN     = -8, /**< Level is unknown. */
} logger_return_t;

/* macros for color text output. */
typedef enum logger_text_attr_e {
  LOGGER_ATTR_RESET     = 0, /**< Reset attributes. */
  LOGGER_ATTR_BRIGHT    = 1, /**< Bright attribute. */
  LOGGER_ATTR_DIM       = 2, /**< Dim attribute. */
  LOGGER_ATTR_UNDERLINE = 3, /**< Underline attribute. */
  LOGGER_ATTR_BLINK     = 5, /**< Blink attribute. */
  LOGGER_ATTR_REVERSE   = 7, /**< Reverse attribute. */
  LOGGER_ATTR_HIDDEN    = 8  /**< Hidden attribute. */
} logger_text_attr_t;

typedef enum logger_text_bg_e {
  LOGGER_BG_BLACK   = 40, /**< Black background color. */
  LOGGER_BG_RED     = 41, /**< Red background color. */
  LOGGER_BG_GREEN   = 42, /**< Green background color. */
  LOGGER_BG_YELLOW  = 43, /**< Yellow background color. */
  LOGGER_BG_BLUE    = 44, /**< Blue background color. */
  LOGGER_BG_MAGENTA = 45, /**< Magenta background color. */
  LOGGER_BG_CYAN    = 46, /**< Cyan background color. */
  LOGGER_BG_WHITE   = 47  /**< White background color. */
} logger_text_bg_t;

typedef enum logger_text_fg_e {
  LOGGER_FG_BLACK   = 30, /**< Black background color. */
  LOGGER_FG_RED     = 31, /**< Red background color. */
  LOGGER_FG_GREEN   = 32, /**< Green background color. */
  LOGGER_FG_YELLOW  = 33, /**< Yellow background color. */
  LOGGER_FG_BLUE    = 34, /**< Blue background color. */
  LOGGER_FG_MAGENTA = 35, /**< Magenta background color. */
  LOGGER_FG_CYAN    = 36, /**< Cyan background color. */
  LOGGER_FG_WHITE   = 37  /**< White background color. */
} logger_text_fg_t;


#ifdef LOGGER_ENABLE
#define LOGGER_STRINGIFY_(x)                          # x
#define LOGGER_STRINGIFY(x)                           LOGGER_STRINGIFY_(x)
#define logger_version()                              __logger_version()
#define logger_init()                                 __logger_init()
#define logger_enable()                               __logger_enable()
#define logger_disable()                              __logger_disable()
#define logger_is_enabled()                           __logger_is_enabled()
#define logger_output_register(__stream)              __logger_output_register(__stream)
#define logger_output_deregister(__stream)            __logger_output_deregister(__stream)
#define logger_output_level_set(__stream, __level)    __logger_output_level_set(__stream, __level)
#define logger_output_level_get(__stream)             __logger_output_level_get(__stream)
#define logger_id_request()                           __logger_id_request()
#define logger_id_release(__id)                       __logger_id_release(__id)
#define logger_id_enable(__id)                        __logger_id_enable(__id)
#define logger_id_disable(__id)                       __logger_id_disable(__id)
#define logger_id_is_enabled(__id)                    __logger_id_is_enabled(__id)
#define logger_id_level_set(__id, __level)            __logger_id_level_set(__id, __level)
#define logger_id_level_get(__id)                     __logger_id_level_get(__id)
#ifdef LOGGER_COLORS
#define logger_color_set(__id, __fg, __bg, __attr)    __logger_color_set(__id, __fg, __bg, __attr)
#define logger_color_reset(__id)                      __logger_color_reset(__id)
#else /* LOGGER_COLORS */
#define logger_color_set(__id, __fg, __bg, __attr)    ((void)(0))
#define logger_color_reset(__id)                      ((void)(0))
#endif /* LOGGER_COLORS */

#if defined(LOGGER_FORMAT_FULL)
#define logger(__id, __level, __format, __args ...)                                                                                                                       \
  {                                                                                                                                                                       \
    __logger_prefix(__id, __level, "%15s:%15s:%30s:%30s():%5s: ", LOGGER_STRINGIFY(__id), LOGGER_STRINGIFY(__level), __FILE__, __FUNCTION__, LOGGER_STRINGIFY(__LINE__)); \
    __logger_msg(__id, __level, __format, ## __args);                                                                                                                     \
  }
#elif defined(LOGGER_FORMAT_FILE)
#define logger(__id, __level, __format, __args ...)                                                                                                  \
  {                                                                                                                                                  \
    __logger_prefix(__id, __level, "%15s:%15s:%30s:%5s: ", LOGGER_STRINGIFY(__id), LOGGER_STRINGIFY(__level), __FILE__, LOGGER_STRINGIFY(__LINE__)); \
    __logger_msg(__id, __level, __format, ## __args);                                                                                                \
  }
#elif defined(LOGGER_FORMAT_FUNCTION)
#define logger(__id, __level, __format, __args ...)                                                                                                        \
  {                                                                                                                                                        \
    __logger_prefix(__id, __level, "%15s:%15s:%30s():%5s: ", LOGGER_STRINGIFY(__id), LOGGER_STRINGIFY(__level), __FUNCTION__, LOGGER_STRINGIFY(__LINE__)); \
    __logger_msg(__id, __level, __format, ## __args);                                                                                                      \
  }
#else
#define logger(__id, __level, __format, __args ...)                                                   \
  {                                                                                                   \
    __logger_prefix(__id, __level, "%15s:%15s: ", LOGGER_STRINGIFY(__id), LOGGER_STRINGIFY(__level)); \
    __logger_msg(__id, __level, __format, ## __args);                                                 \
  }
#endif


logger_version_t __logger_version(void);
logger_return_t __logger_init(void);
logger_return_t __logger_enable(void);
logger_return_t __logger_disable(void);
logger_bool_t __logger_is_enabled(void);
logger_return_t __logger_output_register(FILE *stream);
logger_return_t __logger_output_deregister(FILE *stream);
logger_return_t __logger_output_level_set(FILE           *stream,
                                          logger_level_t level);
logger_level_t __logger_output_level_get(FILE *stream);
logger_id_t __logger_id_request(void);
logger_return_t __logger_id_release(logger_id_t id);
logger_return_t __logger_id_enable(logger_id_t id);
logger_return_t __logger_id_disable(logger_id_t id);
logger_bool_t __logger_id_is_enabled(logger_id_t id);
logger_return_t __logger_id_level_set(logger_id_t    id,
                                      logger_level_t level);
logger_level_t __logger_id_level_get(logger_id_t id);
logger_return_t __logger_color_set(logger_id_t        id,
                                   logger_text_fg_t   fg,
                                   logger_text_bg_t   bg,
                                   logger_text_attr_t attr);
logger_return_t __logger_color_reset(logger_id_t id);
logger_return_t __logger_prefix(logger_id_t    id,
                                logger_level_t level,
                                const char     *format,
                                ...);
logger_return_t __logger_msg(logger_id_t    id,
                             logger_level_t level,
                             const char     *format,
                             ...);

#else  /* LOGGER_ENABLE */
#define logger_version()                               (LOGGER_VERSION)
#define logger_init()                                  ((void)(0))
#define logger_enable()                                ((void)(0))
#define logger_disable()                               ((void)(0))
#define logger_is_enabled(__id)                        logger_false
#define logger_output_register(__stream)               ((void)(0))
#define logger_output_deregister(__stream)             ((void)(0))
#define logger_output_level_set(__stream, __level)     ((void)(0))
#define logger_output_level_get(__stream)              LOGGER_ERR_LEVEL_UNKNOWN
#define logger_id_request()                            LOGGER_ERR_ID_UNKNOWN
#define logger_id_release(__id)                        ((void)(0))
#define logger_id_enable(__id)                         ((void)(0))
#define logger_id_disable(__id)                        ((void)(0))
#define logger_id_is_enabled(__id)                     logger_false
#define logger_id_level_set(__id, __level)             ((void)(0))
#define logger_id_level_get(__id)                      LOGGER_ERR_LEVEL_UNKNOWN
#define logger_color_set(__id, __fg, __bg, __attr)     ((void)(0))
#define logger_color_reset(__id)                       ((void)(0))
#define logger(__id, __level, __format, __args ...)    ((void)(0))
#endif /* LOGGER_ENABLE */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* end of include guard: LOGGER_H */
