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

/** Preprocessor sting conversion helper */
#define LOGGER_STRINGIFY_(x)    # x

/** Preprocessor sting conversion helper */
#define LOGGER_STRINGIFY(x)     LOGGER_STRINGIFY_(x)

/** Logger major version number */
#define LOGGER_VERSION_MAJOR    255

/** Logger minor version number */
#define LOGGER_VERSION_MINOR    255

/** Logger patch level */
#define LOGGER_VERSION_PATCH    255

/** Logger version number as integer */
#define LOGGER_VERSION          ((logger_version_t)(((LOGGER_VERSION_MAJOR << 16) & 0x00FF0000) | ((LOGGER_VERSION_MINOR << 8) & 0x0000FF00) | ((LOGGER_VERSION_PATCH) & 0x000000FF)))

/** Logger version number as string */
#define LOGGER_VERSION_STRING   LOGGER_STRINGIFY(LOGGER_VERSION_MAJOR) "." LOGGER_STRINGIFY(LOGGER_VERSION_MINOR) "." LOGGER_STRINGIFY(LOGGER_VERSION_PATCH)

typedef uint32_t logger_version_t; /**< Logger version type. */

typedef uint8_t  logger_bool_t;    /**< Logger boolean type. */

/** Logger boolean true. */
#define logger_true     ((logger_bool_t)1)

/** Logger boolean false. */
#define logger_false    ((logger_bool_t)0)


/** Logger log level types */
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


/** Logger prefix types */
typedef enum logger_prefix_e {
  LOGGER_PREFIX_UNKNOWN                       =  0, /**< Unknown prefix. */
  LOGGER_PREFIX_EMPTY                         =  1, /**< Prefix "" */
  LOGGER_PREFIX_FILE_FUNCTION_LINE            =  2, /**< Prefix "FILE:FUNCTION:LINE:" */
  LOGGER_PREFIX_FILE_LINE                     =  3, /**< Prefix "FILE:LINE:" */
  LOGGER_PREFIX_FUNCTION_LINE                 =  4, /**< Prefix "FUNCTION:LINE:" */
  LOGGER_PREFIX_NAME                          =  5, /**< Prefix "LOGGER_ID_NAME:" */
  LOGGER_PREFIX_NAME_FILE_FUNCTION_LINE       =  6, /**< Prefix "LOGGER_ID_NAME:FILE:FUNCTION:LINE:" */
  LOGGER_PREFIX_NAME_FILE_LINE                =  7, /**< Prefix "LOGGER_ID_NAME:FILE:LINE:" */
  LOGGER_PREFIX_NAME_FUNCTION_LINE            =  8, /**< Prefix "LOGGER_ID_NAME:FUNCTION:LINE:" */
  LOGGER_PREFIX_NAME_LEVEL                    =  9, /**< Prefix "LOGGER_ID_NAME:LOGGER_LEVEL:" */
  LOGGER_PREFIX_NAME_LEVEL_FILE_FUNCTION_LINE = 10, /**< Prefix "LOGGER_ID_NAME:LOGGER_LEVEL:FILE:FUNCTION:LINE": */
  LOGGER_PREFIX_NAME_LEVEL_FILE_LINE          = 11, /**< Prefix "LOGGER_ID_NAME:LOGGER_LEVEL:FILE:LINE:" */
  LOGGER_PREFIX_NAME_LEVEL_FUNCTION_LINE      = 12, /**< Prefix "LOGGER_ID_NAME:LOGGER_LEVEL:FUNCTION:LINE:" */
  LOGGER_PREFIX_MAX                           = 13  /**< Last entry, always! */
} logger_prefix_t;

/** Legacy define, please use LOGGER_PREFIX_NAME_LEVEL instead */
#define LOGGER_PREFIX_SHORT       LOGGER_PREFIX_NAME_LEVEL

/** Legacy define, please use LOGGER_PREFIX_NAME_LEVEL_FUNCTION_LINE instead */
#define LOGGER_PREFIX_FUNCTION    LOGGER_PREFIX_NAME_LEVEL_FUNCTION_LINE

/** Legacy define, please use LOGGER_PREFIX_NAME_LEVEL_FILE_LINE instead */
#define LOGGER_PREFIX_FILE        LOGGER_PREFIX_NAME_LEVEL_FILE_LINE

/** Legacy define, please use LOGGER_PREFIX_NAME_LEVEL_FILE_FUNCTION_LINE instead */
#define LOGGER_PREFIX_FULL        LOGGER_PREFIX_NAME_LEVEL_FILE_FUNCTION_LINE

/** Logger ID type. */
typedef int16_t  logger_id_t;

/** Unknown logger ID. */
#define logger_id_unknown    ((logger_id_t)-1)


/** Logger function return codes */
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
  LOGGER_ERR_TYPE_INVALID      = -11,   /**< Output type is invalid. */
  LOGGER_ERR_STREAM_INVALID    = -12,   /**< File stream is invalid. */
  LOGGER_ERR_FUNCTION_INVALID  = -13,   /**< Function is invalid. */
  LOGGER_ERR_NAME_INVALID      = -14,   /**< Name string is invalid. */
  LOGGER_ERR_FORMAT_INVALID    = -15,   /**< Format string is invalid. */
  LOGGER_ERR_FILE_INVALID      = -16    /**< File string is invalid. */
} logger_return_t;


/** Logger enum for text style output. */
typedef enum logger_text_attr_e {
  LOGGER_ATTR_RESET     = 0, /**< Reset attributes. */
  LOGGER_ATTR_BRIGHT    = 1, /**< Bright attribute. */
  LOGGER_ATTR_DIM       = 2, /**< Dim attribute. */
  LOGGER_ATTR_UNDERLINE = 3, /**< Underline attribute. */
  LOGGER_ATTR_BLINK     = 5, /**< Blink attribute. */
  LOGGER_ATTR_REVERSE   = 7, /**< Reverse attribute. */
  LOGGER_ATTR_HIDDEN    = 8  /**< Hidden attribute. */
} logger_text_attr_t;


/** Logger enum for text background color output. */
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


/** Logger enum for text foreground color output. */
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


/** logger output function type */
typedef void (*logger_output_function_t)(const char *);


#ifdef LOGGER_ENABLE
logger_version_t logger_version(void);
logger_return_t logger_init(void);
logger_bool_t logger_is_initialized(void);
logger_return_t logger_enable(void);
logger_return_t logger_disable(void);
logger_bool_t logger_is_enabled(void);
logger_return_t logger_output_register(FILE *stream);
logger_return_t logger_output_deregister(FILE *stream);
logger_bool_t logger_output_is_registered(FILE *stream);
logger_return_t logger_output_level_set(FILE                 *stream,
                                        const logger_level_t level);
logger_level_t logger_output_level_get(FILE *stream);
logger_return_t logger_output_color_enable(FILE *stream);
logger_return_t logger_output_color_disable(FILE *stream);
logger_bool_t logger_output_color_is_enabled(FILE *stream);
logger_return_t logger_output_flush(void);
logger_return_t logger_output_function_register(logger_output_function_t function);
logger_return_t logger_output_function_deregister(logger_output_function_t function);
logger_bool_t logger_output_function_is_registered(logger_output_function_t function);
logger_return_t logger_output_function_level_set(logger_output_function_t function,
                                                 const logger_level_t     level);
logger_level_t logger_output_function_level_get(logger_output_function_t function);
logger_return_t logger_output_function_color_enable(logger_output_function_t function);
logger_return_t logger_output_function_color_disable(logger_output_function_t function);
logger_bool_t logger_output_function_color_is_enabled(logger_output_function_t function);
logger_id_t logger_id_request(const char *name);
logger_return_t logger_id_release(const logger_id_t id);
logger_return_t logger_id_enable(const logger_id_t id);
logger_return_t logger_id_disable(const logger_id_t id);
logger_bool_t logger_id_is_enabled(const logger_id_t id);
logger_return_t logger_id_level_set(const logger_id_t    id,
                                    const logger_level_t level);
logger_level_t logger_id_level_get(const logger_id_t id);
logger_return_t logger_id_prefix_set(const logger_id_t     id,
                                     const logger_prefix_t prefix);
logger_prefix_t logger_id_prefix_get(const logger_id_t id);
const char *logger_id_name_get(const logger_id_t id);
logger_return_t logger_id_output_register(const logger_id_t id,
                                          FILE              *stream);
logger_return_t logger_id_output_deregister(const logger_id_t id,
                                            FILE              *stream);
logger_bool_t logger_id_output_is_registered(const logger_id_t id,
                                             FILE              *stream);
logger_return_t logger_id_output_level_set(const logger_id_t    id,
                                           FILE                 *stream,
                                           const logger_level_t level);
logger_level_t logger_id_output_level_get(const logger_id_t id,
                                          FILE              *stream);
logger_return_t logger_id_output_color_enable(const logger_id_t id,
                                              FILE              *stream);
logger_return_t logger_id_output_color_disable(const logger_id_t id,
                                               FILE              *stream);
logger_bool_t logger_id_output_color_is_enabled(const logger_id_t id,
                                                FILE              *stream);
logger_return_t logger_id_output_function_register(const logger_id_t        id,
                                                   logger_output_function_t function);
logger_return_t logger_id_output_function_deregister(const logger_id_t        id,
                                                     logger_output_function_t function);
logger_bool_t logger_id_output_function_is_registered(const logger_id_t        id,
                                                      logger_output_function_t function);
logger_return_t logger_id_output_function_level_set(const logger_id_t        id,
                                                    logger_output_function_t function,
                                                    const logger_level_t     level);
logger_level_t logger_id_output_function_level_get(const logger_id_t        id,
                                                   logger_output_function_t function);
logger_return_t logger_id_output_function_color_enable(const logger_id_t        id,
                                                       logger_output_function_t function);
logger_return_t logger_id_output_function_color_disable(const logger_id_t        id,
                                                        logger_output_function_t function);
logger_bool_t logger_id_output_function_color_is_enabled(const logger_id_t        id,
                                                         logger_output_function_t function);
logger_return_t logger_color_prefix_enable(void);
logger_return_t logger_color_prefix_disable(void);
logger_bool_t logger_color_prefix_is_enabled(void);
logger_return_t logger_color_message_enable(void);
logger_return_t logger_color_message_disable(void);
logger_bool_t logger_color_message_is_enabled(void);
logger_return_t logger_color_set(const logger_id_t        id,
                                 const logger_text_fg_t   fg,
                                 const logger_text_bg_t   bg,
                                 const logger_text_attr_t attr);
logger_return_t logger_color_reset(const logger_id_t id);
const char *logger_level_name_get(const logger_level_t level);
logger_return_t logger_implementation(logger_id_t    id,
                                      logger_level_t level,
                                      const char     *file,
                                      const char     *function,
                                      uint32_t       line,
                                      const char     *format,
                                      ...) __attribute__((format(printf, 6, 7)));

/** macro to call the real logger function logger() with the information about the current position in code (file, function and line) */
#define logger(__id, __level, ...)   logger_implementation(__id, __level, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

#else  /* LOGGER_ENABLE */
static inline logger_return_t logger_ignore_ok(void)
{
  return(LOGGER_OK);
}


static inline logger_return_t logger_ignore_err(void)
{
  return(LOGGER_ERR_UNKNOWN);
}


static inline logger_id_t logger_ignore_id(void)
{
  return(0);
}


static inline logger_bool_t logger_ignore_true(void)
{
  return(logger_true);
}


static inline logger_bool_t logger_ignore_false(void)
{
  return(logger_false);
}


#define logger_version()                                                (LOGGER_VERSION)
#define logger_init()                                                   logger_ignore_ok()
#define logger_is_initialized()                                         logger_ignore_ok()
#define logger_enable()                                                 logger_ignore_ok()
#define logger_disable()                                                logger_ignore_ok()
#define logger_is_enabled(__id)                                         logger_ignore_false()
#define logger_output_register(__stream)                                logger_ignore_ok()
#define logger_output_deregister(__stream)                              logger_ignore_ok()
#define logger_output_is_registered(__stream)                           logger_ignore_ok()
#define logger_output_level_set(__stream, __level)                      logger_ignore_ok()
#define logger_output_level_get(__stream)                               logger_ignore_err()
#define logger_output_color_enable(__stream)                            logger_ignore_ok()
#define logger_output_color_disable(__stream)                           logger_ignore_ok()
#define logger_output_flush()                                           logger_ignore_ok()
#define logger_output_function_register(__function)                     logger_ignore_ok()
#define logger_output_function_deregister(__function)                   logger_ignore_ok()
#define logger_output_function_is_registered(__function)                logger_ignore_ok()
#define logger_output_function_level_set(__function, __level)           logger_ignore_ok()
#define logger_output_function_level_get(__function)                    logger_ignore_err()
#define logger_output_function_color_enable(__function)                 logger_ignore_ok()
#define logger_output_function_color_disable(__function)                logger_ignore_ok()
#define logger_id_request(__name)                                       logger_ignore_id()
#define logger_id_release(__id)                                         logger_ignore_ok()
#define logger_id_enable(__id)                                          logger_ignore_ok()
#define logger_id_disable(__id)                                         logger_ignore_ok()
#define logger_id_is_enabled(__id)                                      logger_ignore_false()
#define logger_id_level_set(__id, __level)                              logger_ignore_ok()
#define logger_id_level_get(__id)                                       logger_ignore_err()
#define logger_id_prefix_set(__id, __prefix)                            logger_ignore_ok()
#define logger_id_prefix_get(__id)                                      logger_ignore_err()
#define logger_id_name_get(__id)                                        ""
#define logger_id_output_register(__id, __stream)                       logger_ignore_ok()
#define logger_id_output_deregister(__id, __stream)                     logger_ignore_ok()
#define logger_id_output_is_registered(__id, __stream)                  logger_ignore_ok()
#define logger_id_output_level_set(__id, __stream, __level)             logger_ignore_ok()
#define logger_id_output_level_get(__id, __stream)                      logger_ignore_err()
#define logger_id_output_color_enable(__id, __stream)                   logger_ignore_ok()
#define logger_id_output_color_disable(__id, __stream)                  logger_ignore_ok()
#define logger_id_output_function_register(__id, __function)            logger_ignore_ok()
#define logger_id_output_function_deregister(__id, __function)          logger_ignore_ok()
#define logger_id_output_function_is_registered(__id, __function)       logger_ignore_ok()
#define logger_id_output_function_level_set(__id, __function, __level)  logger_ignore_ok()
#define logger_id_output_function_level_get(__id, __function)           logger_ignore_err()
#define logger_id_output_function_color_enable(__id, __function)        logger_ignore_ok()
#define logger_id_output_function_color_disable(__id, __function)       logger_ignore_ok()
#define logger_color_prefix_enable()                                    logger_ignore_ok()
#define logger_color_prefix_disable()                                   logger_ignore_ok()
#define logger_color_prefix_is_enabled()                                logger_ignore_ok()
#define logger_color_message_enable()                                   logger_ignore_ok()
#define logger_color_message_disable()                                  logger_ignore_ok()
#define logger_color_message_is_enabled()                               logger_ignore_ok()
#define logger_color_set(__id, __fg, __bg, __attr)                      logger_ignore_ok()
#define logger_color_reset(__id)                                        logger_ignore_ok()
#define logger_level_name_get(__level)                                  ""
#define logger(__id, __level, ...)                                      logger_ignore_ok()
#endif /* LOGGER_ENABLE */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* end of include guard: LOGGER_H */
