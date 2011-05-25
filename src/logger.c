/***************************************************************************//**
 * \file   logger.c
 *
 * \brief  logging facility for C
 * \author Markus Braun
 * \date   2010-10-13
 ******************************************************************************/
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include "logger.h"

#ifdef LOGGER_ENABLE

#define LOGGER_OUTPUTS_MAX    16                           /**< number of possible simultaneous outputs */
#define LOGGER_IDS_MAX        16                           /**< number of possible ids */

typedef struct logger_control_s {
  logger_bool_t      used;    /**< this id is used */
  logger_bool_t      enabled; /**< this id is enabled */
  logger_level_t     level;   /**< level for this id */
  logger_bool_t      color;   /**< changed colors for this id */
  logger_text_fg_t   fg;      /**< foreground color of this id */
  logger_text_bg_t   bg;      /**< background color of this id */
  logger_text_attr_t attr;    /**< attributes of this id */
  logger_bool_t      cont;    /**< previous message didn't contain a newline, thus omit prefix for next message */
} logger_control_t;

typedef struct logger_output_s {
  int            count;   /**< number of registrations for this stream */
  logger_level_t level;   /**< level for this stream */
  FILE           *stream; /**< file pointer given during registration */
} logger_output_t;

static logger_bool_t    logger_enabled;                     /**< logger is enabled */
static logger_control_t logger_control[LOGGER_IDS_MAX];     /**< control storage for possible ids */
static logger_output_t  logger_outputs[LOGGER_OUTPUTS_MAX]; /**< storage for possible output streams */

/** ************************************************************************//**
 * \brief  initialize logger
 *
 * \return     LOGGER_OK if no error occurred, error code otherwise
 ******************************************************************************/
logger_return_t __logger_init(void)
{
  logger_return_t ret = LOGGER_OK;

  logger_enabled = logger_true;
  memset(logger_control, 0, sizeof(logger_control));
  memset(logger_outputs, 0, sizeof(logger_outputs));

  return(ret);
}


/** ************************************************************************//**
 * \brief  enable logger
 *
 * \return     LOGGER_OK if no error occurred, error code otherwise
 ******************************************************************************/
logger_return_t __logger_enable(void)
{
  logger_return_t ret = LOGGER_OK;

  logger_enabled = logger_true;

  return(ret);
}


/** ************************************************************************//**
 * \brief  disable logger
 *
 * \return     LOGGER_OK if no error occurred, error code otherwise
 ******************************************************************************/
logger_return_t __logger_disable(void)
{
  logger_return_t ret = LOGGER_OK;

  logger_enabled = logger_false;

  return(ret);
}


/** ************************************************************************//**
 * \brief  check if logger is enabled
 *
 * \return     LOGGER_OK if no error occurred, error code otherwise
 ******************************************************************************/
logger_bool_t __logger_is_enabled()
{
  return(logger_enabled);
}


/** ************************************************************************//**
 * \brief  register an output stream to logger
 *
 * the given file stream may be on of stdout, stderr or a file stream opened by
 * the user. the default logging level is set to LOGGER_DEBUG thus all messages
 * will appear on this stream.
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
 * \brief  set required minimum level for logging on this output
 *
 * \param[in]     stream previous registered file stream
 * \param[in]     level level to set
 *
 * \return     LOGGER_OK if no error occurred, error code otherwise
 ******************************************************************************/
logger_return_t __logger_output_level_set(FILE           *stream,
                                          logger_level_t level)
{
  logger_return_t ret = LOGGER_OK;
  int             index;
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
 * \brief  get currently set minimum level for logging on this
 *
 * \param[in]     stream previous registered file stream
 *
 * \return     currently set level
 ******************************************************************************/
logger_level_t __logger_output_level_get(FILE *stream)
{
  logger_level_t ret = LOGGER_UNKNOWN;
  int            index;
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
 * \brief  request a id from logger
 *
 * \return     id number if id is available, error code otherwise
 ******************************************************************************/
logger_id_t __logger_id_request(void)
{
  logger_return_t ret = LOGGER_OK;
  int             index;
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
 * \brief  release a logger id
 *
 * \param[in]     id id to enable
 *
 * \return     LOGGER_OK if no error occurred, error code otherwise
 ******************************************************************************/
logger_return_t __logger_id_release(logger_id_t id)
{
  logger_return_t ret = LOGGER_OK;

  /* check for valid id */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX)) {
    if (logger_control[id].used == logger_true) {
      /* reset all id dependend values to defaults */
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
 * \brief  enable a logger id for output
 *
 * \param[in]     id id to enable
 *
 * \return     LOGGER_OK if no error occurred, error code otherwise
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
 * \brief  disable a logger id for output
 *
 * \param[in]     id id to disable
 *
 * \return     LOGGER_OK if no error occurred, error code otherwise
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
 * \brief  check if logging of given id is enabled
 *
 * \param[in]     id id to check enable state
 *
 * \return     LOGGER_OK if no error occurred, error code otherwise
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
 * \brief  set required minimum level for log output
 *
 * \param[in]     id    id for setting level
 * \param[in]     level level to set
 *
 * \return     LOGGER_OK if no error occurred, error code otherwise
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
 * \brief  get currently set minimum level for log output
 *
 * \param[in]     id id for setting level
 *
 * \return     currently set level
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
 * \brief  change terminal text color and attributes
 *
 * \param[in]     id      id for setting level
 * \param[in]     fg      text foreground
 * \param[in]     bg      text background
 * \param[in]     attr    text attribute
 *
 * \return     LOGGER_OK if no error occurred, error code otherwise
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
 * \brief  reset terminal text color and attributes
 *
 * \param[in]     id      id for setting level
 *
 * \return     LOGGER_OK if no error occurred, error code otherwise
 ******************************************************************************/
logger_return_t __logger_color_reset(logger_id_t id)
{
  logger_level_t ret = LOGGER_OK;

  logger_control[id].color = logger_false;

  return(ret);
}


#endif /* LOGGER_COLORS */


/** ************************************************************************//**
 * \brief  output log message prefix
 *
 * \param[in]     id      id outputting this message
 * \param[in]     level   level of this message
 * \param[in]     format  printf like format string
 * \param[in]     va_args argument list
 *
 * \return     LOGGER_OK if no error occurred, error code otherwise
 ******************************************************************************/
logger_return_t __logger_prefix(logger_id_t    id,
                                logger_level_t level,
                                const char     *format,
                                ...)
{
  logger_return_t ret = LOGGER_OK;
  va_list         argp;
  int             index;

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

        /* loop over all possibe outputs */
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

        /* deinitialize variable arguments */
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
 * \brief  output log message
 *
 * \param[in]     id      id outputting this message
 * \param[in]     level   level of this message
 * \param[in]     format  printf like format string
 * \param[in]     va_args argument list
 *
 * \return     LOGGER_OK if no error occurred, error code otherwise
 ******************************************************************************/
logger_return_t __logger_msg(logger_id_t    id,
                             logger_level_t level,
                             const char     *format,
                             ...)
{
  logger_return_t ret = LOGGER_OK;
  va_list         argp;
  int             index;
  logger_bool_t   cont;

  /* check for valid id */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX) &&
      (logger_enabled == logger_true)) {
    /* check for valid level */
    if ((level >= LOGGER_DEBUG) &&
        (level <= LOGGER_MAX)) {
      /* check for multiline message */
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

        /* loop over all possibe outputs */
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

        /* deinitialize variable arguments */
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
