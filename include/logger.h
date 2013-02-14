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
 ******************************************************************************/
#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define LOGGER_STRINGIFY_(x)    # x
#define LOGGER_STRINGIFY(x)     LOGGER_STRINGIFY_(x)

#define LOGGER_VERSION_MAJOR    (2)
#define LOGGER_VERSION_MINOR    (0)
#define LOGGER_VERSION_PATCH    (0)
#define LOGGER_VERSION          ((logger_version_t)(((LOGGER_VERSION_MAJOR << 16) & 0x00FF0000) | ((LOGGER_VERSION_MINOR << 8) & 0x0000FF00) | ((LOGGER_VERSION_PATCH) & 0x000000FF)))
#define LOGGER_VERSION_STRING   LOGGER_STRINGIFY(LOGGER_VERSION_MAJOR) "."LOGGER_STRINGIFY (LOGGER_VERSION_MINOR)"."LOGGER_STRINGIFY (LOGGER_VERSION_PATCH)

typedef uint32_t logger_version_t; /**< Logger version type. */

typedef uint8_t  logger_bool_t;            /**< Logger boolean type. */

/** Logger boolean true. */
#define logger_true     ((logger_bool_t)1)

/** Logger boolean false. */
#define logger_false    ((logger_bool_t)0)


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

typedef enum logger_prefix_e {
  LOGGER_PREFIX_UNKNOWN  = 0, /**< Unknown prefix. */
  LOGGER_PREFIX_EMPTY    = 1, /**< Prefix "" */
  LOGGER_PREFIX_NAME     = 2, /**< Prefix "LOGGER_ID_NAME:" */
  LOGGER_PREFIX_SHORT    = 3, /**< Prefix "LOGGER_ID_NAME:LOGGER_LEVEL:" */
  LOGGER_PREFIX_FUNCTION = 4, /**< Prefix "LOGGER_ID_NAME:LOGGER_LEVEL:FUNCTION:LINE:" */
  LOGGER_PREFIX_FILE     = 5, /**< Prefix "LOGGER_ID_NAME:LOGGER_LEVEL:FILE:LINE:" */
  LOGGER_PREFIX_FULL     = 6, /**< Prefix "LOGGER_ID_NAME:LOGGER_LEVEL:FILE:FUNCTION:LINE": */
  LOGGER_PREFIX_MAX      = 7  /**< Last entry, always! */
} logger_prefix_t;

typedef int16_t  logger_id_t;                  /**< Logger ID type. */
#define logger_id_unknown    ((logger_id_t)-1) /**< Unknown logger ID. */


typedef enum logger_return_e {
  LOGGER_OK                    =   0,   /**< Ok. */
  LOGGER_ERR_UNKNOWN           =  -1,   /**< Unspecified error. */
  LOGGER_ERR_OUTPUT_INVALID    =  -2,   /**< Given output stream is invalid. */
  LOGGER_ERR_OUTPUTS_FULL      =  -3,   /**< All available outputs are used. */
  LOGGER_ERR_OUTPUT_REGISTERED =  -4,   /**< Output already registered. */
  LOGGER_ERR_OUTPUT_NOT_FOUND  =  -5,   /**< Output not registered. */
  LOGGER_ERR_IDS_FULL          =  -6,   /**< All available ids are used. */
  LOGGER_ERR_ID_UNKNOWN        =  -7,   /**< Id is unknown. */
  LOGGER_ERR_LEVEL_UNKNOWN     =  -8,   /**< Level is unknown. */
  LOGGER_ERR_PREFIX_UNKNOWN    =  -9,   /**< Prefix is unknown. */
  LOGGER_ERR_OUT_OF_MEMORY     = -10,   /**< Memory allocation error. */
  LOGGER_ERR_STREAM_INVALID    = -11,   /**< File stream is invalid. */
  LOGGER_ERR_NAME_INVALID      = -12,   /**< Name string is invalid. */
  LOGGER_ERR_FORMAT_INVALID    = -13,   /**< Format string is invalid. */
  LOGGER_ERR_FILE_INVALID      = -14    /**< File string is invalid. */
} logger_return_t;


/** enum for text style output. */
typedef enum logger_text_attr_e {
  LOGGER_ATTR_RESET     = 0, /**< Reset attributes. */
  LOGGER_ATTR_BRIGHT    = 1, /**< Bright attribute. */
  LOGGER_ATTR_DIM       = 2, /**< Dim attribute. */
  LOGGER_ATTR_UNDERLINE = 3, /**< Underline attribute. */
  LOGGER_ATTR_BLINK     = 5, /**< Blink attribute. */
  LOGGER_ATTR_REVERSE   = 7, /**< Reverse attribute. */
  LOGGER_ATTR_HIDDEN    = 8  /**< Hidden attribute. */
} logger_text_attr_t;


/** enum for text background color output. */
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


/** enum for text foreground color output. */
typedef enum logger_text_fg_e {
  LOGGER_FG_BLACK   = 30, /**< Black foreground color. */
  LOGGER_FG_RED     = 31, /**< Red foreground color. */
  LOGGER_FG_GREEN   = 32, /**< Green foreground color. */
  LOGGER_FG_YELLOW  = 33, /**< Yellow foreground color. */
  LOGGER_FG_BLUE    = 34, /**< Blue foreground color. */
  LOGGER_FG_MAGENTA = 35, /**< Magenta foreground color. */
  LOGGER_FG_CYAN    = 36, /**< Cyan foreground color. */
  LOGGER_FG_WHITE   = 37  /**< White foreground color. */
} logger_text_fg_t;


#ifdef LOGGER_ENABLE
#define logger_version()                                       __logger_version()
#define logger_init()                                          __logger_init()
#define logger_is_initialized()                                __logger_is_initialized()
#define logger_enable()                                        __logger_enable()
#define logger_disable()                                       __logger_disable()
#define logger_is_enabled()                                    __logger_is_enabled()
#define logger_output_register(__stream)                       __logger_output_register(__stream)
#define logger_output_deregister(__stream)                     __logger_output_deregister(__stream)
#define logger_output_is_registered(__stream)                  __logger_output_is_registered(__stream)
#define logger_output_level_set(__stream, __level)             __logger_output_level_set(__stream, __level)
#define logger_output_level_get(__stream)                      __logger_output_level_get(__stream)
#define logger_output_flush()                                  __logger_output_flush()
#define logger_id_request(__name)                              __logger_id_request(__name)
#define logger_id_release(__id)                                __logger_id_release(__id)
#define logger_id_enable(__id)                                 __logger_id_enable(__id)
#define logger_id_disable(__id)                                __logger_id_disable(__id)
#define logger_id_is_enabled(__id)                             __logger_id_is_enabled(__id)
#define logger_id_level_set(__id, __level)                     __logger_id_level_set(__id, __level)
#define logger_id_level_get(__id)                              __logger_id_level_get(__id)
#define logger_id_prefix_set(__id, __prefix)                   __logger_id_prefix_set(__id, __prefix)
#define logger_id_prefix_get(__id)                             __logger_id_prefix_get(__id)
#define logger_id_name_get(__id)                               __logger_id_name_get(__id)
#define logger_id_output_register(__id, __stream)              __logger_id_output_register(__id, __stream)
#define logger_id_output_deregister(__id, __stream)            __logger_id_output_deregister(__id, __stream)
#define logger_id_output_is_registered(__id, __stream)         __logger_id_output_is_registered(__id, __stream)
#define logger_id_output_level_set(__id, __stream, __level)    __logger_id_output_level_set(__id, __stream, __level)
#define logger_id_output_level_get(__id, __stream)             __logger_id_output_level_get(__id, __stream)
#define logger_color_prefix_disable()                          __logger_color_prefix_disable()
#define logger_color_prefix_enable()                           __logger_color_prefix_enable()
#define logger_color_prefix_is_enabled()                       __logger_color_prefix_is_enabled()
#define logger_color_message_enable()                          __logger_color_message_enable()
#define logger_color_message_disable()                         __logger_color_message_disable()
#define logger_color_message_is_enabled()                      __logger_color_message_is_enabled()
#define logger_color_set(__id, __fg, __bg, __attr)             __logger_color_set(__id, __fg, __bg, __attr)
#define logger_color_reset(__id)                               __logger_color_reset(__id)
#define logger_level_name_get(__level)                         __logger_level_name_get(__level)
#define logger(__id, __level, ...)                             __logger(__id, __level, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

logger_version_t __logger_version(void);
logger_return_t __logger_init(void);
logger_bool_t __logger_is_initialized(void);
logger_return_t __logger_enable(void);
logger_return_t __logger_disable(void);
logger_bool_t __logger_is_enabled(void);
logger_return_t __logger_output_register(FILE *stream);
logger_return_t __logger_output_deregister(FILE *stream);
logger_bool_t __logger_output_is_registered(FILE *stream);
logger_return_t __logger_output_level_set(FILE                 *stream,
                                          const logger_level_t level);
logger_level_t __logger_output_level_get(FILE *stream);
logger_return_t __logger_output_flush(void);
logger_id_t __logger_id_request(const char *name);
logger_return_t __logger_id_release(const logger_id_t id);
logger_return_t __logger_id_enable(const logger_id_t id);
logger_return_t __logger_id_disable(const logger_id_t id);
logger_bool_t __logger_id_is_enabled(const logger_id_t id);
logger_return_t __logger_id_level_set(const logger_id_t    id,
                                      const logger_level_t level);
logger_level_t __logger_id_level_get(const logger_id_t id);
logger_return_t __logger_id_prefix_set(const logger_id_t     id,
                                       const logger_prefix_t prefix);
logger_prefix_t __logger_id_prefix_get(const logger_id_t id);
const char *__logger_id_name_get(const logger_id_t id);
logger_return_t __logger_id_output_register(const logger_id_t id,
                                            FILE              *stream);
logger_return_t __logger_id_output_deregister(const logger_id_t id,
                                              FILE              *stream);
logger_return_t __logger_id_output_is_registered(const logger_id_t id,
                                                 FILE              *stream);
logger_return_t __logger_id_output_level_set(const logger_id_t    id,
                                             FILE                 *stream,
                                             const logger_level_t level);
logger_level_t __logger_id_output_level_get(const logger_id_t id,
                                            FILE              *stream);
logger_return_t __logger_color_prefix_enable(void);
logger_return_t __logger_color_prefix_disable(void);
logger_bool_t __logger_color_prefix_is_enabled(void);
logger_return_t __logger_color_message_enable(void);
logger_return_t __logger_color_message_disable(void);
logger_bool_t __logger_color_message_is_enabled(void);
logger_return_t __logger_color_set(const logger_id_t        id,
                                   const logger_text_fg_t   fg,
                                   const logger_text_bg_t   bg,
                                   const logger_text_attr_t attr);
logger_return_t __logger_color_reset(const logger_id_t id);
const char *__logger_level_name_get(const logger_level_t level);
logger_return_t __logger(logger_id_t    id,
                         logger_level_t level,
                         const char     *file,
                         const char     *function,
                         uint32_t       line,
                         const char     *format,
                         ...) __attribute__((format(printf, 6, 7)));

#else  /* LOGGER_ENABLE */
static inline logger_return_t __logger_ignore_ok(void)
{
  return(LOGGER_OK);
}


static inline logger_return_t __logger_ignore_err(void)
{
  return(LOGGER_ERR_UNKNOWN);
}


static inline logger_id_t __logger_ignore_id(void)
{
  return(0);
}


static inline logger_bool_t __logger_ignore_true(void)
{
  return(logger_true);
}


static inline logger_bool_t __logger_ignore_false(void)
{
  return(logger_false);
}


#define logger_version()                                      (LOGGER_VERSION)
#define logger_init()                                         __logger_ignore_ok()
#define logger_is_initialized()                               __logger_ignore_ok()
#define logger_enable()                                       __logger_ignore_ok()
#define logger_disable()                                      __logger_ignore_ok()
#define logger_is_enabled(__id)                               __logger_ignore_false()
#define logger_output_register(__stream)                      __logger_ignore_ok()
#define logger_output_deregister(__stream)                    __logger_ignore_ok()
#define logger_output_is_registered(__stream)                 __logger_ignore_ok()
#define logger_output_level_set(__stream, __level)            __logger_ignore_ok()
#define logger_output_level_get(__stream)                     __logger_ignore_err()
#define logger_output_flush()                                 __logger_ignore_ok()
#define logger_id_request(__name)                             __logger_ignore_id()
#define logger_id_release(__id)                               __logger_ignore_ok()
#define logger_id_enable(__id)                                __logger_ignore_ok()
#define logger_id_disable(__id)                               __logger_ignore_ok()
#define logger_id_is_enabled(__id)                            __logger_ignore_false()
#define logger_id_level_set(__id, __level)                    __logger_ignore_ok()
#define logger_id_level_get(__id)                             __logger_ignore_err()
#define logger_id_prefix_set(__id, __prefix)                  __logger_ignore_ok()
#define logger_id_prefix_get(__id)                            __logger_ignore_err()
#define logger_id_name_get(__id)                              ""
#define logger_id_output_register(__id, __stream)             __logger_ignore_ok()
#define logger_id_output_deregister(__id, __stream)           __logger_ignore_ok()
#define logger_id_output_is_registered(__id, __stream)        __logger_ignore_ok()
#define logger_id_output_level_set(_id, __stream, __level)    __logger_ignore_ok()
#define logger_id_output_level_get(_id, __stream)             __logger_ignore_err()
#define logger_color_prefix_enable()                          __logger_ignore_ok()
#define logger_color_prefix_disable()                         __logger_ignore_ok()
#define logger_color_prefix_is_enabled()                      __logger_ignore_ok()
#define logger_color_message_enable()                         __logger_ignore_ok()
#define logger_color_message_disable()                        __logger_ignore_ok()
#define logger_color_message_is_enabled()                     __logger_ignore_ok()
#define logger_color_set(__id, __fg, __bg, __attr)            __logger_ignore_ok()
#define logger_color_reset(__id)                              __logger_ignore_ok()
#define logger_level_name_get(__level)                        ""
#define logger(__id, __level, ...)                            __logger_ignore_ok()
#endif /* LOGGER_ENABLE */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* end of include guard: LOGGER_H */
