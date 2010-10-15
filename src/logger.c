/***************************************************************************//**
 * \file   $Id$
 *
 * \brief  logging facility for C
 * \author Markus Braun
 * \date   2010-10-13
 * \note   TODO: additional notes
 ******************************************************************************/
#include <string.h>
#include <stdarg.h>
#include "logger.h"

#ifndef LOGGER_DISABLE

#define LOGGER_OUTPUTS_MAX    16 /**< number of possible simultaneous outputs */
#define LOGGER_IDS_MAX        16 /**< number of possible ids */

static FILE           *logger_outputs[LOGGER_OUTPUTS_MAX]; /**< storage for possible output streams */
static logger_bool_t  logger_ids[LOGGER_IDS_MAX];          /**< storage for possible ids */
static logger_bool_t  logger_ids_enabled[LOGGER_IDS_MAX];  /**< which ids are enabled */
static logger_level_t logger_ids_level[LOGGER_IDS_MAX];    /**< which level for which id */

/** ************************************************************************//**
 * \brief  initialize logger
 *
 * \return     LOGGER_OK if no error occurred, error code otherwise
 ******************************************************************************/
logger_return_t __logger_init(void)
{
  logger_return_t ret = LOGGER_OK;

  memset(logger_ids, 0, sizeof(logger_ids));
  memset(logger_ids_enabled, 0, sizeof(logger_ids_enabled));
  memset(logger_ids_level, LOGGER_DEBUG, sizeof(logger_ids_level));
  memset(logger_outputs, 0, sizeof(logger_outputs));

  return(ret);
}


/** ************************************************************************//**
 * \brief  register an output stream to logger
 *
 * the given file stream may be on of stdout, stderr or a file stream opened by
 * the user.
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
    /* check if this output is already registered */
    found = logger_false;
    for (index = 0 ; index < LOGGER_OUTPUTS_MAX ; index++) {
      if (logger_outputs[index] == stream) {
        found = logger_true;
        break;
      }
      else {
        ret = LOGGER_ERR_OUTPUT_REGISTERED;
      }
    }

    /* stream is not registered */
    if (found == logger_false) {
      /* search for an empty slot */
      found = logger_false;
      for (index = 0 ; index < LOGGER_OUTPUTS_MAX ; index++) {
        if (logger_outputs[index] == NULL) {
          found = logger_true;
          break;
        }
      }

      /* found an empty slot */
      if (found == logger_true) {
        logger_outputs[index] = stream;
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
    if (logger_outputs[index] == stream) {
      found = logger_true;
      break;
    }
  }

  /* found an empty slot */
  if (found == logger_true) {
    logger_outputs[index] = (FILE *)NULL;
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
    if (logger_ids[index] == logger_false) {
      found = logger_true;
      break;
    }
  }

  /* found an empty slot */
  if (found == logger_true) {
    logger_ids[index] = logger_true;
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
    if (logger_ids[id] == logger_true) {
      /* reset all id dependend values to defaults */
      logger_ids[id]         = logger_false;
      logger_ids_enabled[id] = logger_false;
      logger_ids_level[id]   = LOGGER_DEBUG;
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
    logger_ids_enabled[id] = logger_true;
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
    logger_ids_enabled[id] = logger_false;
  }
  else {
    ret = LOGGER_ERR_ID_UNKNOWN;
  }

  return(ret);
}


/** ************************************************************************//**
 * \brief  disable a logger id for output
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
    ret = logger_ids_enabled[id];
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
      logger_ids_level[id] = level;
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
    ret = logger_ids_level[id];
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
logger_return_t __logger(logger_id_t    id,
                         logger_level_t level,
                         const char     *format,
                         ...)
{
  logger_return_t ret = LOGGER_OK;
  va_list         argp;
  int             index;

  /* check for valid id */
  if ((id >= 0) &&
      (id < LOGGER_IDS_MAX)) {
    /* check for valid level */
    if ((level >= LOGGER_DEBUG) &&
        (level <= LOGGER_MAX)) {
      /* check if id is enabled and level is high enough */
      if ((logger_ids_enabled[id] == logger_true) &&
          (logger_ids_level[id] <= level)) {
        va_start(argp, format);

        /* loop over all possibe outputs */
        for (index = 0 ; index < LOGGER_OUTPUTS_MAX ; index++) {
          if (logger_outputs[index] != NULL) {
            (void)vfprintf(logger_outputs[index], format, argp);
          }
        }
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


#endif /* LOGGER_DISABLE */
