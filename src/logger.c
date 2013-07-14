/***************************************************************************//**
 *     __
 *    / /___  ____ _____ ____  _____
 *   / / __ \/ __ `/ __ `/ _ \/ ___/
 *  / / /_/ / /_/ / /_/ /  __/ /
 * /_/\____/\__, /\__, /\___/_/
 *         /____//____/
 *
 * \file   logger.c
 *
 * \brief  Logging facility for C.
 * \author Markus Braun
 ******************************************************************************/
#include "logger.h"

#ifdef LOGGER_ENABLE
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <stddef.h>
#include <fcntl.h>

/** Number of possible simultaneous outputs. */
#ifndef LOGGER_OUTPUTS_MAX
#define LOGGER_OUTPUTS_MAX             (16)
#endif /* LOGGER_OUTPUTS_MAX */

/** Number of possible outputs per ID. */
#ifndef LOGGER_ID_OUTPUTS_MAX
#define LOGGER_ID_OUTPUTS_MAX          (4)
#endif /* LOGGER_ID_OUTPUTS_MAX */

/** Number of possible IDs. */
#ifndef LOGGER_IDS_MAX
#define LOGGER_IDS_MAX                 (128)
#endif /* LOGGER_IDS_MAX */

/** Length of logger ID name including '\0' */
#ifndef LOGGER_NAME_MAX
#define LOGGER_NAME_MAX                (256)
#endif /* LOGGER_NAME_MAX */
#if (LOGGER_NAME_MAX < 1)
#error "LOGGER_NAME_MAX must be at least 1"
#endif /* (LOGGER_NAME_MAX < 1) */

/** Length of logger color string including '\0' */
#ifndef LOGGER_COLOR_STRING_MAX
#define LOGGER_COLOR_STRING_MAX        (16)
#endif /* LOGGER_COLOR_STRING_MAX */
#if (LOGGER_COLOR_STRING_MAX < 1)
#error "LOGGER_COLOR_STRING_MAX must be at least 1"
#endif /* (LOGGER_COLOR_STRING_MAX < 1) */

/** Length of logger prefix string including '\0' */
#ifndef LOGGER_PREFIX_STRING_MAX
#define LOGGER_PREFIX_STRING_MAX       (256)
#endif /* LOGGER_PREFIX_STRING_MAX */
#if (LOGGER_PREFIX_STRING_MAX < 5)
#error "LOGGER_PREFIX_STRING_MAX must be at least 5"
#endif /* (LOGGER_PREFIX_STRING_MAX < 5) */

/** Length of logger message string including '\0' */
#ifndef LOGGER_MESSAGE_STRING_MAX
#define LOGGER_MESSAGE_STRING_MAX      (256)
#endif /* LOGGER_MESSAGE_STRING_MAX */
#if (LOGGER_MESSAGE_STRING_MAX < 6)
#error "LOGGER_MESSAGE_STRING_MAX must be at least 6"
#endif /* (LOGGER_MESSAGE_STRING_MAX < 6) */

/** Length of logger line string including '\0' */
#ifndef LOGGER_LINE_STRING_MAX
#define LOGGER_LINE_STRING_MAX      (LOGGER_COLOR_STRING_MAX + LOGGER_PREFIX_STRING_MAX + LOGGER_COLOR_STRING_MAX + LOGGER_COLOR_STRING_MAX + LOGGER_MESSAGE_STRING_MAX + LOGGER_COLOR_STRING_MAX)
#endif /* LOGGER_LINE_STRING_MAX */
#if (LOGGER_LINE_STRING_MAX < 5)
#error "LOGGER_LINE_STRING_MAX must be at least 5"
#endif /* (LOGGER_LINE_STRING_MAX < 5) */

/** Standard prefix */
#ifndef LOGGER_PREFIX_STANDARD
#define LOGGER_PREFIX_STANDARD       LOGGER_PREFIX_NAME_LEVEL_FUNCTION_LINE
#endif /* LOGGER_PREFIX_STANDARD */

/** Format sting for prefix file name */
#define LOGGER_FORMAT_STRING_FILE      "%30s"

/** Format sting for prefix function name */
#define LOGGER_FORMAT_STRING_FUNCTION  "%30s()"

/** Format sting for prefix line number */
#define LOGGER_FORMAT_STRING_LINE      "%5d"

/** Format sting for prefix logger id name */
#define LOGGER_FORMAT_STRING_NAME      "%15s"

/** Format sting for prefix logger id level name */
#define LOGGER_FORMAT_STRING_LEVEL     "%7s"


/** Logger output type */
typedef enum logger_output_type_e {
  LOGGER_OUTPUT_TYPE_UNKNOWN    = 0, /**< Output type is unknown */
  LOGGER_OUTPUT_TYPE_FILESTREAM = 1, /**< Output is a file stream of type (FILE *). */
  LOGGER_OUTPUT_TYPE_FUNCTION   = 2, /**< Output is a user provided function. */
  LOGGER_OUTPUT_TYPE_MAX             /**< Last entry, always! */
} logger_output_type_t;


/** Logger output structure */
typedef struct logger_output_s {
  int16_t                  count;       /**< Number of registrations for this output. */
  logger_level_t           level;       /**< Level for this output. */
  logger_bool_t            use_color;   /**< Use color strings for this output. */
  logger_output_type_t     type;        /**< Selector for this output. */
  FILE                     *stream;     /**< File pointer given during registration. */
  logger_output_function_t function;    /**< User function given during registration. */
} logger_output_t;


/** Logger color definition */
typedef struct logger_color_string_s {
  char begin[LOGGER_COLOR_STRING_MAX]; /**< Color begin marker string. */
  char end[LOGGER_COLOR_STRING_MAX];   /**< Color end marker string. */
} logger_color_string_t;


/** Logger control structure */
typedef struct  logger_control_s {
  logger_bool_t         used;                           /**< This ID is used. */
  int16_t               count;                          /**< Number of registrations for this ID. */
  logger_bool_t         enabled;                        /**< This ID is enabled. */
  logger_level_t        level;                          /**< Level for this ID. */
  logger_prefix_t       prefix;                         /**< Prefix for this ID. */
  logger_bool_t         color;                          /**< Changed colors for this ID. */
  logger_color_string_t color_string;                   /**< Color string for this ID. */
  logger_bool_t         color_string_changed;           /**< Color string had been changed for this ID. */
  logger_bool_t         append;                         /**< Previous message didn't contain a newline just append next message */
  char                  name[LOGGER_NAME_MAX];          /**< Name of this logger ID. */
  logger_output_t       outputs[LOGGER_ID_OUTPUTS_MAX]; /**< Storage for possible ID outputs. */
} logger_control_t;


static logger_bool_t    logger_initialized           = logger_false; /**< logger is initialized. */
static logger_bool_t    logger_enabled               = logger_false; /**< Logger is enabled. */
static logger_bool_t    logger_color_prefix_enabled  = logger_false; /**< Logger prefix color is enabled. */
static logger_bool_t    logger_color_message_enabled = logger_false; /**< Logger message color is enabled. */
static logger_control_t logger_control[LOGGER_IDS_MAX];              /**< Control storage for possible IDs. */
static logger_output_t  logger_outputs[LOGGER_OUTPUTS_MAX];          /**< Storage for possible outputs. */
static char             logger_prefix[LOGGER_PREFIX_STRING_MAX];     /**< Storage for prefix string */
static char             logger_message[LOGGER_MESSAGE_STRING_MAX];   /**< Storage for message string */
static char             logger_line[LOGGER_LINE_STRING_MAX];         /**< Storage for whole output line */


/** level to name translation */
static const char *logger_level_names[] =
{
  "UNKNOWN", /**< Name for level "UNKNOWN" == 0 */
  "DEBUG",   /**< Name for level "DEBUG"   == 1 */
  "INFO",    /**< Name for level "INFO"    == 2 */
  "NOTICE",  /**< Name for level "NOTICE"  == 3 */
  "WARNING", /**< Name for level "WARNING" == 4 */
  "ERR",     /**< Name for level "ERR"     == 5 */
  "CRIT",    /**< Name for level "CRIT"    == 6 */
  "ALERT",   /**< Name for level "ALERT"   == 7 */
  "EMERG"    /**< Name for level "EMERG"   == 8 */
};


/** level to color translation */
static logger_color_string_t logger_level_colors[] =
{
  { "\x1B[0;37;40m", "\x1B[0m" }, /* Prefix color string for level "UNKNOWN" == 0 -> LOGGER_BG_BLACK,   LOGGER_FG_WHITE, LOGGER_ATTR_RESET */
  { "\x1B[0;37;40m", "\x1B[0m" }, /* Prefix color string for level "DEBUG"   == 1 -> LOGGER_BG_BLACK,   LOGGER_FG_WHITE, LOGGER_ATTR_RESET */
  { "\x1B[0;30;47m", "\x1B[0m" }, /* Prefix color string for level "INFO"    == 2 -> LOGGER_BG_WHITE,   LOGGER_FG_BLACK, LOGGER_ATTR_RESET */
  { "\x1B[0;30;44m", "\x1B[0m" }, /* Prefix color string for level "NOTICE"  == 3 -> LOGGER_BG_BLUE,    LOGGER_FG_BLACK, LOGGER_ATTR_RESET */
  { "\x1B[0;30;46m", "\x1B[0m" }, /* Prefix color string for level "WARNING" == 4 -> LOGGER_BG_CYAN,    LOGGER_FG_BLACK, LOGGER_ATTR_RESET */
  { "\x1B[0;30;42m", "\x1B[0m" }, /* Prefix color string for level "ERR"     == 5 -> LOGGER_BG_GREEN,   LOGGER_FG_BLACK, LOGGER_ATTR_RESET */
  { "\x1B[0;30;43m", "\x1B[0m" }, /* Prefix color string for level "CRIT"    == 6 -> LOGGER_BG_YELLOW,  LOGGER_FG_BLACK, LOGGER_ATTR_RESET */
  { "\x1B[0;30;45m", "\x1B[0m" }, /* Prefix color string for level "ALERT"   == 7 -> LOGGER_BG_MAGENTA, LOGGER_FG_BLACK, LOGGER_ATTR_RESET */
  { "\x1B[0;30;41m", "\x1B[0m" }  /* Prefix color string for level "EMERG"   == 8 -> LOGGER_BG_RED,     LOGGER_FG_BLACK, LOGGER_ATTR_RESET */
};


/** ************************************************************************//**
 * \brief  Version of logger.
 *
 * Get the version of logger for which it was compiled. Needed to check for
 * correct version of logger when linking against a precompiled library.
 *
 * \return        Version of logger
 ******************************************************************************/
logger_version_t __logger_version(void)
{
  return(LOGGER_VERSION);
}


/** ************************************************************************//**
 * \brief  Initialize logger.
 *
 * Initialize logger internal structures. Needs to be called once before any
 * other logger function is called.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_init(void)
{
  logger_return_t ret = LOGGER_OK;

  if (logger_initialized == logger_false) {
    logger_initialized           = logger_true;
    logger_enabled               = logger_true;
    logger_color_prefix_enabled  = logger_false;
    logger_color_message_enabled = logger_false;
    memset(logger_control, 0, sizeof(logger_control));
    memset(logger_outputs, 0, sizeof(logger_outputs));
    memset(logger_message, 0, sizeof(logger_message));
    memset(logger_prefix, 0, sizeof(logger_prefix));
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Query the current initialization state
 *
 * Query the current initialization state of logger.
 *
 * \return        \c logger_true if logger is initialized, logger_false otherwise
 ******************************************************************************/
logger_bool_t __logger_is_initialized(void)
{
  return(logger_initialized);
}


/** ************************************************************************//**
 * \brief  Enable logger.
 *
 * Enable logger globally for all IDs and all outputs.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_enable(void)
{
  logger_return_t ret = LOGGER_OK;

  logger_enabled = logger_true;

  return(ret);
}


/** ************************************************************************//**
 * \brief  Disable logger.
 *
 * Disable logger globally for all IDs and all outputs.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_disable(void)
{
  logger_return_t ret = LOGGER_OK;

  logger_enabled = logger_false;

  return(ret);
}


/** ************************************************************************//**
 * \brief  Query the current enable state of logger.
 *
 * Query the current global enable state of logger.
 *
 * \return        \c logger_true if logger is enabled, logger_false otherwise
 ******************************************************************************/
logger_bool_t __logger_is_enabled()
{
  return(logger_enabled);
}


/** ************************************************************************//**
 * \brief  Register an output stream to a list of outputs.
 *
 * The given file stream may be on of \c stdout, \c stderr or a file stream
 * opened by the user. The default logging level is set to \c LOGGER_UNKNOWN thus
 * no messages will appear on this stream.
 *
 * \param[inout]  outputs List of logger outputs.
 * \param[in]     size    Number of elements in list.
 * \param[in]     stream  Opened file stream.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
static inline logger_return_t __logger_output_stream_common_register(logger_output_t *outputs,
                                                                     const uint16_t  size,
                                                                     FILE            *stream)
{
  logger_return_t ret = LOGGER_OK;
  int16_t         index;
  logger_bool_t   found;

  /* check if stream valid */
  if (stream != NULL) {
    /* check if outputs valid */
    if (outputs != NULL) {
      /* check if this output stream is already registered */
      found = logger_false;
      for (index = 0 ; index < size ; index++) {
        if ((outputs[index].type == LOGGER_OUTPUT_TYPE_FILESTREAM) &&
            (outputs[index].stream == stream)) {
          found = logger_true;
          break;
        }
      }

      /* stream is already registered */
      if (found == logger_true) {
        outputs[index].count++;
        ret = LOGGER_ERR_OUTPUT_REGISTERED;
      }
      /* stream is not registered */
      else {
        /* search for an empty slot */
        found = logger_false;
        for (index = 0 ; index < size ; index++) {
          if (outputs[index].count == 0) {
            found = logger_true;
            break;
          }
        }

        /* found an empty slot */
        if (found == logger_true) {
          outputs[index].count++;
          outputs[index].level  = LOGGER_UNKNOWN;
          outputs[index].type   = LOGGER_OUTPUT_TYPE_FILESTREAM;
          outputs[index].stream = stream;

          /* only stdout and stderr use color by default */
          if ((stream == stdout) ||
              (stream == stderr)) {
            outputs[index].use_color = logger_true;
          }
          else {
            outputs[index].use_color = logger_false;
          }

          /* make a nonblocking stream */
          /*fcntl(fileno(stream), F_SETFL, fcntl(fileno(stream), F_GETFL) | O_NONBLOCK);*/
        }
        else {
          ret = LOGGER_ERR_OUTPUTS_FULL;
        }
      }
    }
    else {
      ret = LOGGER_ERR_OUTPUT_INVALID;
    }
  }
  else {
    ret = LOGGER_ERR_STREAM_INVALID;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Deregister an output stream from a list of outputs.
 *
 * Remove given file stream from list of outputs.
 *
 * \param[inout]  outputs List of logger outputs.
 * \param[in]     size    Number of elements in list.
 * \param[in]     stream  Opened file stream.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
static inline logger_return_t __logger_output_stream_common_deregister(logger_output_t *outputs,
                                                                       const uint16_t  size,
                                                                       FILE            *stream)
{
  logger_return_t ret = LOGGER_OK;
  int16_t         index;
  logger_bool_t   found;

  /* check if stream valid */
  if (stream != NULL) {
    /* check if outputs valid */
    if (outputs != NULL) {
      /* check if this output is already registered */
      found = logger_false;
      for (index = 0 ; index < size ; index++) {
        if ((outputs[index].type == LOGGER_OUTPUT_TYPE_FILESTREAM) &&
            (outputs[index].stream == stream)) {
          found = logger_true;
          break;
        }
      }

      /* found given stream in a slot */
      if (found == logger_true) {
        outputs[index].count--;

        /* remove this stream if this was the last reference */
        if (outputs[index].count <= 0) {
          /* flush everything in this stream */
          fflush(outputs[index].stream);

          /* reset output to default values */
          outputs[index].count     = 0;
          outputs[index].level     = LOGGER_UNKNOWN;
          outputs[index].use_color = logger_false;
          outputs[index].stream    = (FILE *)NULL;
          outputs[index].function  = (logger_output_function_t)NULL;
        }
      }
      else {
        ret = LOGGER_ERR_OUTPUT_NOT_FOUND;
      }
    }
    else {
      ret = LOGGER_ERR_OUTPUT_INVALID;
    }
  }
  else {
    ret = LOGGER_ERR_STREAM_INVALID;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Search an output stream in a list of outputs.
 *
 * The given file stream is searched in the given list of outputs.
 *
 * \param[inout]  outputs List of logger outputs.
 * \param[in]     size    Number of elements in list.
 * \param[in]     stream  Opened file stream.
 *
 * \return        \c logger_true if logger is found, logger_false otherwise
 ******************************************************************************/
static inline logger_bool_t __logger_output_stream_common_is_registered(logger_output_t *outputs,
                                                                        const uint16_t  size,
                                                                        FILE            *stream)
{
  logger_bool_t ret = logger_false;
  int16_t       index;

  /* check if stream valid */
  if (stream != NULL) {
    /* check if outputs valid */
    if (outputs != NULL) {
      /* check if this output stream is already registered */
      for (index = 0 ; index < size ; index++) {
        if ((outputs[index].type == LOGGER_OUTPUT_TYPE_FILESTREAM) &&
            (outputs[index].stream == stream)) {
          ret = logger_true;
          break;
        }
      }
    }
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Set logging level for output stream in a list of outputs.
 *
 * Set the minimum logging level for given output stream. Only log messages
 * equal or above the given level will be printed to the given stream.
 *
 * \param[inout]  outputs List of logger outputs.
 * \param[in]     size    Number of elements in list.
 * \param[in]     stream  Opened file stream.
 * \param[in]     level   Level to set.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
static inline logger_return_t __logger_output_stream_common_level_set(logger_output_t      *outputs,
                                                                      const uint16_t       size,
                                                                      FILE                 *stream,
                                                                      const logger_level_t level)
{
  logger_return_t ret = LOGGER_OK;
  int16_t         index;
  logger_bool_t   found;

  /* check if stream valid */
  if (stream != NULL) {
    /* check if outputs valid */
    if (outputs != NULL) {
      /* check for valid level */
      if ((level > LOGGER_UNKNOWN) &&
          (level < LOGGER_MAX)) {
        /* check if this output is already registered */
        found = logger_false;
        for (index = 0 ; index < size ; index++) {
          if ((outputs[index].type == LOGGER_OUTPUT_TYPE_FILESTREAM) &&
              (outputs[index].stream == stream)) {
            found = logger_true;
            break;
          }
        }

        /* found given stream in a slot */
        if (found == logger_true) {
          /* set log level */
          outputs[index].level = level;
        }
        else {
          ret = LOGGER_ERR_OUTPUT_NOT_FOUND;
        }
      }
      else {
        ret = LOGGER_ERR_LEVEL_UNKNOWN;
      }
    }
    else {
      ret = LOGGER_ERR_OUTPUT_INVALID;
    }
  }
  else {
    ret = LOGGER_ERR_STREAM_INVALID;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Query logging level for output stream in a list of outputs.
 *
 * Query the currently set minimum level for the given logging output stream.
 *
 * \param[inout]  outputs List of logger outputs.
 * \param[in]     size    Number of elements in list.
 * \param[in]     stream  Opened file stream.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
static inline logger_level_t __logger_output_stream_common_level_get(logger_output_t *outputs,
                                                                     const uint16_t  size,
                                                                     FILE            *stream)
{
  logger_level_t ret = LOGGER_UNKNOWN;
  int16_t        index;
  logger_bool_t  found;

  /* check if stream valid */
  if (stream != NULL) {
    /* check if outputs valid */
    if (outputs != NULL) {
      /* check if this output is already registered */
      found = logger_false;
      for (index = 0 ; index < size ; index++) {
        if ((outputs[index].type == LOGGER_OUTPUT_TYPE_FILESTREAM) &&
            (outputs[index].stream == stream)) {
          found = logger_true;
          break;
        }
      }

      /* found given stream in a slot */
      if (found == logger_true) {
        /* set log level */
        ret = outputs[index].level;
      }
    }
    else {
      ret = LOGGER_UNKNOWN;
    }
  }
  else {
    ret = LOGGER_UNKNOWN;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Modify the output stream color setting.
 *
 * Set the color setting for a given output stream to the given state.
 *
 * \param[inout]  outputs List of logger outputs.
 * \param[in]     size    Number of elements in list.
 * \param[in]     stream  Opened file stream.
 * \param[in]     flag    State of color setting.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
static inline logger_return_t __logger_output_stream_common_color(logger_output_t *outputs,
                                                                  const uint16_t  size,
                                                                  FILE            *stream,
                                                                  logger_bool_t   flag)
{
  logger_return_t ret = LOGGER_OK;
  int16_t         index;
  logger_bool_t   found;

  /* check if stream valid */
  if (stream != NULL) {
    /* check if outputs valid */
    if (outputs != NULL) {
      /* check if this output is already registered */
      found = logger_false;
      for (index = 0 ; index < size ; index++) {
        if ((outputs[index].type == LOGGER_OUTPUT_TYPE_FILESTREAM) &&
            (outputs[index].stream == stream)) {
          found = logger_true;
          break;
        }
      }

      /* found given stream in a slot */
      if (found == logger_true) {
        /* set color flag */
        outputs[index].use_color = flag;
      }
    }
    else {
      ret = LOGGER_ERR_OUTPUT_INVALID;
    }
  }
  else {
    ret = LOGGER_ERR_STREAM_INVALID;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Register an output function to a list of outputs.
 *
 * The user provided function must take a single string argument. The default
 * logging level is set to \c LOGGER_UNKNOWN thus this function will not be
 * called for any message.
 *
 * \param[inout]  outputs   List of logger outputs.
 * \param[in]     size      Number of elements in list.
 * \param[in]     function  User provided output function.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
static inline logger_return_t __logger_output_function_common_register(logger_output_t          *outputs,
                                                                       const uint16_t           size,
                                                                       logger_output_function_t function)
{
  logger_return_t ret = LOGGER_OK;
  int16_t         index;
  logger_bool_t   found;

  /* check if function pointer valid */
  if (function != NULL) {
    /* check if outputs valid */
    if (outputs != NULL) {
      /* check if this output function is already registered */
      found = logger_false;
      for (index = 0 ; index < size ; index++) {
        if ((outputs[index].type == LOGGER_OUTPUT_TYPE_FUNCTION) &&
            (outputs[index].function == function)) {
          found = logger_true;
          break;
        }
      }

      /* function is already registered */
      if (found == logger_true) {
        outputs[index].count++;
        ret = LOGGER_ERR_OUTPUT_REGISTERED;
      }
      /* function is not registered */
      else {
        /* search for an empty slot */
        found = logger_false;
        for (index = 0 ; index < size ; index++) {
          if (outputs[index].count == 0) {
            found = logger_true;
            break;
          }
        }

        /* found an empty slot */
        if (found == logger_true) {
          outputs[index].count++;
          outputs[index].level     = LOGGER_UNKNOWN;
          outputs[index].use_color = logger_false;
          outputs[index].type      = LOGGER_OUTPUT_TYPE_FUNCTION;
          outputs[index].function  = function;
        }
        else {
          ret = LOGGER_ERR_OUTPUTS_FULL;
        }
      }
    }
    else {
      ret = LOGGER_ERR_OUTPUT_INVALID;
    }
  }
  else {
    ret = LOGGER_ERR_FUNCTION_INVALID;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Deregister an output function from a list of outputs.
 *
 * Remove given output function from list of outputs.
 *
 * \param[inout]  outputs   List of logger outputs.
 * \param[in]     size      Number of elements in list.
 * \param[in]     function  User provided output function.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
static inline logger_return_t __logger_output_function_common_deregister(logger_output_t          *outputs,
                                                                         const uint16_t           size,
                                                                         logger_output_function_t function)
{
  logger_return_t ret = LOGGER_OK;
  int16_t         index;
  logger_bool_t   found;

  /* check if function valid */
  if (function != NULL) {
    /* check if outputs valid */
    if (outputs != NULL) {
      /* check if this output is already registered */
      found = logger_false;
      for (index = 0 ; index < size ; index++) {
        if ((outputs[index].type == LOGGER_OUTPUT_TYPE_FUNCTION) &&
            (outputs[index].function == function)) {
          found = logger_true;
          break;
        }
      }

      /* found given function in a slot */
      if (found == logger_true) {
        outputs[index].count--;

        /* remove this function if this was the last reference */
        if (outputs[index].count <= 0) {
          /* reset output to default values */
          outputs[index].count     = 0;
          outputs[index].level     = LOGGER_UNKNOWN;
          outputs[index].use_color = logger_false;
          outputs[index].stream    = (FILE *)NULL;
          outputs[index].function  = (logger_output_function_t)NULL;
        }
      }
      else {
        ret = LOGGER_ERR_OUTPUT_NOT_FOUND;
      }
    }
    else {
      ret = LOGGER_ERR_OUTPUT_INVALID;
    }
  }
  else {
    ret = LOGGER_ERR_FUNCTION_INVALID;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Search an output function in a list of outputs.
 *
 * The given file function is searched in the given list of outputs.
 *
 * \param[inout]  outputs   List of logger outputs.
 * \param[in]     size      Number of elements in list.
 * \param[in]     function  User provided output function.
 *
 * \return        \c logger_true if logger is found, logger_false otherwise
 ******************************************************************************/
static inline logger_return_t __logger_output_function_common_is_registered(logger_output_t          *outputs,
                                                                            const uint16_t           size,
                                                                            logger_output_function_t function)
{
  logger_bool_t ret = logger_false;
  int16_t       index;

  /* check if function valid */
  if (function != NULL) {
    /* check if outputs valid */
    if (outputs != NULL) {
      /* check if this output function is already registered */
      for (index = 0 ; index < size ; index++) {
        if ((outputs[index].type == LOGGER_OUTPUT_TYPE_FUNCTION) &&
            (outputs[index].function == function)) {
              ret = logger_true;
          break;
        }
      }
    }
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Set logging level for output function in a list of outputs.
 *
 * Set the minimum logging level for given output function. Only log messages
 * equal or above the given level the function will be called.
 *
 * \param[inout]  outputs   List of logger outputs.
 * \param[in]     size      Number of elements in list.
 * \param[in]     function  User provided output function.
 * \param[in]     level     Level to set.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
static inline logger_return_t __logger_output_function_common_level_set(logger_output_t           *outputs,
                                                                         const uint16_t           size,
                                                                         logger_output_function_t function,
                                                                         const logger_level_t     level)
{
  logger_return_t ret = LOGGER_OK;
  int16_t         index;
  logger_bool_t   found;

  /* check if function valid */
  if (function != NULL) {
    /* check if outputs valid */
    if (outputs != NULL) {
      /* check for valid level */
      if ((level > LOGGER_UNKNOWN) &&
          (level < LOGGER_MAX)) {
        /* check if this output is already registered */
        found = logger_false;
        for (index = 0 ; index < size ; index++) {
          if ((outputs[index].type == LOGGER_OUTPUT_TYPE_FUNCTION) &&
              (outputs[index].function == function)) {
            found = logger_true;
            break;
          }
        }

        /* found given function in a slot */
        if (found == logger_true) {
          /* set log level */
          outputs[index].level = level;
        }
        else {
          ret = LOGGER_ERR_OUTPUT_NOT_FOUND;
        }
      }
      else {
        ret = LOGGER_ERR_LEVEL_UNKNOWN;
      }
    }
    else {
      ret = LOGGER_ERR_OUTPUT_INVALID;
    }
  }
  else {
    ret = LOGGER_ERR_FUNCTION_INVALID;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Query logging level for output function in a list of outputs.
 *
 * Query the currently set minimum level for the given logging output function.
 *
 * \param[inout]  outputs   List of logger outputs.
 * \param[in]     size      Number of elements in list.
 * \param[in]     function  User provided output function.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
static inline logger_level_t __logger_output_function_common_level_get(logger_output_t          *outputs,
                                                                       const uint16_t           size,
                                                                       logger_output_function_t function)
{
  logger_level_t ret = LOGGER_UNKNOWN;
  int16_t        index;
  logger_bool_t  found;

  /* check if function valid */
  if (function != NULL) {
    /* check if outputs valid */
    if (outputs != NULL) {
      /* check if this output is already registered */
      found = logger_false;
      for (index = 0 ; index < size ; index++) {
        if ((outputs[index].type == LOGGER_OUTPUT_TYPE_FUNCTION) &&
            (outputs[index].function == function)) {
          found = logger_true;
          break;
        }
      }

      /* found given function in a slot */
      if (found == logger_true) {
        /* set log level */
        ret = outputs[index].level;
      }
    }
    else {
      ret = LOGGER_UNKNOWN;
    }
  }
  else {
    ret = LOGGER_UNKNOWN;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Modify the output function color setting.
 *
 * Set the color setting for a given output function to the given state.
 *
 * \param[inout]  outputs   List of logger outputs.
 * \param[in]     size      Number of elements in list.
 * \param[in]     function  User provided output function.
 * \param[in]     flag      State of color setting.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
static inline logger_return_t __logger_output_function_common_color(logger_output_t          *outputs,
                                                                    const uint16_t           size,
                                                                    logger_output_function_t function,
                                                                    logger_bool_t            flag)
{
  logger_return_t ret = LOGGER_OK;
  int16_t         index;
  logger_bool_t   found;

  /* check if function valid */
  if (function != NULL) {
    /* check if outputs valid */
    if (outputs != NULL) {
      /* check if this output is already registered */
      found = logger_false;
      for (index = 0 ; index < size ; index++) {
        if ((outputs[index].type == LOGGER_OUTPUT_TYPE_FUNCTION) &&
            (outputs[index].function == function)) {
          found = logger_true;
          break;
        }
      }

      /* found given function in a slot */
      if (found == logger_true) {
        /* set color flag */
        outputs[index].use_color = flag;
      }
    }
    else {
      ret = LOGGER_ERR_OUTPUT_INVALID;
    }
  }
  else {
    ret = LOGGER_ERR_FUNCTION_INVALID;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Register a global output stream.
 *
 * The given file stream may be on of \c stdout, \c stderr or a file stream
 * opened by the user. The default logging level is set to \c LOGGER_UNKNOWN thus
 * no messages will appear on this stream.
 *
 * \param[in]     stream  Opened file stream.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_output_register(FILE *stream)
{
  logger_return_t ret = LOGGER_OK;

  /* add stream to global outputs */
  ret = __logger_output_stream_common_register(logger_outputs, LOGGER_OUTPUTS_MAX, stream);

  return(ret);
}


/** ************************************************************************//**
 * \brief  Unregister a global output stream.
 *
 * Remove given file stream from list of outputs.
 *
 * \param[in]     stream  Previous registered file stream.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_output_deregister(FILE *stream)
{
  logger_return_t ret = LOGGER_OK;

  /* delete stream to global outputs */
  ret = __logger_output_stream_common_deregister(logger_outputs, LOGGER_OUTPUTS_MAX, stream);

  return(ret);
}


/** ************************************************************************//**
 * \brief  Search an output stream in list of global outputs.
 *
 * The given file stream is searched in the list of global outputs.
 *
 * \param[in]     stream  File stream.
 *
 * \return        \c logger_true if logger is found, logger_false otherwise
 ******************************************************************************/
logger_bool_t __logger_output_is_registered(FILE *stream)
{
  logger_bool_t ret = logger_false;

  /* search stream in global outputs */
  ret = __logger_output_stream_common_is_registered(logger_outputs, LOGGER_OUTPUTS_MAX, stream);

  return(ret);
}


/** ************************************************************************//**
 * \brief  Set logging level for global output stream.
 *
 * Set the minimum logging level for given output stream. Only log messages
 * equal or above the given level will be printed to the given stream.
 *
 * \param[in]     stream  Previous registered file stream.
 * \param[in]     level   Level to set.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_output_level_set(FILE                 *stream,
                                          const logger_level_t level)
{
  logger_return_t ret = LOGGER_OK;

  /* set stream output level to global outputs */
  ret = __logger_output_stream_common_level_set(logger_outputs, LOGGER_OUTPUTS_MAX, stream, level);

  return(ret);
}


/** ************************************************************************//**
 * \brief  Query logging level for global output stream.
 *
 * Query the currently set minimum level for the given logging output stream.
 *
 * \param[in]     stream  Previous registered file stream.
 *
 * \return        Currently set logging level.
 ******************************************************************************/
logger_level_t __logger_output_level_get(FILE *stream)
{
  logger_level_t ret = LOGGER_UNKNOWN;

  /* get stream output level to global outputs */
  ret = __logger_output_stream_common_level_get(logger_outputs, LOGGER_OUTPUTS_MAX, stream);

  return(ret);
}


/** ************************************************************************//**
 * \brief  Enable the global output stream color setting.
 *
 * Enable the color setting for a given output stream.
 *
 * \param[in]     stream  Opened file stream.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_output_color_enable(FILE *stream)
{
  logger_return_t ret = LOGGER_OK;

  /* set stream output level to global outputs */
  ret = __logger_output_stream_common_color(logger_outputs, LOGGER_OUTPUTS_MAX, stream, logger_true);

  return(ret);
}


/** ************************************************************************//**
 * \brief  Disable the global output stream color setting.
 *
 * Disable the color setting for a given output stream.
 *
 * \param[in]     stream  Opened file stream.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_output_color_disable(FILE *stream)
{
  logger_return_t ret = LOGGER_OK;

  /* set stream output level to global outputs */
  ret = __logger_output_stream_common_color(logger_outputs, LOGGER_OUTPUTS_MAX, stream, logger_false);

  return(ret);
}


/** ************************************************************************//**
 * \brief  Flush all output streams.
 *
 * Force a flush of all output streams.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_output_flush(void)
{
  logger_return_t ret = LOGGER_OK;
  int16_t         index;
  int16_t         id;

  /* search for used global outputs */
  for (index = 0 ; index < LOGGER_OUTPUTS_MAX ; index++) {
    if (logger_outputs[index].count > 0) {
      fflush(logger_outputs[index].stream);
    }
  }

  /* search for used id specific outputs */
  for (id = 0 ; id < LOGGER_IDS_MAX ; id++) {
    for (index = 0 ; index < LOGGER_ID_OUTPUTS_MAX ; index++) {
      if (logger_control[id].outputs[index].count > 0) {
        fflush(logger_control[id].outputs[index].stream);
      }
    }
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Register a global output function.
 *
 * The user provided function must take a single string argument. The default
 * logging level is set to \c LOGGER_UNKNOWN thus this function will not be
 * called for any message.
 *
 * \param[in]     function  User provided output function.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_output_function_register(logger_output_function_t function)
{
  logger_return_t ret = LOGGER_OK;

  /* add function to global outputs */
  ret = __logger_output_function_common_register(logger_outputs, LOGGER_OUTPUTS_MAX, function);

  return(ret);
}


/** ************************************************************************//**
 * \brief  Deregister a global output function:
 *
 * Remove given output function from list of outputs.
 *
 * \param[in]     function  User provided output function.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_output_function_deregister(logger_output_function_t function)
{
  logger_return_t ret = LOGGER_OK;

  /* delete function to global outputs */
  ret = __logger_output_function_common_deregister(logger_outputs, LOGGER_OUTPUTS_MAX, function);

  return(ret);
}


/** ************************************************************************//**
 * \brief  Search a global output function in a list of global outputs.
 *
 * The given file function is searched in the list of global outputs.
 *
 * \param[in]     function  User provided output function.
 *
 * \return        \c logger_true if logger is found, logger_false otherwise
 ******************************************************************************/
logger_bool_t __logger_output_function_is_registered(logger_output_function_t function)
{
  logger_bool_t ret = logger_false;

  /* search function in global outputs */
  ret = __logger_output_function_common_is_registered(logger_outputs, LOGGER_OUTPUTS_MAX, function);

  return(ret);
}


/** ************************************************************************//**
 * \brief  Set logging level for global output function.
 *
 * Set the minimum logging level for given output function. Only log messages
 * equal or above the given level the function will be called.
 *
 * \param[in]     function  User provided output function.
 * \param[in]     level     Level to set.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_output_function_level_set(logger_output_function_t function,
                                                   const logger_level_t     level)
{
  logger_return_t ret = LOGGER_OK;

  /* set function output level to global outputs */
  ret = __logger_output_function_common_level_set(logger_outputs, LOGGER_OUTPUTS_MAX, function, level);

  return(ret);
}


/** ************************************************************************//**
 * \brief  Query logging level for global output function.
 *
 * Query the currently set minimum level for the given logging output function.
 *
 * \param[in]     function  User provided output function.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_level_t __logger_output_function_level_get(logger_output_function_t function)
{
  logger_level_t ret = LOGGER_UNKNOWN;

  /* get function output level to global outputs */
  ret = __logger_output_function_common_level_get(logger_outputs, LOGGER_OUTPUTS_MAX, function);

  return(ret);
}


/** ************************************************************************//**
 * \brief  Enable the global output function color setting.
 *
 * Enable the color setting for a given output function.
 *
 * \param[in]     function  User provided output function.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_output_function_color_enable(logger_output_function_t function)
{
  logger_return_t ret = LOGGER_OK;

  /* set function output level to global outputs */
  ret = __logger_output_function_common_color(logger_outputs, LOGGER_OUTPUTS_MAX, function, logger_true);

  return(ret);
}


/** ************************************************************************//**
 * \brief  Enable the global output function color setting.
 *
 * Enable the color setting for a given output function.
 *
 * \param[in]     function  User provided output function.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_output_function_color_disable(logger_output_function_t function)
{
  logger_return_t ret = LOGGER_OK;

  /* set function output level to global outputs */
  ret = __logger_output_function_common_color(logger_outputs, LOGGER_OUTPUTS_MAX, function, logger_false);

  return(ret);
}


/** ************************************************************************//**
 * \brief  Request a logging ID.
 *
 * Request a ID for later use with calls to \c logger(). If an ID with the same
 * symbolic name has already been requested that ID is returned otherwise a new
 * ID is allocated. Each logging ID is disabled by default.
 *
 * \param[in]     name    Symbolic name for the ID.
 *
 * \return        ID number if ID is available, error code otherwise.
 ******************************************************************************/
logger_id_t __logger_id_request(const char *name)
{
  logger_id_t   id = logger_id_unknown;
  int16_t       index;
  logger_bool_t found;

  /* check validity of name */
  if (name != NULL) {
    /* start search */
    found = logger_false;

    /* search for an already existing ID with the same name */
    for (index = 0 ; index < LOGGER_IDS_MAX ; index++) {
      if (logger_control[index].used == logger_true) {
        if (strncmp(logger_control[index].name, name, LOGGER_NAME_MAX) == 0) {
          logger_control[index].count++;
          found = logger_true;
          break;
        }
      }
    }

    /* search for an available ID */
    if (found == logger_false) {
      for (index = 0 ; index < LOGGER_IDS_MAX ; index++) {
        if (logger_control[index].used == logger_false) {
          found = logger_true;
          /* reset the ID */
          memset(&logger_control[index], 0, sizeof(logger_control[index]));

          /* initialize the ID */
          logger_control[index].used                  = logger_true;
          logger_control[index].count                 = 1;
          logger_control[index].enabled               = logger_false;
          logger_control[index].level                 = LOGGER_UNKNOWN;
          logger_control[index].prefix                = LOGGER_PREFIX_STANDARD;
          logger_control[index].color                 = logger_false;
          logger_control[index].color_string.begin[0] = '\0';
          logger_control[index].color_string.end[0]   = '\0';
          logger_control[index].color_string_changed  = logger_false;

          /* copy the name */
          strncpy(logger_control[index].name, name, LOGGER_NAME_MAX);
          logger_control[index].name[LOGGER_NAME_MAX - 1] = '\0';

          break;
        }
      }
    }

    /* found an empty slot */
    if (found == logger_true) {
      id = (logger_id_t)index;
    }
    else {
      id = logger_id_unknown;
    }
  }
  else {
    id = logger_id_unknown;
  }

  return(id);
}


/** ************************************************************************//**
 * \brief  Release a logging ID.
 *
 * Release a ID. After this call logging to this ID is not possible anymore.
 *
 * \param[in]     id      ID to enable.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_id_release(const logger_id_t id)
{
  logger_return_t ret = LOGGER_OK;

  /* check for valid ID */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX)) {
    if (logger_control[id].used == logger_true) {
      logger_control[id].count--;

      /* if this was the last ID */
      if (logger_control[id].count <= 0) {
        /* flush all streams */
        __logger_output_flush();

        /* reset the ID */
        memset(&logger_control[id], 0, sizeof(logger_control[id]));

        /* reset all ID dependent values to defaults */
        logger_control[id].used                  = logger_false;
        logger_control[id].count                 = 0;
        logger_control[id].enabled               = logger_false;
        logger_control[id].level                 = LOGGER_UNKNOWN;
        logger_control[id].prefix                = LOGGER_PREFIX_STANDARD;
        logger_control[id].color                 = logger_false;
        logger_control[id].color_string.begin[0] = '\0';
        logger_control[id].color_string.end[0]   = '\0';
        logger_control[id].color_string_changed  = logger_false;
        logger_control[id].name[0]               = '\0';

        /* reset outputs */
        memset(logger_control[id].outputs, 0, sizeof(logger_control[id].outputs));
      }
    }
    else {
      ret = LOGGER_ERR_ID_UNKNOWN;
    }
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Enable a logging ID for output.
 *
 * Enable given logging ID for all outputs.
 *
 * \param[in]     id      ID to enable.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_id_enable(const logger_id_t id)
{
  logger_return_t ret = LOGGER_OK;

  /* check for valid ID */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX) &&
      (logger_control[id].used == logger_true)) {
    /* enable given ID */
    logger_control[id].enabled = logger_true;
  }
  else {
    ret = LOGGER_ERR_ID_UNKNOWN;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Disable a logger ID for output.
 *
 * Disable given logging ID for all outputs.
 *
 * \param[in]     id      ID to disable.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise
 ******************************************************************************/
logger_return_t __logger_id_disable(const logger_id_t id)
{
  logger_return_t ret = LOGGER_OK;

  /* check for valid ID */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX) &&
      (logger_control[id].used == logger_true)) {
    /* disable given ID */
    logger_control[id].enabled = logger_false;
  }
  else {
    ret = LOGGER_ERR_ID_UNKNOWN;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Query the current enable state of ID.
 *
 * Query the current enable state of given logging ID.
 *
 * \param[in]     id      ID to check enable state.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_bool_t __logger_id_is_enabled(const logger_id_t id)
{
  logger_bool_t ret = logger_false;

  /* check for valid ID */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX)) {
    /* ID enable state */
    ret = logger_control[id].enabled;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Set logging level for ID.
 *
 * Set the minimum logging level for given ID. Only log messages equal or above
 * the given level will be printed to outputs.
 *
 * \param[in]     id      ID for setting level.
 * \param[in]     level   Level to set.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_id_level_set(const logger_id_t    id,
                                      const logger_level_t level)
{
  logger_return_t ret = LOGGER_OK;

  /* check for valid ID */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX) &&
      (logger_control[id].used == logger_true)) {
    /* check for valid level */
    if ((level > LOGGER_UNKNOWN) &&
        (level < LOGGER_MAX)) {
      /* set ID level */
      logger_control[id].level = level;
    }
    else {
      ret = LOGGER_ERR_LEVEL_UNKNOWN;
    }
  }
  else {
    ret = LOGGER_ERR_ID_UNKNOWN;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Query logging level for ID.
 *
 * Query the currently set minimum level for the given logging ID.
 *
 * \param[in]     id      ID for querying level
 *
 * \return        Currently set level
 ******************************************************************************/
logger_level_t __logger_id_level_get(const logger_id_t id)
{
  logger_level_t ret = LOGGER_UNKNOWN;

  /* check for valid ID */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX) &&
      (logger_control[id].used == logger_true)) {
    /* get ID level */
    ret = logger_control[id].level;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Set logging prefix for ID.
 *
 * Set the prefix for given ID. All messages for this ID will get this prefix
 * from now on.
 *
 * \param[in]     id      ID for setting prefix.
 * \param[in]     prefix  Prefix to set.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_id_prefix_set(const logger_id_t     id,
                                       const logger_prefix_t prefix)
{
  logger_return_t ret = LOGGER_OK;

  /* check for valid ID */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX) &&
      (logger_control[id].used == logger_true)) {
    /* check for valid level */
    if ((prefix > LOGGER_PREFIX_UNKNOWN) &&
        (prefix < LOGGER_PREFIX_MAX)) {
      /* set ID prefix */
      logger_control[id].prefix = prefix;
    }
    else {
      ret = LOGGER_ERR_PREFIX_UNKNOWN;
    }
  }
  else {
    ret = LOGGER_ERR_ID_UNKNOWN;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Query logging prefix for ID.
 *
 * Query the currently set prefix for the given logging ID.
 *
 * \param[in]     id      ID for querying prefix
 *
 * \return        Currently set level
 ******************************************************************************/
logger_prefix_t __logger_id_prefix_get(const logger_id_t id)
{
  logger_prefix_t ret = LOGGER_PREFIX_UNKNOWN;

  /* check for valid ID */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX) &&
      (logger_control[id].used == logger_true)) {
    /* get ID level */
    ret = logger_control[id].prefix;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Query name for id.
 *
 * Query the name for the given logging ID.
 *
 * \param[in]     id      ID for querying name
 *
 * \return        Symbolic name of the ID
 ******************************************************************************/
const char *__logger_id_name_get(const logger_id_t id)
{
  char *name = NULL;

  /* check for valid ID */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX) &&
      (logger_control[id].used == logger_true)) {
    /* get ID level */
    name = logger_control[id].name;
  }

  return(name);
}


/** ************************************************************************//**
 * \brief  Register an id specific output stream.
 *
 * The given file stream may be on of \c stdout, \c stderr or a file stream
 * opened by the user. The default logging level is set to \c LOGGER_UNKNOWN thus
 * no messages will appear on this stream.
 *
 * \param[in]     id      ID for querying name
 * \param[in]     stream  Opened file stream.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_id_output_register(const logger_id_t id,
                                            FILE              *stream)
{
  logger_return_t ret = LOGGER_OK;

  /* check if ID is valid */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX) &&
      (logger_control[id].used == logger_true)) {
    /* add stream to id specific outputs */
    ret = __logger_output_stream_common_register(logger_control[id].outputs, LOGGER_ID_OUTPUTS_MAX, stream);
  }
  else {
    ret = LOGGER_ERR_ID_UNKNOWN;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Unregister an id specific output stream.
 *
 * Remove given file stream from list of outputs.
 *
 * \param[in]     id      ID for querying name
 * \param[in]     stream  Previous registered file stream.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_id_output_deregister(const logger_id_t id,
                                              FILE              *stream)
{
  logger_return_t ret = LOGGER_OK;

  /* check if ID is valid */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX) &&
      (logger_control[id].used == logger_true)) {
    /* delete stream to id specific outputs */
    ret = __logger_output_stream_common_deregister(logger_control[id].outputs, LOGGER_ID_OUTPUTS_MAX, stream);
  }
  else {
    ret = LOGGER_ERR_ID_UNKNOWN;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Search an output stream in id specific outputs.
 *
 * The given file stream is searched in of id specific outputs.
 *
 * \param[in]     stream  File stream.
 *
 * \return        \c logger_true if logger is found, logger_false otherwise
 ******************************************************************************/
logger_bool_t __logger_id_output_is_registered(const logger_id_t id,
                                                 FILE              *stream)
{
  logger_bool_t ret = logger_false;

  /* check if ID is valid */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX) &&
      (logger_control[id].used == logger_true)) {
    /* search stream in id specific outputs */
    ret = __logger_output_stream_common_is_registered(logger_control[id].outputs, LOGGER_ID_OUTPUTS_MAX, stream);
  }
  else {
    ret = logger_false;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Set logging level for id specific output stream.
 *
 * Set the minimum logging level for given output stream. Only log messages
 * equal or above the given level will be printed to the given stream.
 *
 * \param[in]     id      ID for setting the logging level.
 * \param[in]     stream  Previous registered file stream.
 * \param[in]     level   Level to set.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_id_output_level_set(const logger_id_t    id,
                                             FILE                 *stream,
                                             const logger_level_t level)
{
  logger_return_t ret = LOGGER_OK;

  /* check if ID is valid */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX) &&
      (logger_control[id].used == logger_true)) {
    /* set stream output level to id specific outputs */
    ret = __logger_output_stream_common_level_set(logger_control[id].outputs, LOGGER_ID_OUTPUTS_MAX, stream, level);
  }
  else {
    ret = LOGGER_ERR_ID_UNKNOWN;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Query logging level for id specific output stream.
 *
 * Query the currently set minimum level for the given logging output stream.
 *
 * \param[in]     id      ID for getting the logging level.
 * \param[in]     stream  Previous registered file stream.
 *
 * \return        Currently set logging level.
 ******************************************************************************/
logger_level_t __logger_id_output_level_get(const logger_id_t id,
                                            FILE              *stream)
{
  logger_level_t ret = LOGGER_UNKNOWN;

  /* check if ID is valid */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX) &&
      (logger_control[id].used == logger_true)) {
    /* get stream output level from id specific outputs */
    ret = __logger_output_stream_common_level_get(logger_control[id].outputs, LOGGER_ID_OUTPUTS_MAX, stream);
  }
  else {
    ret = LOGGER_UNKNOWN;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Enable the id specific output stream color setting.
 *
 * Enable the color setting for a given output stream.
 *
 * \param[in]     stream  Opened file stream.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_id_output_color_enable(const logger_id_t id,
                                                FILE              *stream)
{
  logger_return_t ret = LOGGER_OK;

  /* check if ID is valid */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX) &&
      (logger_control[id].used == logger_true)) {
    /* enable stream output color in id specific outputs */
    ret = __logger_output_stream_common_color(logger_control[id].outputs, LOGGER_ID_OUTPUTS_MAX, stream, logger_true);
  }
  else {
    ret = LOGGER_ERR_ID_UNKNOWN;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Disable the id specific output stream color setting.
 *
 * Disable the color setting for a given output stream.
 *
 * \param[in]     stream  Opened file stream.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_id_output_color_disable(const logger_id_t id,
                                                 FILE              *stream)
{
  logger_return_t ret = LOGGER_OK;

  /* check if ID is valid */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX) &&
      (logger_control[id].used == logger_true)) {
    /* disable stream output color in id specific outputs */
    ret = __logger_output_stream_common_color(logger_control[id].outputs, LOGGER_ID_OUTPUTS_MAX, stream, logger_false);
  }
  else {
    ret = LOGGER_ERR_ID_UNKNOWN;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Register an id specific output stream.
 *
 * The user provided function must take a single string argument. The default
 * logging level is set to \c LOGGER_UNKNOWN thus this function will not be
 * called for any message.
 *
 * \param[in]     id        ID for querying name
 * \param[in]     function  User provided output function.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_id_output_function_register(const logger_id_t        id,
                                                     logger_output_function_t function)
{
  logger_return_t ret = LOGGER_OK;

  /* check if ID is valid */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX) &&
      (logger_control[id].used == logger_true)) {
    /* add function to global outputs */
    ret = __logger_output_function_common_register(logger_control[id].outputs, LOGGER_ID_OUTPUTS_MAX, function);
  }
  else {
    ret = LOGGER_ERR_ID_UNKNOWN;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Unregister an id specific output function.
 *
 * Remove given file function from list of outputs.
 *
 * \param[in]     id        ID for querying name
 * \param[in]     function  User provided output function.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_id_output_function_deregister(const logger_id_t        id,
                                                       logger_output_function_t function)
{
  logger_return_t ret = LOGGER_OK;

  /* check if ID is valid */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX) &&
      (logger_control[id].used == logger_true)) {
    /* delete function to global outputs */
    ret = __logger_output_function_common_deregister(logger_control[id].outputs, LOGGER_ID_OUTPUTS_MAX, function);
  }
  else {
    ret = LOGGER_ERR_ID_UNKNOWN;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Search an output function in id specific outputs.
 *
 * The given output function is searched in of id specific outputs.
 *
 * \param[in]     function  User provided output function.
 *
 * \return        \c logger_true if logger is found, logger_false otherwise
 ******************************************************************************/
logger_bool_t __logger_id_output_function_is_registered(const logger_id_t        id,
                                                        logger_output_function_t function)
{
  logger_bool_t ret = logger_false;

  /* check if ID is valid */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX) &&
      (logger_control[id].used == logger_true)) {
    /* search function in global outputs */
    ret = __logger_output_function_common_is_registered(logger_control[id].outputs, LOGGER_ID_OUTPUTS_MAX, function);
  }
  else {
    ret = logger_false;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Set logging level for id specific output function.
 *
 * Set the minimum logging level for given output function. Only log messages
 * equal or above the given level will be printed to the given function.
 *
 * \param[in]     id        ID for setting the logging level.
 * \param[in]     function  User provided output function.
 * \param[in]     level     Level to set.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_id_output_function_level_set(const logger_id_t        id,
                                                      logger_output_function_t function,
                                                      const logger_level_t     level)
{
  logger_return_t ret = LOGGER_OK;

  /* check if ID is valid */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX) &&
      (logger_control[id].used == logger_true)) {
    /* set function output level to id specific outputs */
    ret = __logger_output_function_common_level_set(logger_control[id].outputs, LOGGER_ID_OUTPUTS_MAX, function, level);
  }
  else {
    ret = LOGGER_ERR_ID_UNKNOWN;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Query logging level for id specific output function.
 *
 * Query the currently set minimum level for the given logging output function.
 *
 * \param[in]     id        ID for getting the logging level.
 * \param[in]     function  User provided output function.
 *
 * \return        Currently set logging level.
 ******************************************************************************/
logger_level_t __logger_id_output_function_level_get(const logger_id_t        id,
                                                     logger_output_function_t function)
{
  logger_level_t ret = LOGGER_UNKNOWN;

  /* check if ID is valid */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX) &&
      (logger_control[id].used == logger_true)) {
    /* get function output level from id specific outputs */
    ret = __logger_output_function_common_level_get(logger_control[id].outputs, LOGGER_ID_OUTPUTS_MAX, function);
  }
  else {
    ret = LOGGER_UNKNOWN;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Enable the id specific output function color setting.
 *
 * Enable the color setting for a given output function.
 *
 * \param[in]     function  User provided output function.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_id_output_function_color_enable(const logger_id_t        id,
                                                         logger_output_function_t function)
{
  logger_return_t ret = LOGGER_OK;

  /* check if ID is valid */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX) &&
      (logger_control[id].used == logger_true)) {
    /* enable function output enable to id specific outputs */
    ret = __logger_output_function_common_color(logger_control[id].outputs, LOGGER_ID_OUTPUTS_MAX, function, logger_true);
  }
  else {
    ret = LOGGER_ERR_ID_UNKNOWN;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Disable the id specific output function color setting.
 *
 * Disable the color setting for a given output function.
 *
 * \param[in]     function  User provided output function.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_id_output_function_color_disable(const logger_id_t        id,
                                                          logger_output_function_t function)
{
  logger_return_t ret = LOGGER_OK;

  /* check if ID is valid */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX) &&
      (logger_control[id].used == logger_true)) {
    /* disable function output color in id specific outputs */
    ret = __logger_output_function_common_color(logger_control[id].outputs, LOGGER_ID_OUTPUTS_MAX, function, logger_false);
  }
  else {
    ret = LOGGER_ERR_ID_UNKNOWN;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Enable logger prefix colors.
 *
 * Enable logger prefix color output.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_color_prefix_enable(void)
{
  logger_return_t ret = LOGGER_OK;

  logger_color_prefix_enabled = logger_true;

  return(ret);
}


/** ************************************************************************//**
 * \brief  Disable logger prefix colors.
 *
 * Disable logger prefix color output.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_color_prefix_disable(void)
{
  logger_return_t ret = LOGGER_OK;

  logger_color_prefix_enabled = logger_false;

  return(ret);
}


/** ************************************************************************//**
 * \brief  Query the current enable state of logger prefix colors.
 *
 * Query the current prefix color enable state of logger.
 *
 * \return        \c logger_true if logger prefix color is enabled, logger_false otherwise
 ******************************************************************************/
logger_bool_t __logger_color_prefix_is_enabled(void)
{
  return(logger_color_prefix_enabled);
}


/** ************************************************************************//**
 * \brief  Enable logger message colors.
 *
 * Enable logger message color output.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_color_message_enable(void)
{
  logger_return_t ret = LOGGER_OK;

  logger_color_message_enabled = logger_true;

  return(ret);
}


/** ************************************************************************//**
 * \brief  Disable logger message colors.
 *
 * Disable logger message color output.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_color_message_disable(void)
{
  logger_return_t ret = LOGGER_OK;

  logger_color_message_enabled = logger_false;

  return(ret);
}


/** ************************************************************************//**
 * \brief  Query the current enable state of logger message colors.
 *
 * Query the current color message enable state of logger.
 *
 * \return        \c logger_true if logger color is enabled, logger_false otherwise
 ******************************************************************************/
logger_bool_t __logger_color_message_is_enabled(void)
{
  return(logger_color_message_enabled);
}


/** ************************************************************************//**
 * \brief  Change terminal text color and attributes.
 *
 * Change text color and attributes for all messages of given ID when they are
 * printed to \c stdout or \c stdin. Outputs to other streams including files
 * will have no
 * color.
 *
 * \param[in]     id      ID for setting level.
 * \param[in]     fg      Text foreground.
 * \param[in]     bg      Text background.
 * \param[in]     attr    Text attribute.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_color_set(const logger_id_t        id,
                                   const logger_text_fg_t   fg,
                                   const logger_text_bg_t   bg,
                                   const logger_text_attr_t attr)
{
  logger_return_t ret = LOGGER_OK;

  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX) &&
      (logger_control[id].used == logger_true)) {
    logger_control[id].color = logger_true;
    (void)snprintf(logger_control[id].color_string.begin, LOGGER_COLOR_STRING_MAX, "\x1B[%d;%d;%dm", attr, fg, bg);
    (void)snprintf(logger_control[id].color_string.end, LOGGER_COLOR_STRING_MAX, "\x1B[%dm", LOGGER_ATTR_RESET);
    logger_control[id].color_string.begin[LOGGER_COLOR_STRING_MAX - 1] = '\0';
    logger_control[id].color_string.end[LOGGER_COLOR_STRING_MAX - 1]   = '\0';
    logger_control[id].color_string_changed = logger_true;
  }
  else {
    ret = LOGGER_ERR_ID_UNKNOWN;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Reset terminal text color and attributes.
 *
 * Reset text color and attributes of given ID back to defaults.
 *
 * \param[in]     id      ID for setting level.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_color_reset(const logger_id_t id)
{
  logger_return_t ret = LOGGER_OK;

  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX) &&
      (logger_control[id].used == logger_true)) {
    logger_control[id].color                 = logger_false;
    logger_control[id].color_string.begin[0] = '\0';
    logger_control[id].color_string.end[0]   = '\0';
    logger_control[id].color_string_changed  = logger_true;
  }
  else {
    ret = LOGGER_ERR_ID_UNKNOWN;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Query name for level.
 *
 * Query the name for the given logging level.
 *
 * \param[in]     level   Level for querying name
 *
 * \return        Symbolic name of the level
 ******************************************************************************/
const char *__logger_level_name_get(const logger_level_t level)
{
  const char *name = logger_level_names[0];

  /* check for valid ID */
  if ((level > LOGGER_UNKNOWN) &&
      (level < LOGGER_MAX)) {
    /* get ID level */
    name = logger_level_names[level];
  }

  return(name);
}


/** ************************************************************************//**
 * \brief  strip path from file name.
 *
 * Strip path from a fully qualified file name. Returns pointer to string part
 * after last separator.
 *
 * \param[in]     file    File name containing path
 *
 * \return        Pointer to file name.
 ******************************************************************************/
static inline const char *__logger_strip_path(const char *file)
{
  const char *basename;

  /* find last separator */
  basename = strrchr(file, '/');
  if (basename != NULL) {
    /* eat last separator */
    basename++;
  }
  else {
    /* unable to find separator, use whole file name */
    basename = file;
  }

  return(basename);
}


/** ************************************************************************//**
 * \brief  Format message prefix
 *
 * Allocate memory for prefix and print prefix to allocated memory.
 *
 * Memory needs to be deallocated using free() when the sting is not needed anymore.
 *
 * \param[in]     id           ID outputting this message.
 * \param[out]    prefix       Formatted message prefix.
 * \param[in]     prefix_size  String length of formatted message prefix.
 * \param[in]     level        Level of this message.
 * \param[in]     file         Name of file where this call happened.
 * \param[in]     function     Name of function where this call happened.
 * \param[in]     line         Line where this call happened.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
static inline logger_return_t __logger_format_prefix(logger_id_t    id,
                                                     char           *prefix,
                                                     uint16_t       prefix_size,
                                                     logger_level_t level,
                                                     const char     *file,
                                                     const char     *function,
                                                     uint32_t       line)
{
  logger_return_t ret        = LOGGER_OK;
  int16_t         characters = 0;
  uint16_t        rev_idx;

  /* do prefix stuff only if needed */
  if (logger_control[id].append == logger_false) {
    switch (logger_control[id].prefix) {
      case LOGGER_PREFIX_UNKNOWN:
        /* empty */
        prefix[0] = '\0';
        characters = 0;
        break;

      case LOGGER_PREFIX_EMPTY:
        /* empty */
        prefix[0] = '\0';
        characters = 0;
        break;

      case LOGGER_PREFIX_FILE_FUNCTION_LINE:
        characters = snprintf(prefix, prefix_size,
                              LOGGER_FORMAT_STRING_FILE ":"
                              LOGGER_FORMAT_STRING_FUNCTION ":"
                              LOGGER_FORMAT_STRING_LINE ":"
                              " ",
                              __logger_strip_path(file),
                              function,
                              line);
        break;

      case LOGGER_PREFIX_FILE_LINE:
        characters = snprintf(prefix, prefix_size,
                              LOGGER_FORMAT_STRING_FILE ":"
                              LOGGER_FORMAT_STRING_LINE ":"
                              " ",
                              __logger_strip_path(file),
                              line);
        break;

      case LOGGER_PREFIX_FUNCTION_LINE:
        characters = snprintf(prefix, prefix_size,
                              LOGGER_FORMAT_STRING_FUNCTION ":"
                              LOGGER_FORMAT_STRING_LINE ":"
                              " ",
                              function,
                              line);
        break;

      case LOGGER_PREFIX_NAME:
        characters = snprintf(prefix, prefix_size,
                              LOGGER_FORMAT_STRING_NAME ":"
                              " ",
                              __logger_id_name_get(id));
        break;

      case LOGGER_PREFIX_NAME_FILE_FUNCTION_LINE:
        characters = snprintf(prefix, prefix_size,
                              LOGGER_FORMAT_STRING_NAME ":"
                              LOGGER_FORMAT_STRING_FILE ":"
                              LOGGER_FORMAT_STRING_FUNCTION ":"
                              LOGGER_FORMAT_STRING_LINE ":"
                              " ",
                              __logger_id_name_get(id),
                              __logger_strip_path(file),
                              function,
                              line);
        break;

      case LOGGER_PREFIX_NAME_FILE_LINE:
        characters = snprintf(prefix, prefix_size,
                              LOGGER_FORMAT_STRING_NAME ":"
                              LOGGER_FORMAT_STRING_FILE ":"
                              LOGGER_FORMAT_STRING_LINE ":"
                              " ",
                              __logger_id_name_get(id),
                              __logger_strip_path(file),
                              line);
        break;

      case LOGGER_PREFIX_NAME_FUNCTION_LINE:
        characters = snprintf(prefix, prefix_size,
                              LOGGER_FORMAT_STRING_NAME ":"
                              LOGGER_FORMAT_STRING_FUNCTION ":"
                              LOGGER_FORMAT_STRING_LINE ":"
                              " ",
                              __logger_id_name_get(id),
                              function,
                              line);
        break;

      case LOGGER_PREFIX_NAME_LEVEL:
        characters = snprintf(prefix, prefix_size,
                              LOGGER_FORMAT_STRING_NAME ":"
                              LOGGER_FORMAT_STRING_LEVEL ":"
                              " ",
                              __logger_id_name_get(id),
                              __logger_level_name_get(level));
        break;

      case LOGGER_PREFIX_NAME_LEVEL_FILE_FUNCTION_LINE:
        characters = snprintf(prefix, prefix_size,
                              LOGGER_FORMAT_STRING_NAME ":"
                              LOGGER_FORMAT_STRING_LEVEL ":"
                              LOGGER_FORMAT_STRING_FILE ":"
                              LOGGER_FORMAT_STRING_FUNCTION ":"
                              LOGGER_FORMAT_STRING_LINE ":"
                              " ",
                              __logger_id_name_get(id),
                              __logger_level_name_get(level),
                              __logger_strip_path(file),
                              function,
                              line);
        break;

      case LOGGER_PREFIX_NAME_LEVEL_FILE_LINE:
        characters = snprintf(prefix, prefix_size,
                              LOGGER_FORMAT_STRING_NAME ":"
                              LOGGER_FORMAT_STRING_LEVEL ":"
                              LOGGER_FORMAT_STRING_FILE ":"
                              LOGGER_FORMAT_STRING_LINE ":"
                              " ",
                              __logger_id_name_get(id),
                              __logger_level_name_get(level),
                              __logger_strip_path(file),
                              line);
        break;

      case LOGGER_PREFIX_NAME_LEVEL_FUNCTION_LINE:
        characters = snprintf(prefix, prefix_size,
                              LOGGER_FORMAT_STRING_NAME ":"
                              LOGGER_FORMAT_STRING_LEVEL ":"
                              LOGGER_FORMAT_STRING_FUNCTION ":"
                              LOGGER_FORMAT_STRING_LINE ":"
                              " ",
                              __logger_id_name_get(id),
                              __logger_level_name_get(level),
                              function,
                              line);
        break;

      case LOGGER_PREFIX_MAX:
        /* empty */
        prefix[0] = '\0';
        characters = 0;
        break;
    }

    /* set rev_idx to the last valid character in string */
    rev_idx = prefix_size - 1;

    /* make sure message is '\0' terminated */
    prefix[rev_idx--] = '\0';

    /* check if there was enough space in storage */
    if ((characters < 0) || (characters >= prefix_size)) {
      prefix[rev_idx--] = ' ';
      prefix[rev_idx--] = '.';
      prefix[rev_idx--] = '.';
      prefix[rev_idx--] = '.';
    }
  }
  else {
    /* clear prefix */
    prefix[0] = '\0';
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  String copy
 *
 * Copies string from `src` to `dest` until end od `src` is reached or `n`
 * characters are copied. The string in `dest` is '\0' terminated if length of
 * string in `src` is less than `n`. Otherwise the string will *not* be
 * terminated. The function returns the number of characters copied to `dest`
 * not including the '\0'.
 *
 * \param[out]    dest  Destination string.
 * \param[in]     src   Source string.
 * \param[in]     n     Maximum amount of characters available in destination string.
 *
 * \return     Number of characters written to destination string
 ******************************************************************************/
static inline size_t __logger_string_copy(char *dest, const char *src, size_t n)
{
  size_t idx;

  for (idx = 0 ; idx < n && src[idx] != '\0' ; idx++) {
    dest[idx] = src[idx];
  }

  if (idx < n) {
    dest[idx] = '\0';
  }

  return(idx);
}


/** ************************************************************************//**
 * \brief  Format log message.
 *
 * Allocate memory for message and print message to allocated memory.
 *
 * Memory needs to be deallocated using free() when the sting is not needed
 * anymore.
 *
 * \param[in]     id            ID outputting this message.
 * \param[out]    message       Formatted message.
 * \param[in]     message_size  String length of formatted message.
 * \param[in]     format        \c printf() like format string.
 * \param[in]     va_args       Argument list.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
static inline logger_return_t __logger_format_message(logger_id_t id,
                                                      char        *message,
                                                      uint16_t    message_size,
                                                      const char  *format,
                                                      va_list     argp)
{
  logger_return_t ret        = LOGGER_OK;
  int16_t         characters = 0;
  uint16_t        rev_idx;
  char            *message_end;

  /* format message */
  characters = vsnprintf(message, message_size, format, argp);

  /* set rev_idx to the last valid character in string */
  rev_idx = message_size - 1;

  /* make sure message is '\0' terminated */
  message[rev_idx--] = '\0';

  /* check if there was enough space in storage */
  if ((characters < 0) || (characters >= message_size)) {
    if (strrchr(format, '\n') != NULL) {
      message[rev_idx--] = '\n';
    }
    message[rev_idx--] = ' ';
    message[rev_idx--] = '.';
    message[rev_idx--] = '.';
    message[rev_idx--] = '.';
  }

  /* check for multi line message */
  message_end = strrchr(message, '\n');

  if (message_end != NULL) {
    /* '\n' -> will not be continued */
    logger_control[id].append = logger_false;

    /* remove '\n', needed for correct color display (see below) */
    *message_end = '\0';
  }
  else {
    /* no '\n' -> will be continued */
    logger_control[id].append = logger_true;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Output log messages to streams
 *
 * Print the log message to all outputs registered. It is possible to do
 * repeated prints to the same line by omitting '\n' in the log message format
 * sting. In this case a subsequent call will be appended without prefix. Only
 * print the message if
 *
 *   - logging is globally enabled.
 *   - logging ID is enabled.
 *   - logging level is high enough.
 *
 * \param[in]     id        ID outputting this message.
 * \param[in]     level     Level of this message.
 * \param[in]     prefix    Formatted message prefix.
 * \param[in]     message   Formatted message.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
static inline logger_return_t __logger_output(logger_id_t     id,
                                              logger_level_t  level,
                                              logger_output_t *outputs,
                                              uint16_t        size,
                                              const char      *prefix,
                                              const char      *message)
{
  logger_return_t       ret = LOGGER_OK;
  int16_t               index;
  logger_bool_t         prefix_color_print_begin;
  logger_bool_t         prefix_color_print_end;
  logger_color_string_t *prefix_color;
  logger_bool_t         message_color_print_begin;
  logger_bool_t         message_color_print_end;
  logger_color_string_t *message_color;
  size_t                line_characters;
  char                  *line;
  size_t                line_size;

  /* loop over all possible outputs */
  for (index = 0 ; index < size ; index++) {
    if ((outputs[index].count > 0) &&
        (outputs[index].level > LOGGER_UNKNOWN) &&
        (outputs[index].level < LOGGER_MAX) &&
        (outputs[index].level <= level)) {
      /* set colors */
      if (outputs[index].use_color == logger_true) {
        /* message color */
        if ((logger_color_prefix_enabled == logger_false) &&
            (logger_color_message_enabled == logger_false)) {
          prefix_color_print_begin  = logger_false;
          prefix_color_print_end    = logger_false;

          message_color_print_begin = logger_false;
          message_color_print_end   = logger_false;
        }
        else {
          if ((logger_color_prefix_enabled == logger_true) &&
              (logger_color_message_enabled == logger_false)) {
            prefix_color_print_begin = logger_true;
            prefix_color_print_end   = logger_true;
            prefix_color             = &logger_level_colors[level];

            message_color_print_begin = logger_false;
            message_color_print_end   = logger_false;
          }
          else {
            if ((logger_color_prefix_enabled == logger_false) &&
                (logger_color_message_enabled == logger_true)) {
              prefix_color_print_begin = logger_true;
              prefix_color_print_end   = logger_false;
              prefix_color             = &logger_control[id].color_string;

              message_color_print_begin = logger_false;
              message_color_print_end   = logger_true;
              message_color             = &logger_control[id].color_string;
            }
            else {
              if ((logger_color_prefix_enabled == logger_true) &&
                  (logger_color_message_enabled == logger_true)) {
                prefix_color_print_begin = logger_true;
                prefix_color_print_end   = logger_true;
                prefix_color             = &logger_level_colors[level];

                message_color_print_begin = logger_true;
                message_color_print_end   = logger_true;
                message_color             = &logger_control[id].color_string;
              }
              else {
                prefix_color_print_begin = logger_false;
                prefix_color_print_end   = logger_false;

                message_color_print_begin = logger_false;
                message_color_print_end   = logger_false;
              }
            }
          }
        }

        /* modify color printing for continued messages if color setting had not been changed since the last call */
        if (logger_control[id].color_string_changed == logger_false) {
          if (logger_control[id].append == logger_true &&
              prefix[0] != '\0') {
            /* first message in a continued message */
            /* prefix_color_print_begin unchanged */
            /* prefix_color_print_end unchanged */

            /* message_color_print_begin unchanged */
            message_color_print_end   = logger_false;
          }
          else {
            if (logger_control[id].append == logger_true &&
                prefix[0] == '\0') {
              /* inner message in a continued message */
              prefix_color_print_begin = logger_false;
              prefix_color_print_end   = logger_false;

              message_color_print_begin = logger_false;
              message_color_print_end   = logger_false;
            }
            else {
              if (logger_control[id].append == logger_false &&
                  prefix[0] == '\0') {
                /* last message in a continued message */
                prefix_color_print_begin = logger_false;
                prefix_color_print_end   = logger_false;

                message_color_print_begin = logger_false;
                /* message_color_print_end unchanged */
              }
              else {
              }
            }
          }
        }
      }
      else {
        prefix_color_print_begin = logger_false;
        prefix_color_print_end   = logger_false;

        message_color_print_begin = logger_false;
        message_color_print_end   = logger_false;
      }

      /* iniialize variables */
      line      = logger_line;
      line_size = sizeof(logger_line);

      if (prefix_color_print_begin == logger_true) {
        line_characters = __logger_string_copy(line, prefix_color->begin, line_size);
        line           += line_characters;
        line_size      -= line_characters;
      }

      /* actually output prefix */
      line_characters = __logger_string_copy(line, prefix, line_size);
      line           += line_characters;
      line_size      -= line_characters;

      if (prefix_color_print_end == logger_true) {
        line_characters = __logger_string_copy(line, prefix_color->end, line_size);
        line           += line_characters;
        line_size      -= line_characters;
      }

      if (message_color_print_begin == logger_true) {
        line_characters = __logger_string_copy(line, message_color->begin, line_size);
        line           += line_characters;
        line_size      -= line_characters;
      }

      /* actually output message */
      line_characters = __logger_string_copy(line, message, line_size);
      line           += line_characters;
      line_size      -= line_characters;

      if (message_color_print_end == logger_true) {
        line_characters = __logger_string_copy(line, message_color->end, line_size);
        line           += line_characters;
        line_size      -= line_characters;
      }

      /* print '\n' if needed. color reset needs to be printed before '\n', otherwise some terminals show wrong colors in next line */
      if (logger_control[id].append == logger_false) {
        line_characters = __logger_string_copy(line, "\n", line_size);
        line           += line_characters;
        line_size      -= line_characters;
      }

      /* make sure line is '\0' terminated */
      logger_line[sizeof(logger_line) - 1] = '\0';

      switch(outputs[index].type) {
        case LOGGER_OUTPUT_TYPE_UNKNOWN:
          /* nothing */
          break;

        case LOGGER_OUTPUT_TYPE_FILESTREAM:
          /* put the message to stream */
          fputs(logger_line, outputs[index].stream);
#ifdef LOGGER_FORCE_FLUSH
          (void)fflush(outputs[index].stream);
#endif  /* LOGGER_FORCE_FLUSH */
          break;

        case LOGGER_OUTPUT_TYPE_FUNCTION:
          /* call user provided function */
          (*outputs[index].function)(logger_line);
          break;

        case LOGGER_OUTPUT_TYPE_MAX:
          /* nothing */
          break;
      }
    }
  }

  /* reset the color string changed flag */
  logger_control[id].color_string_changed = logger_false;

  return(ret);
}


/** ************************************************************************//**
 * \brief  Print log message.
 *
 * Print the log message to all outputs registered. It is possible to do
 * repeated prints to the same line by omitting '\n' in the log message format
 * sting. In this case a subsequent call will be appended without prefix. Only
 * print the message if
 *
 *   - logging is globally enabled.
 *   - logging ID is enabled.
 *   - logging level is high enough.
 *
 * \param[in]     id        ID outputting this message.
 * \param[in]     level     Level of this message.
 * \param[in]     file      Name of file where this call happend.
 * \param[in]     function  Name of function where this call happend.
 * \param[in]     line      Line where this call happend.
 * \param[in]     format    \c printf() like format string.
 * \param[in]     va_args   Argument list.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger(logger_id_t    id,
                         logger_level_t level,
                         const char     *file,
                         const char     *function,
                         uint32_t       line,
                         const char     *format,
                         ...)
{
  logger_return_t ret = LOGGER_OK;
  va_list         argp;
  char            *message_part;
  char            *message_end;

  /* check for valid ID */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX) &&
      (logger_enabled == logger_true)) {
    /* check for valid level */
    if ((level > LOGGER_UNKNOWN) &&
        (level < LOGGER_MAX)) {
      /* check if file valid */
      if (file != NULL) {
        /* check if function valid */
        if (function != NULL) {
          /* check if format valid */
          if (format != NULL) {
            /* check if ID is enabled and level is high enough */
            if ((logger_control[id].enabled == logger_true) &&
                (logger_control[id].level > LOGGER_UNKNOWN) &&
                (logger_control[id].level < LOGGER_MAX) &&
                (logger_control[id].level <= level)) {
              /* format prefix */
              __logger_format_prefix(id, logger_prefix, sizeof(logger_prefix), level, file, function, line);

              /* format message */
              va_start(argp, format);
              __logger_format_message(id, logger_message, sizeof(logger_message), format, argp);
              va_end(argp);

              /* initialize message pointer */
              message_part = logger_message;

              /* loop over all message parts */
              do {
                /* search for the next linefeed */
                message_end = strchr(message_part, '\n');

                if (message_end != NULL) {
                  /* replace linefeed with string end */
                  *message_end = '\0';

                  /* make message_end point to the next message part */
                  message_end++;
                }

                /* output message to global streams */
                __logger_output(id, level, logger_outputs, LOGGER_OUTPUTS_MAX, logger_prefix, message_part);

                /* output message to id streams */
                __logger_output(id, level, logger_control[id].outputs, LOGGER_ID_OUTPUTS_MAX, logger_prefix, message_part);

                /* update message part for next loop */
                message_part = message_end;
              }
              while (message_part != NULL);
            }
          }
          else {
            ret = LOGGER_ERR_FORMAT_INVALID;
          }
        }
        else {
          ret = LOGGER_ERR_OUTPUT_INVALID;
        }
      }
      else {
        ret = LOGGER_ERR_FILE_INVALID;
      }
    }
    else {
      ret = LOGGER_ERR_LEVEL_UNKNOWN;
    }
  }
  else {
    ret = LOGGER_ERR_ID_UNKNOWN;
  }

  return(ret);
}


#endif /* LOGGER_ENABLE */
