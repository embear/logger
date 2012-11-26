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
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <stddef.h>
#include <fcntl.h>
#include "logger.h"

#ifdef LOGGER_ENABLE

/** Number of possible simultaneous outputs. */
#define LOGGER_OUTPUTS_MAX           (16)

/** Number of possible outputs per ID. */
#define LOGGER_ID_OUTPUTS_MAX        (4)

/** Number of possible ids. */
#define LOGGER_IDS_MAX               (256)

/** Length of logger ID name including '\0' */
#define LOGGER_NAME_MAX              (256)

/** Length of logger prefix string including '\0' */
#define LOGGER_PREFIX_STRING_MAX     (128)

/** Length of logger message string including '\0' */
#define LOGGER_MESSAGE_STRING_MAX    (512)

typedef struct logger_output_s {
  int16_t        count;   /**< Number of registrations for this stream. */
  logger_level_t level;   /**< Level for this stream. */
  FILE           *stream; /**< File pointer given during registration. */
} logger_output_t;

typedef struct  logger_control_s {
  logger_bool_t      cont;                           /**< Previous message didn't contain a newline, thus omit prefix for next message. */
  logger_bool_t      used;                           /**< This ID is used. */
  int16_t            count;                          /**< Number of registrations for this ID. */
  logger_bool_t      enabled;                        /**< This ID is enabled. */
  logger_level_t     level;                          /**< Level for this ID. */
  logger_prefix_t    prefix;                         /**< Prefix for this ID. */
  logger_bool_t      color;                          /**< Changed colors for this ID. */
#ifdef LOGGER_COLORS
  logger_text_fg_t   fg;                             /**< Foreground color of this ID. */
  logger_text_bg_t   bg;                             /**< Background color of this ID. */
  logger_text_attr_t attr;                           /**< Attributes of this ID. */
#endif      /* LOGGER_COLORS */
  char               name[LOGGER_NAME_MAX];          /**< Name of this logger ID. */
  logger_output_t    outputs[LOGGER_ID_OUTPUTS_MAX]; /**< Storage for possible ID output streams. */
} logger_control_t;

static logger_bool_t    logger_enabled;                     /**< Logger is enabled. */
static logger_control_t logger_control[LOGGER_IDS_MAX];     /**< Control storage for possible IDs. */
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
 * \brief  Register an output stream to a list of outputs.
 *
 * The given file stream may be on of \c stdout, \c stderr or a file stream
 * opened by the user. The default logging level is set to \c LOGGER_DEBUG thus
 * all messages will appear on this stream.
 *
 * \param[inout]  outputs List of logger file stream.
 * \param[in]     size    Number of elements in list.
 * \param[in]     stream  Opened file stream.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
static logger_return_t __logger_output_common_register(logger_output_t *outputs,
                                                       const uint16_t  size,
                                                       FILE            *stream)
{
  logger_return_t ret = LOGGER_OK;
  int16_t         index;
  logger_bool_t   found;

  /* check if stream is not NULL */
  if (stream != NULL) {
    /* check if this output stream is already registered */
    found = logger_false;
    for (index = 0 ; index < size ; index++) {
      if (outputs[index].stream == stream) {
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
        outputs[index].level  = LOGGER_DEBUG;
        outputs[index].stream = stream;

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

  return(ret);
}


/** ************************************************************************//**
 * \brief  Unregister an output stream from a list of outputs.
 *
 * Remove given file stream from list of outputs.
 *
 * \param[inout]  outputs List of logger file stream.
 * \param[in]     size    Number of elements in list.
 * \param[in]     stream  Opened file stream.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
static logger_return_t __logger_output_common_deregister(logger_output_t *outputs,
                                                         const uint16_t  size,
                                                         FILE            *stream)
{
  logger_return_t ret = LOGGER_OK;
  int16_t         index;
  logger_bool_t   found;

  /* check if this output is already registered */
  found = logger_false;
  for (index = 0 ; index < size ; index++) {
    if (outputs[index].stream == stream) {
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
      outputs[index].count  = 0;
      outputs[index].level  = LOGGER_UNKNOWN;
      outputs[index].stream = (FILE *)NULL;
    }
  }
  else {
    ret = LOGGER_ERR_OUTPUT_NOT_FOUND;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Set logging level for output stream in a list of outputs.
 *
 * Set the minimum logging level for given output stream. Only log messages
 * equal or above the given level will be printed to the given stream.
 *
 * \param[inout]  outputs List of logger file stream.
 * \param[in]     size    Number of elements in list.
 * \param[in]     stream  Opened file stream.
 * \param[in]     level   Level to set.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
static logger_return_t __logger_output_common_level_set(logger_output_t      *outputs,
                                                        const uint16_t       size,
                                                        FILE                 *stream,
                                                        const logger_level_t level)
{
  logger_return_t ret = LOGGER_OK;
  int16_t         index;
  logger_bool_t   found;

  /* check for valid level */
  if ((level >= LOGGER_DEBUG) &&
      (level <= LOGGER_MAX)) {
    /* check if this output is already registered */
    found = logger_false;
    for (index = 0 ; index < size ; index++) {
      if (outputs[index].stream == stream) {
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

  return(ret);
}


/** ************************************************************************//**
 * \brief  Query logging level for output stream in a list of outputs.
 *
 * Query the currently set minimum level for the given logging output stream.
 *
 * \param[inout]  outputs List of logger file stream.
 * \param[in]     size    Number of elements in list.
 * \param[in]     stream  Opened file stream.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
static logger_level_t __logger_output_common_level_get(logger_output_t      *outputs,
                                                       const uint16_t       size,
                                                       FILE                 *stream)
{
  logger_level_t ret = LOGGER_UNKNOWN;
  int16_t        index;
  logger_bool_t  found;

  /* check if this output is already registered */
  found = logger_false;
  for (index = 0 ; index < size ; index++) {
    if (outputs[index].stream == stream) {
      found = logger_true;
      break;
    }
  }

  /* found given stream in a slot */
  if (found == logger_true) {
    /* set log level */
    ret = outputs[index].level;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Register an global output stream.
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

  /* add stream to global output streams */
  ret = __logger_output_common_register(logger_outputs, LOGGER_OUTPUTS_MAX, stream);

  return(ret);
}


/** ************************************************************************//**
 * \brief  Unregister an global output stream.
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

  /* delete stream to global output streams */
  ret = __logger_output_common_deregister(logger_outputs, LOGGER_OUTPUTS_MAX, stream);

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

  /* set stream output level to global output streams */
  ret = __logger_output_common_level_set(logger_outputs, LOGGER_OUTPUTS_MAX, stream, level);

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

  /* get stream output level to global output streams */
  ret = __logger_output_common_level_get(logger_outputs, LOGGER_OUTPUTS_MAX, stream);

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

  /* search for an empty slot */
  for (index = 0 ; index < LOGGER_OUTPUTS_MAX ; index++) {
    if (logger_outputs[index].count > 0) {
      fflush(logger_outputs[index].stream);
    }
  }

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
 * \return        Id number if ID is available, error code otherwise.
 ******************************************************************************/
logger_id_t __logger_id_request(const char* name)
{
  logger_return_t ret = LOGGER_OK;
  int16_t         index;
  logger_bool_t   found;

  /* start search */
  found = logger_false;

  /* search for an already existing ID with the same name */
  for (index = 0 ; index < LOGGER_OUTPUTS_MAX ; index++) {
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
    for (index = 0 ; index < LOGGER_OUTPUTS_MAX ; index++) {
      if (logger_control[index].used == logger_false) {
        found = logger_true;
        /* reset the ID */
        memset(&logger_control[index], 0, sizeof(logger_control[index]));

        /* initialize thethe  ID */
        logger_control[index].used    = logger_true;
        logger_control[index].count   = 1;
        logger_control[index].enabled = logger_false;
        logger_control[index].level   = LOGGER_DEBUG;
        logger_control[index].prefix  = LOGGER_PREFIX_FULL;
        logger_control[index].color   = logger_false;

        /* copy the name */
        strncpy(logger_control[index].name, name, LOGGER_NAME_MAX);
        logger_control[index].name[LOGGER_NAME_MAX - 1] = '\0';

        break;
      }
    }
  }

  /* found an empty slot */
  if (found == logger_true) {
    ret = index;
  }
  else {
    ret = LOGGER_ERR_IDS_FULL;
  }

  return(ret);
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
        logger_control[id].used    = logger_false;
        logger_control[id].count = 0;
        logger_control[id].enabled = logger_false;
        logger_control[id].level   = LOGGER_DEBUG;
        logger_control[id].color   = logger_false;
        logger_control[id].name[0] = '\0';

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
 * the given level will be printed to output streams.
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
    if ((level >= LOGGER_DEBUG) &&
        (level <= LOGGER_MAX)) {
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
logger_return_t __logger_id_prefix_set(const logger_id_t    id,
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
 * opened by the user. The default logging level is set to \c LOGGER_DEBUG thus
 * all messages will appear on this stream.
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

  /* add stream to global output streams */
  ret = __logger_output_common_register(logger_control[id].outputs, LOGGER_ID_OUTPUTS_MAX, stream);

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

  /* delete stream to global output streams */
  ret = __logger_output_common_deregister(logger_control[id].outputs, LOGGER_ID_OUTPUTS_MAX, stream);

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

  /* set stream output level to global output streams */
  ret = __logger_output_common_level_set(logger_control[id].outputs, LOGGER_ID_OUTPUTS_MAX, stream, level);

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

  /* get stream output level to global output streams */
  ret = __logger_output_common_level_get(logger_control[id].outputs, LOGGER_ID_OUTPUTS_MAX, stream);

  return(ret);
}


#ifdef LOGGER_COLORS


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
    logger_control[id].fg    = fg;
    logger_control[id].bg    = bg;
    logger_control[id].attr  = attr;
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
    logger_control[id].color = logger_false;
  }
  else {
    ret = LOGGER_ERR_ID_UNKNOWN;
  }

  return(ret);
}


#endif /* LOGGER_COLORS */


/** ************************************************************************//**
 * \brief  Format message prefix
 *
 * Allocate memory for prefix and print prefix to allocated memory.
 *
 * Memory needs to be deallocated using free() when the sting is not needed anymore.
 *
 * \param[in]     id        ID outputting this message.
 * \param[out]    prefix    Formatted message prefix.
 * \param[in]     level     Level of this message.
 * \param[in]     level_str Level of this message as string.
 * \param[in]     file      Name of file where this call happend.
 * \param[in]     function  Name of function where this call happend.
 * \param[in]     line      Line where this call happend.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
static inline logger_return_t __logger_format_prefix(logger_id_t    id,
                                                     char           **prefix,
                                                     logger_level_t level,
                                                     const char     *level_str,
                                                     const char     *file,
                                                     const char     *function,
                                                     uint32_t       line)
{
  logger_return_t ret = LOGGER_OK;

  /* do prefix stuff only if needed */
  if (logger_control[id].cont == logger_false) {
    /* allocate storage for prefix */
    *prefix = (char *)malloc(LOGGER_PREFIX_STRING_MAX * sizeof(char));

    /* print prefix to string */
    if (*prefix != NULL) {
      switch (logger_control[id].prefix) {
        case LOGGER_PREFIX_UNKNOWN:
          /* empty */
          *prefix[0] = '\0';
          break;

        case LOGGER_PREFIX_EMPTY:
          /* empty */
          *prefix[0] = '\0';
          break;

        case LOGGER_PREFIX_NAME:
          (void)snprintf(*prefix, LOGGER_PREFIX_STRING_MAX - 1, "%15s: ", logger_id_name_get(id));
          break;

        case LOGGER_PREFIX_SHORT:
          (void)snprintf(*prefix, LOGGER_PREFIX_STRING_MAX - 1, "%15s:%15s: ", logger_id_name_get(id), level_str);
          break;

        case LOGGER_PREFIX_FUNCTION:
          (void)snprintf(*prefix, LOGGER_PREFIX_STRING_MAX - 1, "%15s:%15s:%30s():%5d: ", logger_id_name_get(id), level_str, function, line);
          break;

        case LOGGER_PREFIX_FILE:
          (void)snprintf(*prefix, LOGGER_PREFIX_STRING_MAX - 1, "%15s:%15s:%30s:%5d: ", logger_id_name_get(id), level_str, file, line);
          break;

        case LOGGER_PREFIX_FULL:
          (void)snprintf(*prefix, LOGGER_PREFIX_STRING_MAX - 1, "%15s:%15s:%30s:%30s():%5d: ", logger_id_name_get(id), level_str, file, function, line);
          break;

        case LOGGER_PREFIX_MAX:
          /* empty */
          *prefix[0] = '\0';
          break;
      }
      (*prefix)[LOGGER_PREFIX_STRING_MAX - 1] = '\0';
    }
    else {
      ret = LOGGER_ERR_OUT_OF_MEMORY;
    }
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Format log message.
 *
 * Allocate memory for message and print message to allocated memory.
 *
 * Memory needs to be deallocated using free() when the sting is not needed
 * anymore.
 *
 * \param[in]     id        ID outputting this message.
 * \param[out]    message   Formatted message.
 * \param[in]     format    \c printf() like format string.
 * \param[in]     va_args   Argument list.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
static inline logger_return_t __logger_format_message(logger_id_t id,
                                                      char        **message,
                                                      const char  *format,
                                                      va_list     argp)
{
  logger_return_t ret = LOGGER_OK;
  char            *message_end;

  /* allocate storage for message */
  *message = (char *)malloc(LOGGER_MESSAGE_STRING_MAX * sizeof(char));

  /* print message to string */
  if (*message != NULL) {

    /* format message */
    (void)vsnprintf(*message, LOGGER_MESSAGE_STRING_MAX - 1, format, argp);

    /* make sure message is '\0' terminated */
    (*message)[LOGGER_MESSAGE_STRING_MAX - 1] = '\0';

    /* check for multi line message */
    message_end = rindex(*message, '\n');
    if (message_end != NULL) {
      /* '\n' -> will not be continued */
      logger_control[id].cont = logger_false;

      /* remove '\n', needed for correct color display (see below) */
      *message_end = '\0';
    }
    else {
      /* no '\n' -> will be continued */
      logger_control[id].cont = logger_true;
    }
  }

  else {
    ret = LOGGER_ERR_OUT_OF_MEMORY;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  Output log messages to streams
 *
 * Print the log message to all output streams registered. It is possible to do
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
  logger_return_t ret = LOGGER_OK;
  int16_t         index;

  /* loop over all possible outputs */
  for (index = 0 ; index < size ; index++) {
    if ((outputs[index].count > 0) &&
        (outputs[index].level <= level)) {
      /* set color */
#ifdef LOGGER_COLORS
      if ((logger_control[id].color == logger_true) &&
          ((outputs[index].stream == stdout) ||
           (outputs[index].stream == stderr))) {
        (void)fprintf(outputs[index].stream, "%c[%d;%d;%dm", 0x1B, logger_control[id].attr, logger_control[id].fg, logger_control[id].bg);
      }
#endif      /* LOGGER_COLORS */

      /* actually output prefix */
      if (prefix != NULL) {
        fputs(prefix, outputs[index].stream);
      }

      /* actually output message */
      if (message != NULL) {
        fputs(message, outputs[index].stream);
      }

      /* reset color */
#ifdef LOGGER_COLORS
      if ((logger_control[id].color == logger_true) &&
          (logger_control[id].cont == logger_false) &&
          ((outputs[index].stream == stdout) ||
           (outputs[index].stream == stderr))) {
        (void)fprintf(outputs[index].stream, "%c[%dm", 0x1B, LOGGER_ATTR_RESET);
      }
#endif      /* LOGGER_COLORS */

      /* print '\n' if needed. color reset needs to be printed before '\n', otherwise some terminals show wrong colors in next line */
      if (logger_control[id].cont == logger_false) {
        fputc('\n', outputs[index].stream);
      }
    }
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
 *   - logging ID is enabled.
 *   - logging level is high enough.
 *
 * \param[in]     id        ID outputting this message.
 * \param[in]     level     Level of this message.
 * \param[in]     level_str Level of this message as string.
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
                         const char     *level_str,
                         const char     *file,
                         const char     *function,
                         uint32_t       line,
                         const char     *format,
                         ...)
{
  logger_return_t ret = LOGGER_OK;
  va_list         argp;
  char            *prefix  = NULL;
  char            *message = NULL;
  char            *message_part;
  char            *message_end;

  /* check for valid ID */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX) &&
      (logger_enabled == logger_true)) {

    /* check for valid level */
    if ((level >= LOGGER_DEBUG) &&
        (level <= LOGGER_MAX)) {

      /* check if ID is enabled and level is high enough */
      if ((logger_control[id].enabled == logger_true) &&
          (logger_control[id].level <= level)) {
        /* format prefix */
        __logger_format_prefix(id, &prefix, level, level_str, file, function, line);

        /* format message */
        va_start(argp, format);
        __logger_format_message(id, &message, format, argp);
        va_end(argp);

        /* initialize message pointer */
        message_part = message;

        /* loop over all message parts */
        do {
          /* search for the next linefeed */
          message_end = index(message_part, '\n');
          if (message_end != NULL) {
            /* replace linefeed with string end */
            *message_end = '\0';

            /* make message_end point to the next message part */
            message_end++;
          }

          /* output message to global streams */
          __logger_output(id, level, logger_outputs, LOGGER_OUTPUTS_MAX, prefix, message_part);

          /* output message to id streams */
          __logger_output(id, level, logger_control[id].outputs, LOGGER_ID_OUTPUTS_MAX, prefix, message_part);

          /* update message part for next loop */
          message_part = message_end;
        } while (message_part != NULL);

        /* release memory of prefix and message */
        if (prefix != NULL) {
          free(prefix);
        }
        if (message != NULL) {
          free(message);
        }
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
