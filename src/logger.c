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
 * \date   2010-10-13
 ******************************************************************************/
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <stddef.h>
#include <fcntl.h>
#include "logger.h"

#ifdef LOGGER_ENABLE

#define LOGGER_OUTPUTS_MAX    256                          /**< Number of possible simultaneous outputs. */
#define LOGGER_IDS_MAX        256                          /**< Number of possible ids. */

typedef struct logger_control_s {
  logger_bool_t      used;    /**< This id is used. */
  logger_bool_t      enabled; /**< This id is enabled. */
  logger_level_t     level;   /**< Level for this id. */
  logger_bool_t      color;   /**< Changed colors for this id. */
  logger_text_fg_t   fg;      /**< Foreground color of this id. */
  logger_text_bg_t   bg;      /**< Background color of this id. */
  logger_text_attr_t attr;    /**< Attributes of this id. */
  logger_bool_t      cont;    /**< Previous message didn't contain a newline, thus omit prefix for next message. */
} logger_control_t;

typedef struct logger_output_s {
  int16_t        count;   /**< Number of registrations for this stream. */
  logger_level_t level;   /**< Level for this stream. */
  FILE           *stream; /**< File pointer given during registration. */
} logger_output_t;

static logger_bool_t    logger_enabled;                     /**< Logger is enabled. */
static logger_control_t logger_control[LOGGER_IDS_MAX];     /**< Control storage for possible ids. */
static logger_output_t  logger_outputs[LOGGER_OUTPUTS_MAX]; /**< Storage for possible output streams. */

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
  static logger_bool_t logger_initialized = 0;
  logger_return_t ret = LOGGER_OK;

  if (logger_initialized == logger_false) {
    logger_enabled = logger_true;
    memset(logger_control, 0, sizeof(logger_control));
    memset(logger_outputs, 0, sizeof(logger_outputs));
    logger_initialized = logger_true;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Enable logger.
 *
 * Enable logger globally for all ids and all outputs.
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
 * Disable logger globally for all ids and all outputs.
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
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_bool_t __logger_is_enabled()
{
  return(logger_enabled);
}


/** ************************************************************************//**
 * \brief  Register an output stream.
 *
 * The given file stream may be on of \c stdout, \c stderr or a file stream
 * opened by the user. The default logging level is set to \c LOGGER_DEBUG thus
 * all messages will appear on this stream.
 *
 * \param[in]     stream  Opened file stream.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_output_register(FILE *stream)
{
  logger_return_t ret = LOGGER_OK;
  int16_t         index;
  logger_bool_t   found;

  /* check if stream is not NULL */
  if (stream != NULL) {
    /* check if this output stream is already registered */
    found = logger_false;
    for (index = 0 ; index < LOGGER_OUTPUTS_MAX ; index++) {
      if (logger_outputs[index].stream == stream) {
        found = logger_true;
        break;
      }
    }

    /* stream is already registered */
    if (found == logger_true) {
      logger_outputs[index].count++;
      ret = LOGGER_ERR_OUTPUT_REGISTERED;
    }
    /* stream is not registered */
    else {
      /* search for an empty slot */
      found = logger_false;
      for (index = 0 ; index < LOGGER_OUTPUTS_MAX ; index++) {
        if (logger_outputs[index].count == 0) {
          found = logger_true;
          break;
        }
      }

      /* found an empty slot */
      if (found == logger_true) {
        logger_outputs[index].count++;
        logger_outputs[index].level  = LOGGER_DEBUG;
        logger_outputs[index].stream = stream;

        /* make a nonblocking stream */
        //fcntl(fileno(stream), F_SETFL, fcntl(fileno(stream), F_GETFL) | O_NONBLOCK);
      }
      else {
        ret = LOGGER_ERR_OUTPUTS_FULL;
      }
    }
  }
  else {
    ret = LOGGER_ERR_OUTPUT_INVALID;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Unregister an output stream.
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
  int16_t         index;
  logger_bool_t   found;

  /* check if this output is already registered */
  found = logger_false;
  for (index = 0 ; index < LOGGER_OUTPUTS_MAX ; index++) {
    if (logger_outputs[index].stream == stream) {
      found = logger_true;
      break;
    }
  }

  /* found given stream in a slot */
  if (found == logger_true) {
    logger_outputs[index].count--;

    /* remove this stream if this was the last reference */
    if (logger_outputs[index].count <= 0) {
      logger_outputs[index].count  = 0;
      logger_outputs[index].level  = LOGGER_UNKNOWN;
      logger_outputs[index].stream = (FILE *)NULL;
    }
  }
  else {
    ret = LOGGER_ERR_OUTPUT_NOT_FOUND;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Set logging level for output stream.
 *
 * Set the minimum logging level for given output stream. Only log messages
 * equal or above the given level will be printed to the given stream.
 *
 * \param[in]     stream  Previous registered file stream.
 * \param[in]     level   Level to set.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_output_level_set(FILE           *stream,
                                          logger_level_t level)
{
  logger_return_t ret = LOGGER_OK;
  int16_t         index;
  logger_bool_t   found;

  /* check for valid level */
  if ((level >= LOGGER_DEBUG) &&
      (level <= LOGGER_MAX)) {
    /* check if this output is already registered */
    found = logger_false;
    for (index = 0 ; index < LOGGER_OUTPUTS_MAX ; index++) {
      if (logger_outputs[index].stream == stream) {
        found = logger_true;
        break;
      }
    }

    /* found given stream in a slot */
    if (found == logger_true) {
      /* set log level */
      logger_outputs[index].level = level;
    }
    else {
      ret = LOGGER_ERR_OUTPUT_NOT_FOUND;
    }
  }
  else {
    ret = LOGGER_ERR_LEVEL_UNKNOWN;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Query logging level for output stream.
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
  int16_t        index;
  logger_bool_t  found;

  /* check if this output is already registered */
  found = logger_false;
  for (index = 0 ; index < LOGGER_OUTPUTS_MAX ; index++) {
    if (logger_outputs[index].stream == stream) {
      found = logger_true;
      break;
    }
  }

  /* found given stream in a slot */
  if (found == logger_true) {
    /* set log level */
    ret = logger_outputs[index].level;
  }
  else {
    ret = LOGGER_ERR_OUTPUT_NOT_FOUND;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Request a logging id.
 *
 * Request a id for later use with calls to \c logger(). Each logging id is
 * disabled by default.
 *
 * \return        Id number if id is available, error code otherwise.
 ******************************************************************************/
logger_id_t __logger_id_request(void)
{
  logger_return_t ret = LOGGER_OK;
  int16_t         index;
  logger_bool_t   found;

  /* search for an available id */
  found = logger_false;
  for (index = 0 ; index < LOGGER_OUTPUTS_MAX ; index++) {
    if (logger_control[index].used == logger_false) {
      found = logger_true;
      break;
    }
  }

  /* found an empty slot */
  if (found == logger_true) {
    logger_control[index].used = logger_true;
    ret = index;
  }
  else {
    ret = LOGGER_ERR_IDS_FULL;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Release a logging id.
 *
 * Release a id. After this call logging to this id is not possible anymore.
 *
 * \param[in]     id      Id to enable.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_id_release(logger_id_t id)
{
  logger_return_t ret = LOGGER_OK;

  /* check for valid id */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX)) {
    if (logger_control[id].used == logger_true) {
      /* reset all id dependent values to defaults */
      logger_control[id].used    = logger_false;
      logger_control[id].enabled = logger_false;
      logger_control[id].level   = LOGGER_DEBUG;
    }
    else {
      ret = LOGGER_ERR_ID_NOT_FOUND;
    }
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Enable a logging id for output.
 *
 * Enable given logging id for all outputs.
 *
 * \param[in]     id      Id to enable.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_id_enable(logger_id_t id)
{
  logger_return_t ret = LOGGER_OK;

  /* check for valid id */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX)) {
    /* enable given id */
    logger_control[id].enabled = logger_true;
  }
  else {
    ret = LOGGER_ERR_ID_UNKNOWN;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Disable a logger id for output.
 *
 * Disable given logging id for all outputs.
 *
 * \param[in]     id      Id to disable.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise
 ******************************************************************************/
logger_return_t __logger_id_disable(logger_id_t id)
{
  logger_return_t ret = LOGGER_OK;

  /* check for valid id */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX)) {
    /* disable given id */
    logger_control[id].enabled = logger_false;
  }
  else {
    ret = LOGGER_ERR_ID_UNKNOWN;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Query the current enable state of id.
 *
 * Query the current enable state of given logging id.
 *
 * \param[in]     id      Id to check enable state.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_bool_t __logger_id_is_enabled(logger_id_t id)
{
  logger_bool_t ret = logger_false;

  /* check for valid id */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX)) {
    /* id enable state */
    ret = logger_control[id].enabled;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Set logging level for id.
 *
 * Set the minimum logging level for given id. Only log messages equal or above
 * the given level will be printed to output streams.
 *
 * \param[in]     id      Id for setting level.
 * \param[in]     level   Level to set.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_id_level_set(logger_id_t    id,
                                      logger_level_t level)
{
  logger_return_t ret = LOGGER_OK;

  /* check for valid id */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX)) {
    /* check for valid level */
    if ((level >= LOGGER_DEBUG) &&
        (level <= LOGGER_MAX)) {
      /* set id level */
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
 * \brief  Query logging level for id.
 *
 * Query the currently set minimum level for the given logging id.
 *
 * \param[in]     id      Id for setting level
 *
 * \return        Currently set level
 ******************************************************************************/
logger_level_t __logger_id_level_get(logger_id_t id)
{
  logger_level_t ret = LOGGER_UNKNOWN;

  /* check for valid id */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX)) {
    /* get id level */
    ret = logger_control[id].level;
  }

  return(ret);
}


#ifdef LOGGER_COLORS


/** ************************************************************************//**
 * \brief  Change terminal text color and attributes.
 *
 * Change text color and attributes for all messages of given id when they are
 * printed to \c stdout or \c stdin. Outputs to other streams including files
 * will have no
 * color.
 *
 * \param[in]     id      Id for setting level.
 * \param[in]     fg      Text foreground.
 * \param[in]     bg      Text background.
 * \param[in]     attr    Text attribute.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_color_set(logger_id_t        id,
                                   logger_text_fg_t   fg,
                                   logger_text_bg_t   bg,
                                   logger_text_attr_t attr)
{
  logger_level_t ret = LOGGER_OK;

  logger_control[id].color = logger_true;
  logger_control[id].fg    = fg;
  logger_control[id].bg    = bg;
  logger_control[id].attr  = attr;

  return(ret);
}


/** ************************************************************************//**
 * \brief  Reset terminal text color and attributes.
 *
 * Reset text color and attributes of given id back to defaults.
 *
 * \param[in]     id      Id for setting level.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_color_reset(logger_id_t id)
{
  logger_level_t ret = LOGGER_OK;

  logger_control[id].color = logger_false;

  return(ret);
}


#endif /* LOGGER_COLORS */


/** ************************************************************************//**
 * \brief  Print log message prefix.
 *
 * Print the log message prefix to all output streams registered. Only print
 * the prefix if
 *
 *   - previous logging call ended with a '\n' i.e. is complete.
 *   - logging is globally enabled.
 *   - logging id is enabled.
 *   - logging level is high enough.
 *
 * \param[in]     id      Id outputting this message.
 * \param[in]     level   Level of this message.
 * \param[in]     format  \c printf() like format string.
 * \param[in]     va_args Argument list.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_prefix(logger_id_t    id,
                                logger_level_t level,
                                const char     *format,
                                ...)
{
  logger_return_t ret = LOGGER_OK;
  va_list         argp;
  int16_t         index;

  /* check for valid id */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX) &&
      (logger_enabled == logger_true)) {
    /* check for valid level */
    if ((level >= LOGGER_DEBUG) &&
        (level <= LOGGER_MAX)) {
      /* check if id is enabled and level is high enough */
      if ((logger_control[id].enabled == logger_true) &&
          (logger_control[id].level <= level) &&
          (logger_control[id].cont == logger_false)) {
        /* initialize variable arguments */
        va_start(argp, format);

        /* loop over all possible outputs */
        for (index = 0 ; index < LOGGER_OUTPUTS_MAX ; index++) {
          if ((logger_outputs[index].count > 0) &&
              (logger_outputs[index].level <= level)) {
            /* set color */
#ifdef LOGGER_COLORS
            if ((logger_control[id].color == logger_true) &&
                ((logger_outputs[index].stream == stdout) ||
                 (logger_outputs[index].stream == stderr))) {
              (void)fprintf(logger_outputs[index].stream, "%c[%d;%d;%dm", 0x1B, logger_control[id].attr, logger_control[id].fg, logger_control[id].bg);
            }
#endif      /* LOGGER_COLORS */

            /* print message */
            (void)vfprintf(logger_outputs[index].stream, format, argp);
          }
        }

        /* end variable arguments */
        va_end(argp);
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


/** ************************************************************************//**
 * \brief  Print log message.
 *
 * Print the log message to all output streams registered. It is possible to do
 * repeated prints to the same line by omitting '\n' in the log message format
 * sting. In this case a subsequent call will be appended without prefix. Only
 * print the message if
 *
 *   - logging is globally enabled.
 *   - logging id is enabled.
 *   - logging level is high enough.
 *
 * \param[in]     id      Id outputting this message.
 * \param[in]     level   Level of this message.
 * \param[in]     format  \c printf() like format string.
 * \param[in]     va_args Argument list.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t __logger_msg(logger_id_t    id,
                             logger_level_t level,
                             const char     *format,
                             ...)
{
  logger_return_t ret = LOGGER_OK;
  va_list         argp;
  int16_t         index;
  logger_bool_t   cont;

  /* check for valid id */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX) &&
      (logger_enabled == logger_true)) {
    /* check for valid level */
    if ((level >= LOGGER_DEBUG) &&
        (level <= LOGGER_MAX)) {
      /* check for multi line message */
      cont = logger_true;
      for (index = 0 ; format[index] != '\0' ; index++) {
        if (format[index] == '\n') {
          cont = logger_false;
          break;
        }
      }

      /* check if id is enabled and level is high enough */
      if ((logger_control[id].enabled == logger_true) &&
          (logger_control[id].level <= level)) {
        logger_control[id].cont = cont;

        /* initialize variable arguments */
        va_start(argp, format);

        /* loop over all possible outputs */
        for (index = 0 ; index < LOGGER_OUTPUTS_MAX ; index++) {
          if ((logger_outputs[index].count > 0) &&
              (logger_outputs[index].level <= level)) {
            /* print message */
            (void)vfprintf(logger_outputs[index].stream, format, argp);

            /* reset color */
#ifdef LOGGER_COLORS
            if ((logger_control[id].color == logger_true) &&
                (logger_control[id].cont == logger_false) &&
                ((logger_outputs[index].stream == stdout) ||
                 (logger_outputs[index].stream == stderr))) {
              (void)fprintf(logger_outputs[index].stream, "%c[%dm", 0x1B, LOGGER_ATTR_RESET);
            }
#endif      /* LOGGER_COLORS */
          }
        }

        /* end variable arguments */
        va_end(argp);
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
