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
#include <unistd.h>
#include <stddef.h>
#include <time.h>
#include <inttypes.h>

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
#define LOGGER_COLOR_STRING_MAX        (128)
#endif /* LOGGER_COLOR_STRING_MAX */
#if (LOGGER_COLOR_STRING_MAX < 1)
#error "LOGGER_COLOR_STRING_MAX must be at least 1"
#endif /* (LOGGER_COLOR_STRING_MAX < 1) */

/** Length of logger date string including '\0' */
#ifndef LOGGER_DATE_STRING_MAX
#define LOGGER_DATE_STRING_MAX       (32)
#endif /* LOGGER_DATE_STRING_MAX */
#if (LOGGER_DATE_STRING_MAX < 4)
#error "LOGGER_DATE_STRING_MAX must be at least 4"
#endif /* (LOGGER_DATE_STRING_MAX < 4) */

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
#define LOGGER_PREFIX_STANDARD       (LOGGER_PFX_NAME | LOGGER_PFX_LEVEL | LOGGER_PFX_FUNCTION | LOGGER_PFX_LINE)
#endif /* LOGGER_PREFIX_STANDARD */

/** Number of unified outputs */
#define LOGGER_UNIFIED_OUTPUTS_MAX   (LOGGER_OUTPUTS_MAX + LOGGER_ID_OUTPUTS_MAX)

/** Format string for strftime */
#define LOGGER_FORMAT_STRFTIME         "%Y-%m-%d %H:%M:%S"

/** Format string for prefix date string */
#define LOGGER_FORMAT_STRING_DATE_WIDTH (19)

/** Format string for prefix file name */
#define LOGGER_FORMAT_STRING_FILE_WIDTH (30)

/** Format string for prefix function name */
#define LOGGER_FORMAT_STRING_FUNCTION_WIDTH (30)

/** Format string for prefix line number */
#define LOGGER_FORMAT_STRING_LINE_WIDTH (5)

/** Format string for prefix logger id name */
#define LOGGER_FORMAT_STRING_NAME_WIDTH (15)

/** Format string for prefix logger id level name */
#define LOGGER_FORMAT_STRING_LEVEL_WIDTH (7)

/** Format string for field separator (one character!) */
#define LOGGER_FORMAT_STRING_SEPARATOR ':'

/** Reset attribute string. */
#define LOGGER_ATTR_STRING_RESET     "0"

/** Bright attribute string */
#define LOGGER_ATTR_STRING_BRIGHT    "1"

/** Dim attribute string */
#define LOGGER_ATTR_STRING_DIM       "2"

/** Underline attribute string */
#define LOGGER_ATTR_STRING_UNDERLINE "3"

/** Blink attribute string */
#define LOGGER_ATTR_STRING_BLINK     "5"

/** Reverse attribute string */
#define LOGGER_ATTR_STRING_REVERSE   "7"

/** Hidden attribute string */
#define LOGGER_ATTR_STRING_HIDDEN    "8"

/** Helper for number of elements in an array */
#define LOGGER_ELEMENTS(x)  (sizeof(x) / sizeof(x[0]))

/** Helper for determine the minimum of to numbers */
#define LOGGER_NUMERIC_MIN(a, b) ((a) < (b) ? (a) : (b))

/** Helper for determine the maximum of to numbers */
#define LOGGER_NUMERIC_MAX(a, b) ((a) > (b) ? (a) : (b))

/** Helper to limit a number to a given range */
#define LOGGER_NUMERIC_LIMIT(a, lower, upper) (LOGGER_NUMERIC_MIN(LOGGER_NUMERIC_MAX((a), (lower)), (upper)))

/** Helper to convert a number to its absolute value */
#define LOGGER_NUMERIC_ABS(a) ((a) < 0 ? (-a) : (a))

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
typedef struct logger_control_s {
  logger_bool_t         used;                                        /**< This ID is used. */
  int16_t               count;                                       /**< Number of registrations for this ID. */
  logger_bool_t         enabled;                                     /**< This ID is enabled. */
  logger_level_t        level;                                       /**< Level for this ID. */
  logger_prefix_t       prefix;                                      /**< Prefix for this ID. */
  logger_bool_t         color;                                       /**< Changed colors for this ID. */
  logger_color_string_t color_string;                                /**< Color string for this ID. */
  logger_bool_t         color_string_changed;                        /**< Color string had been changed for this ID. */
  logger_bool_t         append;                                      /**< Previous message didn't contain a newline just append next message */
  char                  name[LOGGER_NAME_MAX];                       /**< Name of this logger ID. */
  logger_output_t       outputs[LOGGER_ID_OUTPUTS_MAX];              /**< Storage for possible ID outputs. */
  logger_output_t       unified_outputs[LOGGER_UNIFIED_OUTPUTS_MAX]; /**< Storage for unified global and ID specific outputs. */
} logger_control_t;


/** Logger repeated message structure */
typedef struct logger_repeat_s {
  uint16_t       count;                              /**< Number of repeats for previous message */
  logger_id_t    id;                                 /**< ID of repeated message */
  logger_level_t level;                              /**< Level of repeated message */
  uint16_t       prefix_length;                      /**< Length of prefix string of repeated message */
  char           message[LOGGER_MESSAGE_STRING_MAX]; /**< Storage for repeated message string */
} logger_repeat_t;


/** logger rate limit structure */
typedef struct logger_limit_s {
  logger_rate_limit_t limit;           /**< Message rate limit (messages per second) */
  logger_rate_limit_t counter;         /**< Message counter (messages this second) */
  logger_bool_t       exceeded;        /**< Flag: Number of messages per second is exceeded */
  logger_bool_t       message_request; /**< Flag: Output of message is requested */
  logger_rate_limit_t skipped;         /**< Number of skipped messages */
} logger_limit_t;


static logger_bool_t     logger_initialized           = logger_false;           /**< Logger is initialized. */
static logger_bool_t     logger_enabled               = logger_false;           /**< Logger is enabled. */
static logger_prefix_t   logger_prefix_standard       = LOGGER_PREFIX_STANDARD; /**< Logger standard prefix */
static logger_bool_t     logger_color_prefix_enabled  = logger_false;           /**< Logger prefix color is enabled. */
static logger_bool_t     logger_color_message_enabled = logger_false;           /**< Logger message color is enabled. */
static logger_control_t  logger_control[LOGGER_IDS_MAX + 1];                    /**< Control storage for possible IDs plus system ID. */
static logger_output_t   logger_outputs[LOGGER_OUTPUTS_MAX];                    /**< Storage for possible outputs. */
static logger_repeat_t   logger_repeat;                                         /**< Storage for repeated message information. */
static logger_limit_t    logger_limit;                                          /**< Storage for rate limit information. */
static char              logger_date[LOGGER_DATE_STRING_MAX];                   /**< Storage for date string */
static char              logger_prefix[LOGGER_PREFIX_STRING_MAX];               /**< Storage for prefix string */
static char              logger_message[LOGGER_MESSAGE_STRING_MAX];             /**< Storage for message string */
static char              logger_line[LOGGER_LINE_STRING_MAX];                   /**< Storage for whole output line */
static const logger_id_t logger_system_id = 0;                                  /**< System logging ID */

/** level to name translation */
static const char *logger_level_names[LOGGER_MAX] =
{
  "UNKNOWN", /**< Name for level "UNKNOWN" */
  "DEBUG",   /**< Name for level "DEBUG"   */
  "INFO",    /**< Name for level "INFO"    */
  "NOTICE",  /**< Name for level "NOTICE"  */
  "WARNING", /**< Name for level "WARNING" */
  "ERR",     /**< Name for level "ERR"     */
  "CRIT",    /**< Name for level "CRIT"    */
  "ALERT",   /**< Name for level "ALERT"   */
  "EMERG"    /**< Name for level "EMERG"   */
};


/** level to color translation */
static logger_color_string_t logger_level_colors[LOGGER_MAX];


/** level to color translation for console */
static logger_color_string_t logger_level_colors_console[LOGGER_MAX] =
{
  { "\x1B[0;37;40m", "\x1B[0m" }, /* Prefix color string for level "UNKNOWN" -> LOGGER_BG_BLACK,   LOGGER_FG_WHITE, LOGGER_ATTR_RESET */
  { "\x1B[0;37;40m", "\x1B[0m" }, /* Prefix color string for level "DEBUG"   -> LOGGER_BG_BLACK,   LOGGER_FG_WHITE, LOGGER_ATTR_RESET */
  { "\x1B[0;30;47m", "\x1B[0m" }, /* Prefix color string for level "INFO"    -> LOGGER_BG_WHITE,   LOGGER_FG_BLACK, LOGGER_ATTR_RESET */
  { "\x1B[0;30;44m", "\x1B[0m" }, /* Prefix color string for level "NOTICE"  -> LOGGER_BG_BLUE,    LOGGER_FG_BLACK, LOGGER_ATTR_RESET */
  { "\x1B[0;30;46m", "\x1B[0m" }, /* Prefix color string for level "WARNING" -> LOGGER_BG_CYAN,    LOGGER_FG_BLACK, LOGGER_ATTR_RESET */
  { "\x1B[0;30;42m", "\x1B[0m" }, /* Prefix color string for level "ERR"     -> LOGGER_BG_GREEN,   LOGGER_FG_BLACK, LOGGER_ATTR_RESET */
  { "\x1B[0;30;43m", "\x1B[0m" }, /* Prefix color string for level "CRIT"    -> LOGGER_BG_YELLOW,  LOGGER_FG_BLACK, LOGGER_ATTR_RESET */
  { "\x1B[0;30;45m", "\x1B[0m" }, /* Prefix color string for level "ALERT"   -> LOGGER_BG_MAGENTA, LOGGER_FG_BLACK, LOGGER_ATTR_RESET */
  { "\x1B[0;30;41m", "\x1B[0m" }  /* Prefix color string for level "EMERG"   -> LOGGER_BG_RED,     LOGGER_FG_BLACK, LOGGER_ATTR_RESET */
};

static char logger_int_to_char[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

/* declarations */
LOGGER_INLINE void logger_repeat_message(void);
LOGGER_INLINE void logger_rate_limit_message(logger_bool_t force);
LOGGER_INLINE size_t logger_string_copy(char *dest, const char *src, size_t n);
LOGGER_INLINE size_t logger_prefix_concatenate(char *dest, const char *dest_end, const char *src, size_t width);


/***************************************************************************//**
 * \brief  Format escape sequence for unix consoles.
 *
 * Create a string containing a escape sequence constructed from given
 * foreground and background color as well as attributes. Attributes can be
 * combined using `|`.
 *
 * \param[out]    string  String for escape sequence.
 * \param[in]     size    Size of string.
 * \param[in]     fg      Text foreground.
 * \param[in]     bg      Text background.
 * \param[in]     attr    Text attribute.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
static logger_return_t logger_color_console_format(char                     *string,
                                                   const size_t             size,
                                                   const logger_text_fg_t   fg,
                                                   const logger_text_bg_t   bg,
                                                   const logger_text_attr_t attr)
{
  int           used      = 0;
  logger_bool_t separator = logger_false;

  /* GUARD: check for valid string */
  if (string == NULL) {
    return(LOGGER_ERR_STRING_INVALID);
  }

  /* escape sequence start */
  used += snprintf(string + used, size - used, "\x1B[");

  /* attribute */
  if (attr & LOGGER_ATTR_RESET) {
    if (separator == logger_true) {
      used += snprintf(string + used, size - used, ";");
    }
    used     += snprintf(string + used, size - used, LOGGER_ATTR_STRING_RESET);
    separator = logger_true;
  }

  if (attr & LOGGER_ATTR_BRIGHT) {
    if (separator == logger_true) {
      used += snprintf(string + used, size - used, ";");
    }
    used     += snprintf(string + used, size - used, LOGGER_ATTR_STRING_BRIGHT);
    separator = logger_true;
  }

  if (attr & LOGGER_ATTR_DIM) {
    if (separator == logger_true) {
      used += snprintf(string + used, size - used, ";");
    }
    used     += snprintf(string + used, size - used, LOGGER_ATTR_STRING_DIM);
    separator = logger_true;
  }

  if (attr & LOGGER_ATTR_UNDERLINE) {
    if (separator == logger_true) {
      used += snprintf(string + used, size - used, ";");
    }
    used     += snprintf(string + used, size - used, LOGGER_ATTR_STRING_UNDERLINE);
    separator = logger_true;
  }

  if (attr & LOGGER_ATTR_BLINK) {
    if (separator == logger_true) {
      used += snprintf(string + used, size - used, ";");
    }
    used     += snprintf(string + used, size - used, LOGGER_ATTR_STRING_BLINK);
    separator = logger_true;
  }

  if (attr & LOGGER_ATTR_REVERSE) {
    if (separator == logger_true) {
      used += snprintf(string + used, size - used, ";");
    }
    used     += snprintf(string + used, size - used, LOGGER_ATTR_STRING_REVERSE);
    separator = logger_true;
  }

  if (attr & LOGGER_ATTR_HIDDEN) {
    if (separator == logger_true) {
      used += snprintf(string + used, size - used, ";");
    }
    used     += snprintf(string + used, size - used, LOGGER_ATTR_STRING_HIDDEN);
    separator = logger_true;
  }

  /* foreground color */
  if (fg != LOGGER_FG_UNCHANGED) {
    if (separator == logger_true) {
      used += snprintf(string + used, size - used, ";");
    }
    used     += snprintf(string + used, size - used, "%d", fg);
    separator = logger_true;
  }

  /* background color */
  if (bg != LOGGER_BG_UNCHANGED) {
    if (separator == logger_true) {
      used += snprintf(string + used, size - used, ";");
    }
    used     += snprintf(string + used, size - used, "%d", bg);
    separator = logger_true;
  }

  /* escape sequence end */
  used += snprintf(string + used, size - used, "m");

  string[size - 1] = '\0';

  return(LOGGER_OK);
}


/***************************************************************************//**
 * \brief  Version of logger.
 *
 * Get the version of logger for which it was compiled. Needed to check for
 * correct version of logger when linking against a precompiled library.
 *
 * \return        Version of logger.
 ******************************************************************************/
logger_version_t logger_version(void)
{
  return(LOGGER_VERSION);
}


/***************************************************************************//**
 * \brief  Initialize logger.
 *
 * Initialize logger internal structures. Needs to be called once before any
 * other logger function is called.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_init(void)
{
  if (logger_initialized == logger_false) {
    logger_initialized           = logger_true;
    logger_enabled               = logger_true;
    logger_color_prefix_enabled  = logger_false;
    logger_color_message_enabled = logger_false;
    (void)memcpy(logger_level_colors, logger_level_colors_console, sizeof(logger_level_colors));
    (void)memset(logger_control, 0, sizeof(logger_control));
    (void)memset(logger_outputs, 0, sizeof(logger_outputs));
    (void)memset(logger_message, 0, sizeof(logger_message));
    (void)memset(logger_prefix,  0, sizeof(logger_prefix));

    /* initialize limit variables */
    logger_limit.limit           = 0;
    logger_limit.counter         = 0;
    logger_limit.exceeded        = logger_false;
    logger_limit.message_request = logger_false;
    logger_limit.skipped         = 0;

    /* initialize logger system ID */
    logger_control[logger_system_id].used    = logger_true;
    logger_control[logger_system_id].count   = 1;
    logger_control[logger_system_id].enabled = logger_true;
    logger_control[logger_system_id].level   = LOGGER_ALL;
    logger_control[logger_system_id].prefix  = LOGGER_PREFIX_STANDARD;
    logger_control[logger_system_id].color   = logger_false;
    (void)strncpy(logger_control[logger_system_id].name, "LOGGER_SYSTEM", LOGGER_ELEMENTS(logger_control[logger_system_id].name));
  }

  return(LOGGER_OK);
}


/***************************************************************************//**
 * \brief  Query the current initialization state
 *
 * Query the current initialization state of logger.
 *
 * \return        \c logger_true if logger is initialized, logger_false otherwise.
 ******************************************************************************/
logger_bool_t logger_is_initialized(void)
{
  return(logger_initialized);
}


/***************************************************************************//**
 * \brief  Enable logger.
 *
 * Enable logger globally for all IDs and all outputs.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_enable(void)
{
  logger_enabled = logger_true;

  return(LOGGER_OK);
}


/***************************************************************************//**
 * \brief  Disable logger.
 *
 * Disable logger globally for all IDs and all outputs.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_disable(void)
{
  /* outputs will change, print repeat message */
  logger_repeat_message();

  /* outputs will change, print rate limit message */
  logger_rate_limit_message(logger_true);

  logger_enabled = logger_false;

  return(LOGGER_OK);
}


/***************************************************************************//**
 * \brief  Query the current enable state of logger.
 *
 * Query the current global enable state of logger.
 *
 * \return        \c logger_true if logger is enabled, logger_false otherwise.
 ******************************************************************************/
logger_bool_t logger_is_enabled()
{
  return(logger_enabled);
}


/***************************************************************************//**
 * \brief  Set global logging prefix.
 *
 * Set the global default prefix. This prefix is used for all IDs without
 * specific prefix.
 *
 * \param[in]     prefix  Prefix to set.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_prefix_set(const logger_prefix_t prefix)
{
  /* GUARD: check for valid prefix */
  if ((prefix < LOGGER_PFX_EMPTY) ||
      (prefix > LOGGER_PFX_UNSET)) {
    return(LOGGER_ERR_PREFIX_UNKNOWN);
  }

  /* set prefix */
  if (prefix & LOGGER_PFX_UNSET) {
    logger_prefix_standard = LOGGER_PREFIX_STANDARD;
  }
  else {
    logger_prefix_standard = prefix;
  }

  return(LOGGER_OK);
}


/***************************************************************************//**
 * \brief  Query global logging prefix,
 *
 * Query the currently set prefix for the given logging ID.
 *
 * \return        Currently set prefix.
 ******************************************************************************/
logger_prefix_t logger_prefix_get(void)
{
  /* get global prefix */
  return(logger_prefix_standard);
}


/***************************************************************************//**
 * \brief  Set message rate limit
 *
 * Set the the limit of messages per second that will be printed to outputs.
 * specific prefix. A value of `0` disables rate limiting.
 *
 * \param[in]     limit   Limit to set.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_rate_limit_set(const logger_rate_limit_t limit)
{
  logger_limit.limit = limit;

  return(LOGGER_OK);
}


/***************************************************************************//**
 * \brief  Query message rate limit
 *
 * Query the currently set limit of messages per second.
 *
 * \return        Currently set limit.
 ******************************************************************************/
logger_rate_limit_t logger_rate_limit_get(void)
{
  return(logger_limit.limit);
}


/***************************************************************************//**
 * \brief  Unify outputs for IDs
 *
 * Merge all possible outputs of all IDs into one list. During merge make every
 * output unique.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_output_common_unify(void)
{
  size_t id_index;
  size_t unified_index;
  size_t global_index;
  size_t specific_index;

  /* loop over all IDs */
  for (id_index = 0 ; id_index < LOGGER_ELEMENTS(logger_control) ; id_index++) {
    if (logger_control[id_index].used == logger_true) {
      /* reset list of unified outputs */
      memset(logger_control[id_index].unified_outputs, 0, sizeof(logger_control[id_index].unified_outputs));

      /* loop over all global outputs */
      for (global_index = 0 ; global_index < LOGGER_ELEMENTS(logger_outputs) ; global_index++)
      {
        /* if global output is valid */
        if (logger_outputs[global_index].count > 0) {
          /* search this global output in unified outputs */
          for (unified_index = 0 ; unified_index < LOGGER_ELEMENTS(logger_control[id_index].unified_outputs) ; unified_index++) {
            /* if this unified entry is used */
            if (logger_control[id_index].unified_outputs[unified_index].count != 0) {
              /* if it is the same then current global output has been added already*/
              if (((logger_outputs[global_index].type == LOGGER_OUTPUT_TYPE_FILESTREAM) &&
                   (logger_control[id_index].unified_outputs[unified_index].type == LOGGER_OUTPUT_TYPE_FILESTREAM) &&
                   (logger_control[id_index].unified_outputs[unified_index].stream == logger_outputs[global_index].stream)) ||
                  ((logger_outputs[global_index].type == LOGGER_OUTPUT_TYPE_FUNCTION) &&
                   (logger_control[id_index].unified_outputs[unified_index].type == LOGGER_OUTPUT_TYPE_FUNCTION) &&
                   (logger_control[id_index].unified_outputs[unified_index].function == logger_outputs[global_index].function))) {
                logger_control[id_index].unified_outputs[unified_index].use_color |= logger_outputs[global_index].use_color;
                logger_control[id_index].unified_outputs[unified_index].level     |= logger_outputs[global_index].level;
                break;
              }
            }
            else {
              /* if this unified entry is not used the global output has not been added to unified list */
              logger_control[id_index].unified_outputs[unified_index]       = logger_outputs[global_index];
              logger_control[id_index].unified_outputs[unified_index].count = 1;
              break;
            }
          }
        }
      }

      /* loop over all ID specific outputs */
      for (specific_index = 0 ; specific_index < LOGGER_ELEMENTS(logger_control[id_index].outputs) ; specific_index++)
      {
        /* if ID specific output is valid */
        if (logger_control[id_index].outputs[specific_index].count > 0) {
          /* search this ID specific output in unified outputs */
          for (unified_index = 0 ; unified_index < LOGGER_ELEMENTS(logger_control[id_index].unified_outputs) ; unified_index++) {
            /* if this unified entry is used */
            if (logger_control[id_index].unified_outputs[unified_index].count != 0) {
              /* if it is the same then current ID specific output has been added already*/
              if (((logger_control[id_index].outputs[specific_index].type == LOGGER_OUTPUT_TYPE_FILESTREAM) &&
                   (logger_control[id_index].unified_outputs[unified_index].type == LOGGER_OUTPUT_TYPE_FILESTREAM) &&
                   (logger_control[id_index].unified_outputs[unified_index].stream == logger_control[id_index].outputs[specific_index].stream)) ||
                  ((logger_control[id_index].outputs[specific_index].type == LOGGER_OUTPUT_TYPE_FUNCTION) &&
                   (logger_control[id_index].unified_outputs[unified_index].type == LOGGER_OUTPUT_TYPE_FUNCTION) &&
                   (logger_control[id_index].unified_outputs[unified_index].function == logger_control[id_index].outputs[specific_index].function))) {
                logger_control[id_index].unified_outputs[unified_index].use_color |= logger_control[id_index].outputs[specific_index].use_color;
                logger_control[id_index].unified_outputs[unified_index].level     |= logger_control[id_index].outputs[specific_index].level;
                break;
              }
            }
            else {
              /* if this unified entry is not used the ID specific output has not been added to unified list */
              logger_control[id_index].unified_outputs[unified_index]       = logger_control[id_index].outputs[specific_index];
              logger_control[id_index].unified_outputs[unified_index].count = 1;
              break;
            }
          }
        }
      }
    }
  }

  return(LOGGER_OK);
}


/***************************************************************************//**
 * \brief  Register an output to a list of outputs.
 *
 * The given file stream may be on of \c stdout, \c stderr or a file stream
 * opened by the user. The user provided function must take a single string
 * argument. The default logging level is set to \c LOGGER_UNKNOWN thus no
 * messages will appear on this stream.
 *
 * \param[in,out] outputs   List of logger outputs.
 * \param[in]     size      Number of elements in list.
 * \param[in]     type      Type of output.
 * \param[in]     stream    Opened file stream.
 * \param[in]     function  User provided output function.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
LOGGER_INLINE logger_return_t logger_output_common_register(logger_output_t          *outputs,
                                                            const size_t             size,
                                                            logger_output_type_t     type,
                                                            FILE                     *stream,
                                                            logger_output_function_t function)
{
  logger_return_t ret = LOGGER_OK;
  size_t          index;
  logger_bool_t   found;

  /* GUARD: check for valid type */
  if ((type <= LOGGER_OUTPUT_TYPE_UNKNOWN) ||
      (type >= LOGGER_OUTPUT_TYPE_MAX)) {
    return(LOGGER_ERR_TYPE_INVALID);
  }

  /* GUARD: check for valid stream */
  if ((type == LOGGER_OUTPUT_TYPE_FILESTREAM) &&
      (stream == NULL)) {
    return(LOGGER_ERR_STREAM_INVALID);
  }

  /* GUARD: check for valid function */
  if ((type == LOGGER_OUTPUT_TYPE_FUNCTION) &&
      (function == NULL)) {
    return(LOGGER_ERR_FUNCTION_INVALID);
  }

  /* GUARD: check for valid output */
  if (outputs == NULL) {
    return(LOGGER_ERR_OUTPUT_INVALID);
  }

  /* check if this output stream is already registered */
  found = logger_false;
  for (index = 0 ; index < size ; index++) {
    if (((outputs[index].type == LOGGER_OUTPUT_TYPE_FILESTREAM) &&
        (outputs[index].stream == stream)) ||
        ((outputs[index].type == LOGGER_OUTPUT_TYPE_FUNCTION) &&
         (outputs[index].function == function))){
      found = logger_true;
      break;
    }
  }

  /* output is already registered */
  if (found == logger_true) {
    outputs[index].count++;
    ret = LOGGER_ERR_OUTPUT_REGISTERED;
  }
  /* output is not registered */
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
      /* outputs will change, print repeat message */
      logger_repeat_message();

      /* outputs will change, print rate limit message */
      logger_rate_limit_message(logger_true);

      outputs[index].count++;
      outputs[index].level     = LOGGER_UNKNOWN;
      outputs[index].use_color = logger_false;
      outputs[index].type      = type;

      switch (type) {
        case LOGGER_OUTPUT_TYPE_UNKNOWN:
          /* nothing */
          break;

        case LOGGER_OUTPUT_TYPE_FILESTREAM:
          outputs[index].stream = stream;

          /* only stdout and stderr use color by default */
          if ((stream == stdout) ||
              (stream == stderr)) {
            outputs[index].use_color = logger_true;
          }
          break;

        case LOGGER_OUTPUT_TYPE_FUNCTION:
          outputs[index].function = function;
          break;

        case LOGGER_OUTPUT_TYPE_MAX:
          /* nothing */
          break;
      }
    }
    else {
      ret = LOGGER_ERR_OUTPUTS_FULL;
    }
  }

  return(ret);
}


/***************************************************************************//**
 * \brief  Deregister an output from a list of outputs.
 *
 * Remove given output from list of outputs.
 *
 * \param[in,out] outputs   List of logger outputs.
 * \param[in]     size      Number of elements in list.
 * \param[in]     type      Type of output.
 * \param[in]     stream    Opened file stream.
 * \param[in]     function  User provided output function.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
LOGGER_INLINE logger_return_t logger_output_common_deregister(logger_output_t          *outputs,
                                                              const size_t             size,
                                                              logger_output_type_t     type,
                                                              FILE                     *stream,
                                                              logger_output_function_t function)
{
  logger_return_t ret = LOGGER_OK;
  size_t          index;
  logger_bool_t   found;

  /* GUARD: check for valid type */
  if ((type <= LOGGER_OUTPUT_TYPE_UNKNOWN) ||
      (type >= LOGGER_OUTPUT_TYPE_MAX)) {
    return(LOGGER_ERR_TYPE_INVALID);
  }

  /* GUARD: check for valid stream */
  if ((type == LOGGER_OUTPUT_TYPE_FILESTREAM) &&
      (stream == NULL)) {
    return(LOGGER_ERR_STREAM_INVALID);
  }

  /* GUARD: check for valid function */
  if ((type == LOGGER_OUTPUT_TYPE_FUNCTION) &&
      (function == NULL)) {
    return(LOGGER_ERR_FUNCTION_INVALID);
  }

  /* GUARD: check for valid output */
  if (outputs == NULL) {
    return(LOGGER_ERR_OUTPUT_INVALID);
  }

  /* check if this output is registered */
  found = logger_false;
  for (index = 0 ; index < size ; index++) {
    if (((outputs[index].type == LOGGER_OUTPUT_TYPE_FILESTREAM) &&
        (outputs[index].stream == stream)) ||
        ((outputs[index].type == LOGGER_OUTPUT_TYPE_FUNCTION) &&
         (outputs[index].function == function))){
      found = logger_true;
      break;
    }
  }

  /* found given output in a slot */
  if (found == logger_true) {
    outputs[index].count--;

    /* remove this output if this was the last reference */
    if (outputs[index].count <= 0) {
      /* outputs will change, print repeat message */
      logger_repeat_message();

      /* outputs will change, print rate limit message */
      logger_rate_limit_message(logger_true);

      /* flush everything in this stream */
      if (type == LOGGER_OUTPUT_TYPE_FILESTREAM) {
        fflush(outputs[index].stream);
      }

      /* reset output to default values */
      outputs[index].count     = 0;
      outputs[index].level     = LOGGER_UNKNOWN;
      outputs[index].use_color = logger_false;
      outputs[index].type      = LOGGER_OUTPUT_TYPE_UNKNOWN;
      outputs[index].stream    = (FILE *)NULL;
      outputs[index].function  = (logger_output_function_t)NULL;
    }
  }
  else {
    ret = LOGGER_ERR_OUTPUT_NOT_FOUND;
  }

  return(ret);
}


/***************************************************************************//**
 * \brief  Search an output in a list of outputs.
 *
 * The given output is searched in the given list of outputs.
 *
 * \param[in,out] outputs   List of logger outputs.
 * \param[in]     size      Number of elements in list.
 * \param[in]     type      Type of output.
 * \param[in]     stream    Opened file stream.
 * \param[in]     function  User provided output function.
 *
 * \return        \c logger_true if logger is found, logger_false otherwise.
 ******************************************************************************/
LOGGER_INLINE logger_bool_t logger_output_common_is_registered(logger_output_t          *outputs,
                                                               const size_t             size,
                                                               logger_output_type_t     type,
                                                               FILE                     *stream,
                                                               logger_output_function_t function)
{
  logger_bool_t ret = logger_false;
  size_t        index;

  /* GUARD: check for valid type */
  if ((type <= LOGGER_OUTPUT_TYPE_UNKNOWN) ||
      (type >= LOGGER_OUTPUT_TYPE_MAX)) {
    return(LOGGER_ERR_TYPE_INVALID);
  }

  /* GUARD: check for valid stream */
  if ((type == LOGGER_OUTPUT_TYPE_FILESTREAM) &&
      (stream == NULL)) {
    return(LOGGER_ERR_STREAM_INVALID);
  }

  /* GUARD: check for valid function */
  if ((type == LOGGER_OUTPUT_TYPE_FUNCTION) &&
      (function == NULL)) {
    return(LOGGER_ERR_FUNCTION_INVALID);
  }

  /* GUARD: check for valid output */
  if (outputs == NULL) {
    return(logger_false);
  }

  /* check if this output stream is registered */
  for (index = 0 ; index < size ; index++) {
    if (((outputs[index].type == LOGGER_OUTPUT_TYPE_FILESTREAM) &&
        (outputs[index].stream == stream)) ||
        ((outputs[index].type == LOGGER_OUTPUT_TYPE_FUNCTION) &&
         (outputs[index].function == function))){
      ret = logger_true;
      break;
    }
  }

  return(ret);
}


/***************************************************************************//**
 * \brief  Set logging level for output in a list of outputs.
 *
 * Set the minimum logging level for given output. Only log messages
 * equal or above the given level will be printed to the given output.
 *
 * \param[in,out] outputs   List of logger outputs.
 * \param[in]     size      Number of elements in list.
 * \param[in]     type      Type of output.
 * \param[in]     stream    Opened file stream.
 * \param[in]     function  User provided output function.
 * \param[in]     level     Level to set.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
LOGGER_INLINE logger_return_t logger_output_common_level_set(logger_output_t          *outputs,
                                                             const size_t             size,
                                                             logger_output_type_t     type,
                                                             FILE                     *stream,
                                                             logger_output_function_t function,
                                                             const logger_level_t     level)
{
  logger_return_t ret = LOGGER_OK;
  size_t          index;
  logger_bool_t   found;

  /* GUARD: check for valid type */
  if ((type <= LOGGER_OUTPUT_TYPE_UNKNOWN) ||
      (type >= LOGGER_OUTPUT_TYPE_MAX)) {
    return(LOGGER_ERR_TYPE_INVALID);
  }

  /* GUARD: check for valid stream */
  if ((type == LOGGER_OUTPUT_TYPE_FILESTREAM) &&
      (stream == NULL)) {
    return(LOGGER_ERR_STREAM_INVALID);
  }

  /* GUARD: check for valid function */
  if ((type == LOGGER_OUTPUT_TYPE_FUNCTION) &&
      (function == NULL)) {
    return(LOGGER_ERR_FUNCTION_INVALID);
  }

  /* GUARD: check for valid output */
  if (outputs == NULL) {
    return(LOGGER_ERR_OUTPUT_INVALID);
  }

  /* check if this output is registered */
  found = logger_false;
  for (index = 0 ; index < size ; index++) {
    if (((outputs[index].type == LOGGER_OUTPUT_TYPE_FILESTREAM) &&
        (outputs[index].stream == stream)) ||
        ((outputs[index].type == LOGGER_OUTPUT_TYPE_FUNCTION) &&
         (outputs[index].function == function))){
      found = logger_true;
      break;
    }
  }

  /* found given output in a slot */
  if (found == logger_true) {
    /* outputs will change, print repeat message */
    logger_repeat_message();

    /* outputs will change, print rate limit message */
    logger_rate_limit_message(logger_true);

    /* set log level */
    outputs[index].level = level;
  }
  else {
    ret = LOGGER_ERR_OUTPUT_NOT_FOUND;
  }

  /* update unified output lists of each ID */
  (void)logger_output_common_unify();

  return(ret);
}


/***************************************************************************//**
 * \brief  Query logging level for output stream in a list of outputs.
 *
 * Query the currently set minimum level for the given logging output stream.
 *
 * \param[in,out] outputs   List of logger outputs.
 * \param[in]     size      Number of elements in list.
 * \param[in]     type      Type of output.
 * \param[in]     stream    Opened file stream.
 * \param[in]     function  User provided output function.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
LOGGER_INLINE logger_level_t logger_output_common_level_get(logger_output_t          *outputs,
                                                            const size_t             size,
                                                            logger_output_type_t     type,
                                                            FILE                     *stream,
                                                            logger_output_function_t function)
{
  logger_level_t ret = LOGGER_UNKNOWN;
  size_t         index;

  /* GUARD: check for valid type */
  if ((type <= LOGGER_OUTPUT_TYPE_UNKNOWN) ||
      (type >= LOGGER_OUTPUT_TYPE_MAX)) {
    return(LOGGER_UNKNOWN);
  }

  /* GUARD: check for valid stream */
  if ((type == LOGGER_OUTPUT_TYPE_FILESTREAM) &&
      (stream == NULL)) {
    return(LOGGER_UNKNOWN);
  }

  /* GUARD: check for valid function */
  if ((type == LOGGER_OUTPUT_TYPE_FUNCTION) &&
      (function == NULL)) {
    return(LOGGER_UNKNOWN);
  }

  /* GUARD: check for valid output */
  if (outputs == NULL) {
    return(LOGGER_UNKNOWN);
  }

  /* check if this output is registered */
  for (index = 0 ; index < size ; index++) {
    if (((outputs[index].type == LOGGER_OUTPUT_TYPE_FILESTREAM) &&
         (outputs[index].stream == stream)) ||
        ((outputs[index].type == LOGGER_OUTPUT_TYPE_FUNCTION) &&
         (outputs[index].function == function))) {
      ret = outputs[index].level;
      break;
    }
  }

  return(ret);
}


/***************************************************************************//**
 * \brief  Modify the output color setting.
 *
 * Set the color setting for a given output to the given state.
 *
 * \param[in,out] outputs   List of logger outputs.
 * \param[in]     size      Number of elements in list.
 * \param[in]     type      Type of output.
 * \param[in]     stream    Opened file stream.
 * \param[in]     function  User provided output function.
 * \param[in]     flag      State of color setting.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
LOGGER_INLINE logger_return_t logger_output_common_color(logger_output_t          *outputs,
                                                         const size_t             size,
                                                         logger_output_type_t     type,
                                                         FILE                     *stream,
                                                         logger_output_function_t function,
                                                         logger_bool_t            flag)
{
  logger_return_t ret = LOGGER_OK;
  size_t          index;

  /* GUARD: check for valid type */
  if ((type <= LOGGER_OUTPUT_TYPE_UNKNOWN) ||
      (type >= LOGGER_OUTPUT_TYPE_MAX)) {
    return(LOGGER_ERR_TYPE_INVALID);
  }

  /* GUARD: check for valid stream */
  if ((type == LOGGER_OUTPUT_TYPE_FILESTREAM) &&
      (stream == NULL)) {
    return(LOGGER_ERR_STREAM_INVALID);
  }

  /* GUARD: check for valid function */
  if ((type == LOGGER_OUTPUT_TYPE_FUNCTION) &&
      (function == NULL)) {
    return(LOGGER_ERR_FUNCTION_INVALID);
  }

  /* GUARD: check for valid output */
  if (outputs == NULL) {
    return(LOGGER_ERR_OUTPUT_INVALID);
  }

  /* check if this output is registered */
  for (index = 0 ; index < size ; index++) {
    if (((outputs[index].type == LOGGER_OUTPUT_TYPE_FILESTREAM) &&
        (outputs[index].stream == stream)) ||
        ((outputs[index].type == LOGGER_OUTPUT_TYPE_FUNCTION) &&
         (outputs[index].function == function))){
      /* set color flag */
      outputs[index].use_color = flag;
      break;
    }
  }

  /* update unified output lists of each ID */
  (void)logger_output_common_unify();

  return(ret);
}


/***************************************************************************//**
 * \brief  Query the output color setting.
 *
 * Query the color setting for a given output.
 *
 * \param[in,out] outputs   List of logger outputs.
 * \param[in]     size      Number of elements in list.
 * \param[in,out] outputs   List of logger outputs.
 * \param[in]     size      Number of elements in list.
 * \param[in]     type      Type of output.
 * \param[in]     stream    Opened file stream.
 * \param[in]     function  User provided output function.
 *
 * \return        \c logger_true if logger is found, logger_false otherwise.
 ******************************************************************************/
LOGGER_INLINE logger_bool_t logger_output_common_color_is_enabled(logger_output_t          *outputs,
                                                                  const size_t             size,
                                                                  logger_output_type_t     type,
                                                                  FILE                     *stream,
                                                                  logger_output_function_t function)
{
  logger_bool_t ret = logger_false;
  size_t        index;

  /* GUARD: check for valid type */
  if ((type <= LOGGER_OUTPUT_TYPE_UNKNOWN) ||
      (type >= LOGGER_OUTPUT_TYPE_MAX)) {
    return(LOGGER_ERR_TYPE_INVALID);
  }

  /* GUARD: check for valid stream */
  if ((type == LOGGER_OUTPUT_TYPE_FILESTREAM) &&
      (stream == NULL)) {
    return(LOGGER_ERR_STREAM_INVALID);
  }

  /* GUARD: check for valid function */
  if ((type == LOGGER_OUTPUT_TYPE_FUNCTION) &&
      (function == NULL)) {
    return(LOGGER_ERR_FUNCTION_INVALID);
  }

  /* GUARD: check for valid output */
  if (outputs == NULL) {
    return(logger_false);
  }

  /* check if this output is registered */
  for (index = 0 ; index < size ; index++) {
    if (((outputs[index].type == LOGGER_OUTPUT_TYPE_FILESTREAM) &&
        (outputs[index].stream == stream)) ||
        ((outputs[index].type == LOGGER_OUTPUT_TYPE_FUNCTION) &&
         (outputs[index].function == function))){
      ret = outputs[index].use_color;
      break;
    }
  }

  return(ret);
}


/***************************************************************************//**
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
logger_return_t logger_output_register(FILE *stream)
{
  logger_return_t ret = LOGGER_OK;

  /* add stream to global outputs */
  ret = logger_output_common_register(logger_outputs,
                                      LOGGER_ELEMENTS(logger_outputs),
                                      LOGGER_OUTPUT_TYPE_FILESTREAM,
                                      stream,
                                      (logger_output_function_t)NULL);

  /* update unified output lists of each ID */
  (void)logger_output_common_unify();

  return(ret);
}


/***************************************************************************//**
 * \brief  Deregister a global output stream.
 *
 * Remove given file stream from list of outputs.
 *
 * \param[in]     stream  Previous registered file stream.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_output_deregister(FILE *stream)
{
  logger_return_t ret = LOGGER_OK;

  /* delete stream from global outputs */
  ret = logger_output_common_deregister(logger_outputs,
                                        LOGGER_ELEMENTS(logger_outputs),
                                        LOGGER_OUTPUT_TYPE_FILESTREAM,
                                        stream,
                                        (logger_output_function_t)NULL);

  /* update unified output lists of each ID */
  (void)logger_output_common_unify();

  return(ret);
}


/***************************************************************************//**
 * \brief  Search an output stream in list of global outputs.
 *
 * The given file stream is searched in the list of global outputs.
 *
 * \param[in]     stream  File stream.
 *
 * \return        \c logger_true if logger is found, logger_false otherwise.
 ******************************************************************************/
logger_bool_t logger_output_is_registered(FILE *stream)
{
  /* search stream in global outputs */
  return(logger_output_common_is_registered(logger_outputs,
                                            LOGGER_ELEMENTS(logger_outputs),
                                            LOGGER_OUTPUT_TYPE_FILESTREAM,
                                            stream,
                                            (logger_output_function_t)NULL));
}


/***************************************************************************//**
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
logger_return_t logger_output_level_set(FILE                 *stream,
                                        const logger_level_t level)
{
  /* GUARD: check for valid level */
  if ((level & ~LOGGER_ALL) != 0 &&
      (level & (level - 1)) == 0) {
    return(LOGGER_ERR_LEVEL_UNKNOWN);
  }

  /* set stream output level to global outputs */
  return(logger_output_common_level_set(logger_outputs,
                                        LOGGER_ELEMENTS(logger_outputs),
                                        LOGGER_OUTPUT_TYPE_FILESTREAM,
                                        stream,
                                        (logger_output_function_t)NULL,
                                        LOGGER_ALL ^ (level - 1)));
}


/***************************************************************************//**
 * \brief  Query logging level for global output stream.
 *
 * Query the currently set minimum level for the given logging output stream.
 *
 * \param[in]     stream  Previous registered file stream.
 *
 * \return        Currently set logging level.
 ******************************************************************************/
logger_level_t logger_output_level_get(FILE *stream)
{
  logger_level_t level;

  /* get stream output level to global outputs */
  level = logger_output_common_level_get(logger_outputs,
                                         LOGGER_ELEMENTS(logger_outputs),
                                         LOGGER_OUTPUT_TYPE_FILESTREAM,
                                         stream,
                                         (logger_output_function_t)NULL);

  /* return only lowest set bit */
  return(LOGGER_ALL ^ (level - 1));
}


/***************************************************************************//**
 * \brief  Set logging level mask for global output stream.
 *
 * Set a logging level mask for given output stream. Only log messages with
 * a level set in the given level mask will be printed to the given stream.
 *
 * \param[in]     stream  Previous registered file stream.
 * \param[in]     level   Level mask to set.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_output_level_mask_set(FILE                 *stream,
                                             const logger_level_t level)
{
  /* GUARD: check for valid level */
  if ((level & ~LOGGER_ALL) != 0) {
    return(LOGGER_ERR_LEVEL_UNKNOWN);
  }

  /* set stream output level to global outputs */
  return(logger_output_common_level_set(logger_outputs,
                                        LOGGER_ELEMENTS(logger_outputs),
                                        LOGGER_OUTPUT_TYPE_FILESTREAM,
                                        stream,
                                        (logger_output_function_t)NULL,
                                        level));
}


/***************************************************************************//**
 * \brief  Query logging level mask for global output stream.
 *
 * Query the currently set level mask for the given logging output stream.
 *
 * \param[in]     stream  Previous registered file stream.
 *
 * \return        Currently set logging level mask.
 ******************************************************************************/
logger_level_t logger_output_level_mask_get(FILE *stream)
{
  logger_level_t level;

  /* get stream output level to global outputs */
  level = logger_output_common_level_get(logger_outputs,
                                         LOGGER_ELEMENTS(logger_outputs),
                                         LOGGER_OUTPUT_TYPE_FILESTREAM,
                                         stream,
                                         (logger_output_function_t)NULL);

  return(level);
}


/***************************************************************************//**
 * \brief  Enable the global output stream color setting.
 *
 * Enable the color setting for a given output stream.
 *
 * \param[in]     stream  Opened file stream.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_output_color_enable(FILE *stream)
{
  /* set stream output level to global outputs */
  return(logger_output_common_color(logger_outputs,
                                    LOGGER_ELEMENTS(logger_outputs),
                                    LOGGER_OUTPUT_TYPE_FILESTREAM,
                                    stream,
                                    (logger_output_function_t)NULL,
                                    logger_true));
}


/***************************************************************************//**
 * \brief  Disable the global output stream color setting.
 *
 * Disable the color setting for a given output stream.
 *
 * \param[in]     stream  Opened file stream.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_output_color_disable(FILE *stream)
{
  /* set stream output level to global outputs */
  return(logger_output_common_color(logger_outputs,
                                    LOGGER_ELEMENTS(logger_outputs),
                                    LOGGER_OUTPUT_TYPE_FILESTREAM,
                                    stream,
                                    (logger_output_function_t)NULL,
                                    logger_false));
}


/***************************************************************************//**
 * \brief  Query the global output stream color setting.
 *
 * Query the color setting for a given output stream.
 *
 * \param[in]     stream    Opened file stream.
 *
 * \return        \c logger_true if logger is found, logger_false otherwise
 ******************************************************************************/
logger_bool_t logger_output_color_is_enabled(FILE *stream)
{
  /* check if color for this stream is enabled */
  return(logger_output_common_color_is_enabled(logger_outputs,
                                               LOGGER_ELEMENTS(logger_outputs),
                                               LOGGER_OUTPUT_TYPE_FILESTREAM,
                                               stream,
                                               (logger_output_function_t)NULL));
}


/***************************************************************************//**
 * \brief  Flush all output streams.
 *
 * Force a flush of all output streams.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_output_flush(void)
{
  size_t index;
  size_t id;

  /* outputs will change, print repeat message */
  logger_repeat_message();

  /* outputs will change, print rate limit message */
  logger_rate_limit_message(logger_true);

  /* search for used global outputs */
  for (index = 0 ; index < LOGGER_ELEMENTS(logger_outputs) ; index++) {
    if (logger_outputs[index].count > 0) {
      (void)fflush(logger_outputs[index].stream);
    }
  }

  /* search for used id specific outputs */
  for (id = 0 ; id < LOGGER_ELEMENTS(logger_control) ; id++) {
    for (index = 0 ; index < LOGGER_ELEMENTS(logger_control[id].outputs) ; index++) {
      if (logger_control[id].outputs[index].count > 0) {
        (void)fflush(logger_control[id].outputs[index].stream);
      }
    }
  }

  return(LOGGER_OK);
}


/***************************************************************************//**
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
logger_return_t logger_output_function_register(logger_output_function_t function)
{
  logger_return_t ret = LOGGER_OK;

  /* add function to global outputs */
  ret = logger_output_common_register(logger_outputs,
                                      LOGGER_ELEMENTS(logger_outputs),
                                      LOGGER_OUTPUT_TYPE_FUNCTION,
                                      (FILE *)NULL,
                                      function);

  /* update unified output lists of each ID */
  (void)logger_output_common_unify();

  return(ret);
}


/***************************************************************************//**
 * \brief  Deregister a global output function:
 *
 * Remove given output function from list of outputs.
 *
 * \param[in]     function  User provided output function.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_output_function_deregister(logger_output_function_t function)
{
  logger_return_t ret = LOGGER_OK;

  /* delete function from global outputs */
  ret = logger_output_common_deregister(logger_outputs,
                                        LOGGER_ELEMENTS(logger_outputs),
                                        LOGGER_OUTPUT_TYPE_FUNCTION,
                                        (FILE *)NULL,
                                        function);

  /* update unified output lists of each ID */
  (void)logger_output_common_unify();

  return(ret);
}


/***************************************************************************//**
 * \brief  Search a global output function in a list of global outputs.
 *
 * The given file function is searched in the list of global outputs.
 *
 * \param[in]     function  User provided output function.
 *
 * \return        \c logger_true if logger is found, logger_false otherwise.
 ******************************************************************************/
logger_bool_t logger_output_function_is_registered(logger_output_function_t function)
{
  /* search function in global outputs */
  return(logger_output_common_is_registered(logger_outputs,
                                            LOGGER_ELEMENTS(logger_outputs),
                                            LOGGER_OUTPUT_TYPE_FUNCTION,
                                            (FILE *)NULL,
                                            function));
}


/***************************************************************************//**
 * \brief  Set logging level for global output function.
 *
 * Set the minimum logging level for given output function. Only for log
 * messages equal or above the given level the function will be called.
 *
 * \param[in]     function  User provided output function.
 * \param[in]     level     Level to set.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_output_function_level_set(logger_output_function_t function,
                                                 const logger_level_t     level)
{
  /* GUARD: check for valid level */
  if ((level & ~LOGGER_ALL) != 0 &&
      (level & (level - 1)) == 0) {
    return(LOGGER_ERR_LEVEL_UNKNOWN);
  }

  /* set function output level from global outputs */
  return(logger_output_common_level_set(logger_outputs,
                                        LOGGER_ELEMENTS(logger_outputs),
                                        LOGGER_OUTPUT_TYPE_FUNCTION,
                                        (FILE *)NULL,
                                        function,
                                        LOGGER_ALL ^ (level - 1)));
}


/***************************************************************************//**
 * \brief  Query logging level for global output function.
 *
 * Query the currently set minimum level for the given logging output function.
 *
 * \param[in]     function  User provided output function.
 *
 * \return        Currently set logging level.
 ******************************************************************************/
logger_level_t logger_output_function_level_get(logger_output_function_t function)
{
  logger_level_t level;

  /* get function output level to global outputs */
  level = logger_output_common_level_get(logger_outputs,
                                         LOGGER_ELEMENTS(logger_outputs),
                                         LOGGER_OUTPUT_TYPE_FUNCTION,
                                         (FILE *)NULL,
                                         function);

  /* return only lowest set bit */
  return(LOGGER_ALL ^ (level - 1));
}


/***************************************************************************//**
 * \brief  Set logging level mask for global output function.
 *
 * Set a logging level mask for given output function. Only for log messages
 * with a level set in the given level mask the function will be called.
 *
 * \param[in]     function  User provided output function.
 * \param[in]     level     Level mask to set.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_output_function_level_mask_set(logger_output_function_t function,
                                                      const logger_level_t     level)
{
  /* GUARD: check for valid level */
  if ((level & ~LOGGER_ALL) != 0) {
    return(LOGGER_ERR_LEVEL_UNKNOWN);
  }

  /* set function output level to global outputs */
  return(logger_output_common_level_set(logger_outputs,
                                        LOGGER_ELEMENTS(logger_outputs),
                                        LOGGER_OUTPUT_TYPE_FUNCTION,
                                        (FILE *)NULL,
                                        function,
                                        level));
}


/***************************************************************************//**
 * \brief  Query logging level mask for global output function.
 *
 * Query the currently set level mask for the given logging output function.
 *
 * \param[in]     function  User provided output function.
 *
 * \return        Currently set logging level mask.
 ******************************************************************************/
logger_level_t logger_output_function_level_mask_get(logger_output_function_t function)
{
  logger_level_t level;

  /* get function output level from global outputs */
  level = logger_output_common_level_get(logger_outputs,
                                         LOGGER_ELEMENTS(logger_outputs),
                                         LOGGER_OUTPUT_TYPE_FUNCTION,
                                         (FILE *)NULL,
                                         function);

  return(level);
}


/***************************************************************************//**
 * \brief  Enable the global output function color setting.
 *
 * Enable the color setting for a given output function.
 *
 * \param[in]     function  User provided output function.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_output_function_color_enable(logger_output_function_t function)
{
  /* set function output level to global outputs */
  return(logger_output_common_color(logger_outputs,
                                    LOGGER_ELEMENTS(logger_outputs),
                                    LOGGER_OUTPUT_TYPE_FUNCTION,
                                    (FILE *)NULL,
                                    function,
                                    logger_true));
}


/***************************************************************************//**
 * \brief  Enable the global output function color setting.
 *
 * Enable the color setting for a given output function.
 *
 * \param[in]     function  User provided output function.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_output_function_color_disable(logger_output_function_t function)
{
  /* set function output level to global outputs */
  return(logger_output_common_color(logger_outputs,
                                    LOGGER_ELEMENTS(logger_outputs),
                                    LOGGER_OUTPUT_TYPE_FUNCTION,
                                    (FILE *)NULL,
                                    function,
                                    logger_false));
}


/***************************************************************************//**
 * \brief  Query the global output function color setting.
 *
 * Query the color setting for a given output function.
 *
 * \param[in]     function  User provided output function.
 *
 * \return        \c logger_true if logger is found, logger_false otherwise.
 ******************************************************************************/
logger_bool_t logger_output_function_color_is_enabled(logger_output_function_t function)
{
  /* check if color for this function is enabled */
  return(logger_output_common_color_is_enabled(logger_outputs,
                                               LOGGER_ELEMENTS(logger_outputs),
                                               LOGGER_OUTPUT_TYPE_FUNCTION,
                                               (FILE *)NULL,
                                               function));
}


/***************************************************************************//**
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
logger_id_t logger_id_request(const char *name)
{
  logger_id_t   id = logger_id_unknown;
  size_t        index;
  logger_bool_t found;

  /* GUARD: check for valid name */
  if (name == NULL) {
    return(logger_id_unknown);
  }

  /* start search */
  found = logger_false;

  /* search for an already existing ID with the same name */
  for (index = 0 ; index < LOGGER_ELEMENTS(logger_control) ; index++) {
    if (logger_control[index].used == logger_true) {
      if (strncmp(logger_control[index].name, name, LOGGER_ELEMENTS(logger_control[index].name)) == 0) {
        logger_control[index].count++;
        found = logger_true;
        break;
      }
    }
  }

  /* search for an available ID */
  if (found == logger_false) {
    for (index = 0 ; index < LOGGER_ELEMENTS(logger_control) ; index++) {
      if (logger_control[index].used == logger_false) {
        found = logger_true;
        /* reset the ID */
        (void)memset(&logger_control[index], 0, sizeof(logger_control[index]));

        /* initialize the ID */
        logger_control[index].used                  = logger_true;
        logger_control[index].count                 = 1;
        logger_control[index].enabled               = logger_false;
        logger_control[index].level                 = LOGGER_UNKNOWN;
        logger_control[index].prefix                = LOGGER_PFX_UNSET;
        logger_control[index].color                 = logger_false;
        logger_control[index].color_string.begin[0] = '\0';
        logger_control[index].color_string.end[0]   = '\0';
        logger_control[index].color_string_changed  = logger_false;

        /* copy the name */
        (void)strncpy(logger_control[index].name, name, LOGGER_ELEMENTS(logger_control[index].name));
        logger_control[index].name[LOGGER_ELEMENTS(logger_control[index].name) - 1] = '\0';

        /* update unified output lists of each ID */
        (void)logger_output_common_unify();

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

  return(id);
}


/***************************************************************************//**
 * \brief  Release a logging ID.
 *
 * Release a ID. After this call logging to this ID is not possible anymore.
 *
 * \param[in]     id      Logger ID.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_id_release(const logger_id_t id)
{
  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_ERR_ID_UNKNOWN);
  }

  logger_control[id].count--;

  /* if this was the last ID */
  if (logger_control[id].count <= 0) {
    /* outputs will change, print repeat message */
    logger_repeat_message();

    /* outputs will change, print rate limit message */
    logger_rate_limit_message(logger_true);

    /* reset the ID */
    (void)memset(&logger_control[id], 0, sizeof(logger_control[id]));

    /* reset all ID dependent values to defaults */
    logger_control[id].used                  = logger_false;
    logger_control[id].count                 = 0;
    logger_control[id].enabled               = logger_false;
    logger_control[id].level                 = LOGGER_UNKNOWN;
    logger_control[id].prefix                = LOGGER_PFX_UNSET;
    logger_control[id].color                 = logger_false;
    logger_control[id].color_string.begin[0] = '\0';
    logger_control[id].color_string.end[0]   = '\0';
    logger_control[id].color_string_changed  = logger_false;
    logger_control[id].name[0]               = '\0';

    /* reset outputs */
    (void)memset(logger_control[id].outputs, 0, sizeof(logger_control[id].outputs));
  }

  return(LOGGER_OK);
}


/***************************************************************************//**
 * \brief  Enable a logging ID for output.
 *
 * Enable given logging ID for all outputs.
 *
 * \param[in]     id      Logger ID.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_id_enable(const logger_id_t id)
{
  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_ERR_ID_UNKNOWN);
  }

  /* enable given ID */
  logger_control[id].enabled = logger_true;

  return(LOGGER_OK);
}


/***************************************************************************//**
 * \brief  Disable a logger ID for output.
 *
 * Disable given logging ID for all outputs.
 *
 * \param[in]     id      Logger ID.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_id_disable(const logger_id_t id)
{
  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_ERR_ID_UNKNOWN);
  }

  /* outputs will change, print repeat message */
  logger_repeat_message();

  /* outputs will change, print rate limit message */
  logger_rate_limit_message(logger_true);

  /* disable given ID */
  logger_control[id].enabled = logger_false;

  return(LOGGER_OK);
}


/***************************************************************************//**
 * \brief  Query the current enable state of ID.
 *
 * Query the current enable state of given logging ID.
 *
 * \param[in]     id      Logger ID.
 *
 * \return        \c logger_true if id is enabled, \c logger_false otherwise.
 ******************************************************************************/
logger_bool_t logger_id_is_enabled(const logger_id_t id)
{
  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_ERR_ID_UNKNOWN);
  }

  /* ID enable state */
  return(logger_control[id].enabled);
}


/***************************************************************************//**
 * \brief  Query if a given level for a given ID would generate any output
 *
 * Query if a call to logger() for the given ID and level would generate any
 * output. This is useful to to avoid expensive calculations when subsequent
 * calls to logger() would never output the results.
 *
 * \param[in]     id      Logger ID.
 * \param[in]     level   Level.
 *
 * \return        \c logger_true if output would be generated, \c logger_false otherwise.
 ******************************************************************************/
logger_bool_t logger_id_generates_output(const logger_id_t    id,
                                         const logger_level_t level)
{
  logger_bool_t generates_output = logger_false;
  size_t        index;

  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(logger_false);
  }

  /* GUARD: check for valid level */
  if ((level & ~LOGGER_ALL) != 0) {
    return(logger_false);
  }

  /* check if ID is enabled and level is enabled */
  if ((logger_enabled == logger_true) &&
      (logger_control[id].enabled == logger_true) &&
      ((logger_control[id].level & level) != 0)) {
    /* loop over all possible outputs */
    for (index = 0 ; index < LOGGER_ELEMENTS(logger_control[id].unified_outputs) ; index++) {
      /* heck if output is enabled and level is enabled */
      if ((logger_control[id].unified_outputs[index].count > 0) &&
          ((logger_control[id].unified_outputs[index].level & level) != 0)) {
        generates_output = logger_true;
        break;
      }
    }
  }

  return(generates_output);
}


/***************************************************************************//**
 * \brief  Set logging level for ID.
 *
 * Set the minimum logging level for given ID. Only log messages equal or above
 * the given level will be printed to outputs.
 *
 * \param[in]     id      Logger ID.
 * \param[in]     level   Level to set.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_id_level_set(const logger_id_t    id,
                                    const logger_level_t level)
{
  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_ERR_ID_UNKNOWN);
  }

  /* GUARD: check for valid level */
  if ((level & ~LOGGER_ALL) != 0 &&
      (level & (level - 1)) == 0) {
    return(LOGGER_ERR_LEVEL_UNKNOWN);
  }

  /* outputs will change, print repeat message */
  logger_repeat_message();

  /* outputs will change, print rate limit message */
  logger_rate_limit_message(logger_true);

  /* set ID level */
  logger_control[id].level = LOGGER_ALL ^ (level - 1);

  return(LOGGER_OK);
}


/***************************************************************************//**
 * \brief  Query logging level for ID.
 *
 * Query the currently set minimum level for the given logging ID.
 *
 * \param[in]     id      Logger ID.
 *
 * \return        Currently set level.
 ******************************************************************************/
logger_level_t logger_id_level_get(const logger_id_t id)
{
  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_UNKNOWN);
  }

  /* get ID level, return only lowest set bit */
  return(LOGGER_ALL ^ (logger_control[id].level - 1));
}


/***************************************************************************//**
 * \brief  Set logging level mask for ID.
 *
 * Set a logging level mask for given ID. Only log messages with a level set in
 * the given level mask will be printed to outputs.
 *
 * \param[in]     id      Logger ID.
 * \param[in]     level   Level to set.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_id_level_mask_set(const logger_id_t    id,
                                         const logger_level_t level)
{
  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_ERR_ID_UNKNOWN);
  }

  /* GUARD: check for valid level */
  if ((level & ~LOGGER_ALL) != 0) {
    return(LOGGER_ERR_LEVEL_UNKNOWN);
  }

  /* outputs will change, print repeat message */
  logger_repeat_message();

  /* outputs will change, print rate limit message */
  logger_rate_limit_message(logger_true);

  /* set ID level */
  logger_control[id].level = level;

  return(LOGGER_OK);
}


/***************************************************************************//**
 * \brief  Query logging level for ID.
 *
 * Query the currently set minimum level for the given logging ID.
 *
 * \param[in]     id      Logger ID.
 *
 * \return        Currently set level.
 ******************************************************************************/
logger_level_t logger_id_level_mask_get(const logger_id_t id)
{
  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_UNKNOWN);
  }

  return(logger_control[id].level);
}


/***************************************************************************//**
 * \brief  Set logging prefix for ID.
 *
 * Set the prefix for given ID. All messages for this ID will get this prefix
 * from now on.
 *
 * \param[in]     id      Logger ID.
 * \param[in]     prefix  Prefix to set.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_id_prefix_set(const logger_id_t     id,
                                     const logger_prefix_t prefix)
{
  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_ERR_ID_UNKNOWN);
  }

  /* GUARD: check for valid prefix */
  if ((prefix < LOGGER_PFX_EMPTY) ||
      (prefix > LOGGER_PFX_UNSET)) {
    return(LOGGER_ERR_PREFIX_UNKNOWN);
  }

  /* set ID prefix */
  logger_control[id].prefix = prefix;

  return(LOGGER_OK);
}


/***************************************************************************//**
 * \brief  Query logging prefix for ID.
 *
 * Query the currently set prefix for the given logging ID.
 *
 * \param[in]     id      Logger ID.
 *
 * \return        Currently set prefix.
 ******************************************************************************/
logger_prefix_t logger_id_prefix_get(const logger_id_t id)
{
  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_PFX_EMPTY);
  }

  /* get ID prefix */
  return(logger_control[id].prefix);
}


/***************************************************************************//**
 * \brief  Query name for id.
 *
 * Query the name for the given logging ID.
 *
 * \param[in]     id      Logger ID.
 *
 * \return        Symbolic name of the ID.
 ******************************************************************************/
const char *logger_id_name_get(const logger_id_t id)
{
  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(NULL);
  }

  /* get ID name */
  return(logger_control[id].name);
}


/***************************************************************************//**
 * \brief  Register an id specific output stream.
 *
 * The given file stream may be on of \c stdout, \c stderr or a file stream
 * opened by the user. The default logging level is set to \c LOGGER_UNKNOWN thus
 * no messages will appear on this stream.
 *
 * \param[in]     id      Logger ID.
 * \param[in]     stream  Opened file stream.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_id_output_register(const logger_id_t id,
                                          FILE              *stream)
{
  logger_return_t ret = LOGGER_OK;

  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_ERR_ID_UNKNOWN);
  }

  /* add stream to system outputs */
  (void)logger_output_common_register(logger_control[logger_system_id].outputs,
                                      LOGGER_ELEMENTS(logger_control[logger_system_id].outputs),
                                      LOGGER_OUTPUT_TYPE_FILESTREAM,
                                      stream,
                                      (logger_output_function_t)NULL);
  (void)logger_output_common_level_set(logger_control[logger_system_id].outputs,
                                      LOGGER_ELEMENTS(logger_control[logger_system_id].outputs),
                                      LOGGER_OUTPUT_TYPE_FILESTREAM,
                                      stream,
                                      (logger_output_function_t)NULL,
                                      LOGGER_ALL);

  /* add stream to global outputs */
  ret = logger_output_common_register(logger_control[id].outputs,
                                      LOGGER_ELEMENTS(logger_control[id].outputs),
                                      LOGGER_OUTPUT_TYPE_FILESTREAM,
                                      stream,
                                      (logger_output_function_t)NULL);

  /* update unified output lists of each ID */
  (void)logger_output_common_unify();

  return(ret);
}


/***************************************************************************//**
 * \brief  Deregister an id specific output stream.
 *
 * Remove given file stream from list of outputs.
 *
 * \param[in]     id      Logger ID.
 * \param[in]     stream  Previous registered file stream.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_id_output_deregister(const logger_id_t id,
                                            FILE              *stream)
{
  logger_return_t ret = LOGGER_OK;

  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_ERR_ID_UNKNOWN);
  }

  /* delete stream from global outputs */
  ret = logger_output_common_deregister(logger_control[id].outputs,
                                        LOGGER_ELEMENTS(logger_control[id].outputs),
                                        LOGGER_OUTPUT_TYPE_FILESTREAM,
                                        stream,
                                        (logger_output_function_t)NULL);

  /* delete stream from system outputs */
  (void)logger_output_common_deregister(logger_control[logger_system_id].outputs,
                                        LOGGER_ELEMENTS(logger_control[logger_system_id].outputs),
                                        LOGGER_OUTPUT_TYPE_FILESTREAM,
                                        stream,
                                        (logger_output_function_t)NULL);

  /* update unified output lists of each ID */
  (void)logger_output_common_unify();

  return(ret);
}


/***************************************************************************//**
 * \brief  Search an output stream in id specific outputs.
 *
 * The given file stream is searched in of id specific outputs.
 *
 * \param[in]     id      Logger ID.
 * \param[in]     stream  File stream.
 *
 * \return        \c logger_true if logger is found, logger_false otherwise.
 ******************************************************************************/
logger_bool_t logger_id_output_is_registered(const logger_id_t id,
                                             FILE              *stream)
{
  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_ERR_ID_UNKNOWN);
  }

  /* search stream in id specific outputs */
  return(logger_output_common_is_registered(logger_control[id].outputs,
                                            LOGGER_ELEMENTS(logger_control[id].outputs),
                                            LOGGER_OUTPUT_TYPE_FILESTREAM,
                                            stream,
                                            (logger_output_function_t)NULL));
}


/***************************************************************************//**
 * \brief  Set logging level for id specific output stream.
 *
 * Set the minimum logging level for given output stream. Only log messages
 * equal or above the given level will be printed to the given stream.
 *
 * \param[in]     id      Logger ID.
 * \param[in]     stream  Previous registered file stream.
 * \param[in]     level   Level to set.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_id_output_level_set(const logger_id_t    id,
                                           FILE                 *stream,
                                           const logger_level_t level)
{
  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_ERR_ID_UNKNOWN);
  }

  /* GUARD: check for valid level */
  if ((level & ~LOGGER_ALL) != 0 &&
      (level & (level - 1)) == 0) {
    return(LOGGER_ERR_LEVEL_UNKNOWN);
  }

  /* set stream output level to id specific outputs */
  return(logger_output_common_level_set(logger_control[id].outputs,
                                        LOGGER_ELEMENTS(logger_control[id].outputs),
                                        LOGGER_OUTPUT_TYPE_FILESTREAM,
                                        stream,
                                        (logger_output_function_t)NULL,
                                        LOGGER_ALL ^ (level - 1)));
}


/***************************************************************************//**
 * \brief  Query logging level for id specific output stream.
 *
 * Query the currently set minimum level for the given logging output stream.
 *
 * \param[in]     id      Logger ID.
 * \param[in]     stream  Previous registered file stream.
 *
 * \return        Currently set logging level.
 ******************************************************************************/
logger_level_t logger_id_output_level_get(const logger_id_t id,
                                          FILE              *stream)
{
  logger_level_t level;

  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_UNKNOWN);
  }

  /* get stream output level from id specific outputs */
  level = logger_output_common_level_get(logger_control[id].outputs,
                                         LOGGER_ELEMENTS(logger_control[id].outputs),
                                         LOGGER_OUTPUT_TYPE_FILESTREAM,
                                         stream,
                                         (logger_output_function_t)NULL);

  /* return only lowest set bit */
  return(LOGGER_ALL ^ (level - 1));
}


/***************************************************************************//**
 * \brief  Set logging level mask for id specific output stream.
 *
 * Set a logging level mask for given output stream. Only log messages
 * with a level set in the given level mask will be printed to the given stream.
 *
 * \param[in]     id      Logger ID.
 * \param[in]     stream  Previous registered file stream.
 * \param[in]     level   Level mask to set.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_id_output_level_mask_set(const logger_id_t    id,
                                                FILE                 *stream,
                                                const logger_level_t level)
{
  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_ERR_ID_UNKNOWN);
  }

  /* GUARD: check for valid level */
  if ((level & ~LOGGER_ALL) != 0) {
    return(LOGGER_ERR_LEVEL_UNKNOWN);
  }

  /* set stream output level to id specific outputs */
  return(logger_output_common_level_set(logger_control[id].outputs,
                                        LOGGER_ELEMENTS(logger_control[id].outputs),
                                        LOGGER_OUTPUT_TYPE_FILESTREAM,
                                        stream,
                                        (logger_output_function_t)NULL,
                                        level));
}


/***************************************************************************//**
 * \brief  Query logging level mask for id specific output stream.
 *
 * Query the currently set level mask for the given logging output stream.
 *
 * \param[in]     id      Logger ID.
 * \param[in]     stream  Previous registered file stream.
 *
 * \return        Currently set logging level mask.
 ******************************************************************************/
logger_level_t logger_id_output_level_mask_get(const logger_id_t id,
                                               FILE              *stream)
{
  logger_level_t level;

  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_UNKNOWN);
  }

  /* get stream output level from id specific outputs */
  level = logger_output_common_level_get(logger_control[id].outputs,
                                         LOGGER_ELEMENTS(logger_control[id].outputs),
                                         LOGGER_OUTPUT_TYPE_FILESTREAM,
                                         stream,
                                         (logger_output_function_t)NULL);

  return(level);
}


/***************************************************************************//**
 * \brief  Enable the id specific output stream color setting.
 *
 * Enable the color setting for a given output stream.
 *
 * \param[in]     id      Logger ID.
 * \param[in]     stream  Opened file stream.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_id_output_color_enable(const logger_id_t id,
                                              FILE              *stream)
{
  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_ERR_ID_UNKNOWN);
  }

  /* enable stream output color in id specific outputs */
  return(logger_output_common_color(logger_control[id].outputs,
                                    LOGGER_ELEMENTS(logger_control[id].outputs),
                                    LOGGER_OUTPUT_TYPE_FILESTREAM,
                                    stream,
                                    (logger_output_function_t)NULL,
                                    logger_true));
}


/***************************************************************************//**
 * \brief  Disable the id specific output stream color setting.
 *
 * Disable the color setting for a given output stream.
 *
 * \param[in]     id      Logger ID.
 * \param[in]     stream  Opened file stream.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_id_output_color_disable(const logger_id_t id,
                                               FILE              *stream)
{
  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_ERR_ID_UNKNOWN);
  }

  /* disable stream output color in id specific outputs */
  return(logger_output_common_color(logger_control[id].outputs,
                                    LOGGER_ELEMENTS(logger_control[id].outputs),
                                    LOGGER_OUTPUT_TYPE_FILESTREAM,
                                    stream,
                                    (logger_output_function_t)NULL,
                                    logger_false));
}


/***************************************************************************//**
 * \brief  Query the id specific output stream color setting.
 *
 * Query the color setting for a given output stream.
 *
 * \param[in]     id      Logger ID.
 * \param[in]     stream  Opened file stream.
 *
 * \return        \c logger_true if logger is found, logger_false otherwise.
 ******************************************************************************/
logger_bool_t logger_id_output_color_is_enabled(const logger_id_t id,
                                                FILE              *stream)
{
  /* check if color for this stream is enabled */
  return(logger_output_common_color_is_enabled(logger_control[id].outputs,
                                               LOGGER_ELEMENTS(logger_control[id].outputs),
                                               LOGGER_OUTPUT_TYPE_FILESTREAM,
                                               stream,
                                               (logger_output_function_t)NULL));
}


/***************************************************************************//**
 * \brief  Register an id specific output stream.
 *
 * The user provided function must take a single string argument. The default
 * logging level is set to \c LOGGER_UNKNOWN thus this function will not be
 * called for any message.
 *
 * \param[in]     id        Logger ID.
 * \param[in]     function  User provided output function.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_id_output_function_register(const logger_id_t        id,
                                                   logger_output_function_t function)
{
  logger_return_t ret = LOGGER_OK;

  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_ERR_ID_UNKNOWN);
  }

  /* add function to system outputs */
  (void)logger_output_common_register(logger_control[logger_system_id].outputs,
                                      LOGGER_ELEMENTS(logger_control[logger_system_id].outputs),
                                      LOGGER_OUTPUT_TYPE_FUNCTION,
                                      (FILE *)NULL,
                                      function);
  (void)logger_output_common_level_set(logger_control[logger_system_id].outputs,
                                      LOGGER_ELEMENTS(logger_control[logger_system_id].outputs),
                                      LOGGER_OUTPUT_TYPE_FUNCTION,
                                      (FILE *)NULL,
                                      function,
                                      LOGGER_ALL);

  /* add function to global outputs */
  ret = logger_output_common_register(logger_control[id].outputs,
                                      LOGGER_ELEMENTS(logger_control[id].outputs),
                                      LOGGER_OUTPUT_TYPE_FUNCTION,
                                      (FILE *)NULL,
                                      function);

  /* update unified output lists of each ID */
  (void)logger_output_common_unify();

  return(ret);
}


/***************************************************************************//**
 * \brief  Deregister an id specific output function.
 *
 * Remove given file function from list of outputs.
 *
 * \param[in]     id        Logger ID.
 * \param[in]     function  User provided output function.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_id_output_function_deregister(const logger_id_t        id,
                                                     logger_output_function_t function)
{
  logger_return_t ret = LOGGER_OK;

  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_ERR_ID_UNKNOWN);
  }

  /* delete function from global outputs */
  ret = logger_output_common_deregister(logger_control[id].outputs,
                                        LOGGER_ELEMENTS(logger_control[id].outputs),
                                        LOGGER_OUTPUT_TYPE_FUNCTION,
                                        (FILE *)NULL,
                                        function);

  /* delete function from system outputs */
  (void)logger_output_common_deregister(logger_control[logger_system_id].outputs,
                                        LOGGER_ELEMENTS(logger_control[logger_system_id].outputs),
                                        LOGGER_OUTPUT_TYPE_FUNCTION,
                                        (FILE *)NULL,
                                        function);

  /* update unified output lists of each ID */
  (void)logger_output_common_unify();

  return(ret);
}


/***************************************************************************//**
 * \brief  Search an output function in id specific outputs.
 *
 * The given output function is searched in of id specific outputs.
 *
 * \param[in]     id        Logger ID.
 * \param[in]     function  User provided output function.
 *
 * \return        \c logger_true if logger is found, logger_false otherwise.
 ******************************************************************************/
logger_bool_t logger_id_output_function_is_registered(const logger_id_t        id,
                                                      logger_output_function_t function)
{
  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_ERR_ID_UNKNOWN);
  }

  /* search function in global outputs */
  return(logger_output_common_is_registered(logger_control[id].outputs,
                                            LOGGER_ELEMENTS(logger_control[id].outputs),
                                            LOGGER_OUTPUT_TYPE_FUNCTION,
                                            (FILE *)NULL,
                                            function));
}


/***************************************************************************//**
 * \brief  Set logging level for id specific output function.
 *
 * Set the minimum logging level for given output function. Only log messages
 * equal or above the given level will be printed to the given function.
 *
 * \param[in]     id        Logger ID.
 * \param[in]     function  User provided output function.
 * \param[in]     level     Level to set.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_id_output_function_level_set(const logger_id_t        id,
                                                    logger_output_function_t function,
                                                    const logger_level_t     level)
{
  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_ERR_ID_UNKNOWN);
  }

  /* GUARD: check for valid level */
  if ((level & ~LOGGER_ALL) != 0 &&
      (level & (level - 1)) == 0) {
    return(LOGGER_ERR_LEVEL_UNKNOWN);
  }

  /* set function output level to id specific outputs */
  return(logger_output_common_level_set(logger_control[id].outputs,
                                        LOGGER_ELEMENTS(logger_control[id].outputs),
                                        LOGGER_OUTPUT_TYPE_FUNCTION,
                                        (FILE *)NULL,
                                        function,
                                        LOGGER_ALL ^ (level - 1)));
}


/***************************************************************************//**
 * \brief  Query logging level for id specific output function.
 *
 * Query the currently set minimum level for the given logging output function.
 *
 * \param[in]     id        Logger ID.
 * \param[in]     function  User provided output function.
 *
 * \return        Currently set logging level.
 ******************************************************************************/
logger_level_t logger_id_output_function_level_get(const logger_id_t        id,
                                                   logger_output_function_t function)
{
  logger_level_t level;

  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_UNKNOWN);
  }

  /* get function output level from id specific outputs */
  level = logger_output_common_level_get(logger_control[id].outputs,
                                         LOGGER_ELEMENTS(logger_control[id].outputs),
                                         LOGGER_OUTPUT_TYPE_FUNCTION,
                                         (FILE *)NULL,
                                         function);

  /* return only lowest set bit */
  return(LOGGER_ALL ^ (level - 1));
}


/***************************************************************************//**
 * \brief  Set logging level mask for id specific output function.
 *
 * Set a logging level mask for given output function. Only for log messages
 * with a level set in the given level mask the function will be called.
 *
 * \param[in]     id        Logger ID.
 * \param[in]     function  User provided output function.
 * \param[in]     level     Level mask to set.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_id_output_function_level_mask_set(const logger_id_t        id,
                                                         logger_output_function_t function,
                                                         const logger_level_t     level)
{
  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_ERR_ID_UNKNOWN);
  }

  /* GUARD: check for valid level */
  if ((level & ~LOGGER_ALL) != 0) {
    return(LOGGER_ERR_LEVEL_UNKNOWN);
  }

  /* set function output level to id specific outputs */
  return(logger_output_common_level_set(logger_control[id].outputs,
                                        LOGGER_ELEMENTS(logger_control[id].outputs),
                                        LOGGER_OUTPUT_TYPE_FUNCTION,
                                        (FILE *)NULL,
                                        function,
                                        level));
}


/***************************************************************************//**
 * \brief  Query logging level mask for id specific output function.
 *
 * Query the currently set minimum level for the given logging output function.
 *
 * \param[in]     id        Logger ID.
 * \param[in]     function  User provided output function.
 *
 * \return        Currently set logging level mask.
 ******************************************************************************/
logger_level_t logger_id_output_function_level_mask_get(const logger_id_t        id,
                                                   logger_output_function_t function)
{
  logger_level_t level;

  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_UNKNOWN);
  }

  /* get function output level from id specific outputs */
  level = logger_output_common_level_get(logger_control[id].outputs,
                                         LOGGER_ELEMENTS(logger_control[id].outputs),
                                         LOGGER_OUTPUT_TYPE_FUNCTION,
                                         (FILE *)NULL,
                                         function);

  return(level);
}


/***************************************************************************//**
 * \brief  Enable the id specific output function color setting.
 *
 * Enable the color setting for a given output function.
 *
 * \param[in]     id        Logger ID.
 * \param[in]     function  User provided output function.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_id_output_function_color_enable(const logger_id_t        id,
                                                       logger_output_function_t function)
{
  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_ERR_ID_UNKNOWN);
  }

  /* enable function output enable to id specific outputs */
  return(logger_output_common_color(logger_control[id].outputs,
                                    LOGGER_ELEMENTS(logger_control[id].outputs),
                                    LOGGER_OUTPUT_TYPE_FUNCTION,
                                    (FILE *)NULL,
                                    function,
                                    logger_true));
}


/***************************************************************************//**
 * \brief  Disable the id specific output function color setting.
 *
 * Disable the color setting for a given output function.
 *
 * \param[in]     id        Logger ID.
 * \param[in]     function  User provided output function.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_id_output_function_color_disable(const logger_id_t        id,
                                                        logger_output_function_t function)
{
  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_ERR_ID_UNKNOWN);
  }

  /* disable function output color in id specific outputs */
  return(logger_output_common_color(logger_control[id].outputs,
                                    LOGGER_ELEMENTS(logger_control[id].outputs),
                                    LOGGER_OUTPUT_TYPE_FUNCTION,
                                    (FILE *)NULL,
                                    function,
                                    logger_false));
}


/***************************************************************************//**
 * \brief  Query the id specific output function color setting.
 *
 * Query the color setting for a given output function.
 *
 * \param[in]     id        Logger ID.
 * \param[in]     function  User provided output function.
 *
 * \return        \c logger_true if logger is found, logger_false otherwise
 ******************************************************************************/
logger_bool_t logger_id_output_function_color_is_enabled(const logger_id_t        id,
                                                         logger_output_function_t function)
{
  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(logger_false);
  }

  /* check if color for this function is enabled */
  return(logger_output_common_color_is_enabled(logger_control[id].outputs,
                                               LOGGER_ELEMENTS(logger_control[id].outputs),
                                               LOGGER_OUTPUT_TYPE_FUNCTION,
                                               (FILE *)NULL,
                                               function));
}


/***************************************************************************//**
 * \brief  Change terminal text color and attributes.
 *
 * Change text color and attributes for all messages of given ID when they are
 * printed to \c stdout or \c stdin. Outputs to other streams including files
 * will have no color unless explicit enabled. This function uses escape
 * sequences of unix consoles. Attributes can be combined using `|`. To set any
 * other markup strings use logger_id_color_string_set().
 *
 * \param[in]     id      ID for setting level.
 * \param[in]     fg      Text foreground.
 * \param[in]     bg      Text background.
 * \param[in]     attr    Text attribute.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_id_color_console_set(const logger_id_t        id,
                                            const logger_text_fg_t   fg,
                                            const logger_text_bg_t   bg,
                                            const logger_text_attr_t attr)
{
  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_ERR_ID_UNKNOWN);
  }

  logger_control[id].color = logger_true;
  (void)logger_color_console_format(logger_control[id].color_string.begin, LOGGER_ELEMENTS(logger_control[id].color_string.begin), fg, bg, attr);
  (void)logger_color_console_format(logger_control[id].color_string.end, LOGGER_ELEMENTS(logger_control[id].color_string.end), LOGGER_FG_UNCHANGED, LOGGER_BG_UNCHANGED, LOGGER_ATTR_RESET);
  logger_control[id].color_string_changed = logger_true;

  return(LOGGER_OK);
}


/***************************************************************************//**
 * \brief  Change terminal text color and attributes.
 *
 * Change text color and attributes for all messages of given ID when they are
 * printed to \c stdout or \c stdin. Outputs to other streams including files
 * will have no color unless explicit enabled. To set colors for unix consoles
 * use logger_color_prefix_set().
 *
 * \param[in]     id      ID for setting level.
 * \param[in]     begin   Text color begin markup.
 * \param[in]     end     Text color end markup.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_id_color_string_set(const logger_id_t id,
                                           const char        *begin,
                                           const char        *end)
{
  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_ERR_ID_UNKNOWN);
  }

  /* GUARD: check for too long color strings */
  if ((strlen(begin) >= LOGGER_ELEMENTS(logger_control[id].color_string.begin)) ||
      (strlen(end)   >= LOGGER_ELEMENTS(logger_control[id].color_string.end))) {
    return(LOGGER_ERR_STRING_TOO_LONG);
  }

  logger_control[id].color = logger_true;
  (void)strncpy(logger_control[id].color_string.begin, begin, LOGGER_ELEMENTS(logger_control[id].color_string.begin));
  (void)strncpy(logger_control[id].color_string.end, end, LOGGER_ELEMENTS(logger_control[id].color_string.end));
  logger_control[id].color_string.begin[LOGGER_ELEMENTS(logger_control[id].color_string.begin) - 1] = '\0';
  logger_control[id].color_string.end[LOGGER_ELEMENTS(logger_control[id].color_string.end) - 1]   = '\0';
  logger_control[id].color_string_changed = logger_true;

  return(LOGGER_OK);
}


/***************************************************************************//**
 * \brief  Reset terminal text color and attributes.
 *
 * Reset text color and attributes of given ID back to defaults.
 *
 * \param[in]     id      ID for setting level.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_id_color_reset(const logger_id_t id)
{
  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_ERR_ID_UNKNOWN);
  }

  logger_control[id].color                 = logger_false;
  logger_control[id].color_string.begin[0] = '\0';
  logger_control[id].color_string.end[0]   = '\0';
  logger_control[id].color_string_changed  = logger_true;

  return(LOGGER_OK);
}


/***************************************************************************//**
 * \brief  Enable logger prefix colors.
 *
 * Enable logger prefix color output.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_color_prefix_enable(void)
{
  logger_color_prefix_enabled = logger_true;

  return(LOGGER_OK);
}


/***************************************************************************//**
 * \brief  Disable logger prefix colors.
 *
 * Disable logger prefix color output.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_color_prefix_disable(void)
{
  logger_color_prefix_enabled = logger_false;

  return(LOGGER_OK);
}


/***************************************************************************//**
 * \brief  Query the current enable state of logger prefix colors.
 *
 * Query the current prefix color enable state of logger.
 *
 * \return        \c logger_true if logger prefix color is enabled, logger_false otherwise.
 ******************************************************************************/
logger_bool_t logger_color_prefix_is_enabled(void)
{
  return(logger_color_prefix_enabled);
}


/***************************************************************************//**
 * \brief  Change terminal text color and attributes for prefix.
 *
 * Change text color and attributes for message prefix with given level when
 * they are printed to \c stdout or \c stdin. Outputs to other streams
 * including files will have no color unless explicit enabled. This function
 * uses escape sequences of unix consoles. Attributes can be combined using
 * `|`. To set any other markup strings use logger_color_prefix_string_set().
 *
 * \param[in]     level   Level to set.
 * \param[in]     fg      Text foreground.
 * \param[in]     bg      Text background.
 * \param[in]     attr    Text attribute.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_color_prefix_console_set(const logger_level_t     level,
                                                const logger_text_fg_t   fg,
                                                const logger_text_bg_t   bg,
                                                const logger_text_attr_t attr)
{
  size_t   index;

  /* GUARD: check for valid level */
  if ((level & ~LOGGER_ALL) != 0) {
    return(LOGGER_ERR_LEVEL_UNKNOWN);
  }

  /* get index for level */
  index = logger_level_to_index(level);

  (void)logger_color_console_format(logger_level_colors[index].begin, LOGGER_ELEMENTS(logger_level_colors[index].begin), fg, bg, attr);
  (void)logger_color_console_format(logger_level_colors[index].end, LOGGER_ELEMENTS(logger_level_colors[index].end), LOGGER_FG_UNCHANGED, LOGGER_BG_UNCHANGED, LOGGER_ATTR_RESET);

  return(LOGGER_OK);
}


/***************************************************************************//**
 * \brief  Change text color and attributes for prefix.
 *
 * Change text color and attributes for message prefix with given level when
 * they are printed to \c stdout or \c stdin. Outputs to other streams
 * including files will have no color unless explicit enabled. To set colors
 * for unix consoles use logger_color_prefix_set().
 *
 * \param[in]     level   Level for setting color string.
 * \param[in]     begin   Text color begin markup.
 * \param[in]     end     Text color end markup.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_color_prefix_string_set(const logger_level_t level,
                                               const char           *begin,
                                               const char           *end)
{
  size_t   index;

  /* GUARD: check for valid level */
  if ((level & ~LOGGER_ALL) != 0) {
    return(LOGGER_ERR_LEVEL_UNKNOWN);
  }

  /* GUARD: check for too long color strings */
  if ((strlen(begin) >= LOGGER_ELEMENTS(logger_level_colors[index].begin)) ||
      (strlen(end)   >= LOGGER_ELEMENTS(logger_level_colors[index].end))) {
    return(LOGGER_ERR_STRING_TOO_LONG);
  }

  /* get index for level */
  index = logger_level_to_index(level);

  (void)strncpy(logger_level_colors[index].begin, begin, LOGGER_ELEMENTS(logger_level_colors[index].begin));
  (void)strncpy(logger_level_colors[index].end, end, LOGGER_ELEMENTS(logger_level_colors[index].end));
  logger_level_colors[index].begin[LOGGER_ELEMENTS(logger_level_colors[index].begin) - 1] = '\0';
  logger_level_colors[index].end[LOGGER_ELEMENTS(logger_level_colors[index].end) - 1]   = '\0';

  return(LOGGER_OK);
}


/***************************************************************************//**
 * \brief  Change terminal text color and attributes for prefix.
 *
 * Reset text color and attributes for message prefix to console default escape
 * sequences.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_color_prefix_reset(void)
{
  memcpy(logger_level_colors, logger_level_colors_console, sizeof(logger_level_colors));

  return(LOGGER_OK);
}


/***************************************************************************//**
 * \brief  Enable logger message colors.
 *
 * Enable logger message color output.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_color_message_enable(void)
{
  logger_color_message_enabled = logger_true;

  return(LOGGER_OK);
}


/***************************************************************************//**
 * \brief  Disable logger message colors.
 *
 * Disable logger message color output.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_color_message_disable(void)
{
  logger_color_message_enabled = logger_false;

  return(LOGGER_OK);
}


/***************************************************************************//**
 * \brief  Query the current enable state of logger message colors.
 *
 * Query the current color message enable state of logger.
 *
 * \return        \c logger_true if logger color is enabled, logger_false otherwise.
 ******************************************************************************/
logger_bool_t logger_color_message_is_enabled(void)
{
  return(logger_color_message_enabled);
}


/***************************************************************************//**
 * \brief  Query name for level.
 *
 * Query the name for the given logging level.
 *
 * \param[in]     level   Level for querying name
 *
 * \return        Symbolic name of the level.
 ******************************************************************************/
const char *logger_level_name_get(const logger_level_t level)
{
  size_t   index;

  /* GUARD: check for valid level */
  if ((level & ~LOGGER_ALL) != 0) {
    return(logger_level_names[logger_level_to_index(LOGGER_UNKNOWN)]);
  }

  /* get index for level */
  index = logger_level_to_index(level);

  /* get level name */
  return(logger_level_names[index]);
}


/***************************************************************************//**
 * \brief  strip path from file name.
 *
 * Strip path from a fully qualified file name. Returns pointer to string part
 * after last separator.
 *
 * \param[in]     file    File name containing path
 *
 * \return        Pointer to file name.
 ******************************************************************************/
LOGGER_INLINE const char *logger_strip_path(const char *file)
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


/***************************************************************************//**
 * \brief  Format date string
 *
 * Print date string into provided memory.
 * \param[out]    date       Formatted date string.
 * \param[in]     date_size  String length of formatted date string.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
LOGGER_INLINE logger_return_t logger_format_date(char   *date,
                                                 size_t date_size)
{
  time_t        current_time;
  static time_t previous_time = 0;

  /* get current time */
  current_time = time(NULL);

  /* do the expensive stuff only if needed */
  if (current_time != previous_time) {
    size_t    index;
    struct tm *tmp;

    /* update previous time */
    previous_time = current_time;

    /* convert to local time */
    tmp = localtime(&current_time);

    /* if localtime returned a valid time */
    if (tmp != NULL) {
      if (strftime(date, date_size, LOGGER_FORMAT_STRFTIME, tmp) == 0) {
        for (index = 0 ; index < 8 && index < date_size - 2 ; index++) {
          date[index] = '#';
        }
        date[index] = '\0';
      }
    }
    else {
      for (index = 0 ; index < 8 && index < date_size - 2 ; index++) {
        date[index] = '#';
      }
      date[index] = '\0';
    }
  }

  return(LOGGER_OK);
}


/***************************************************************************//**
 * \brief  Format message prefix
 *
 * Print the prefix into provided memory.
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
LOGGER_INLINE logger_return_t logger_format_prefix(logger_id_t    id,
                                                   char           *prefix,
                                                   uint16_t       prefix_size,
                                                   logger_level_t level,
                                                   const char     *file,
                                                   const char     *function,
                                                   uint32_t       line)
{
  /* do prefix stuff only if needed */
  if ((logger_control[id].append == logger_false) &&
      (logger_control[id].prefix != LOGGER_PFX_EMPTY)) {
    logger_prefix_t local_prefix;
    char * write_ptr = prefix;
    char * end_ptr   = prefix + prefix_size;

    if (logger_control[id].prefix & LOGGER_PFX_UNSET) {
      local_prefix = logger_prefix_standard;
    }
    else {
      local_prefix = logger_control[id].prefix;
    }

    /* prefix date */
    if ((write_ptr < end_ptr) &&
        ((local_prefix & LOGGER_PFX_DATE) != 0)) {
      write_ptr += logger_prefix_concatenate(write_ptr,
                                             end_ptr,
                                             logger_date,
                                             LOGGER_FORMAT_STRING_DATE_WIDTH);
    }

    /* prefix id name */
    if ((write_ptr < end_ptr) &&
        ((local_prefix & LOGGER_PFX_NAME) != 0)) {
      write_ptr += logger_prefix_concatenate(write_ptr,
                                             end_ptr,
                                             logger_id_name_get(id),
                                             LOGGER_FORMAT_STRING_NAME_WIDTH);
    }

    /* prefix level */
    if ((write_ptr < end_ptr) &&
        ((local_prefix & LOGGER_PFX_LEVEL) != 0)) {
      write_ptr += logger_prefix_concatenate(write_ptr,
                                             end_ptr,
                                             logger_level_name_get(level),
                                             LOGGER_FORMAT_STRING_LEVEL_WIDTH);
    }

    /* prefix file name */
    if ((write_ptr < end_ptr) &&
        ((local_prefix & LOGGER_PFX_FILE) != 0)) {
      write_ptr += logger_prefix_concatenate(write_ptr,
                                             end_ptr,
                                             logger_strip_path(file),
                                             LOGGER_FORMAT_STRING_FILE_WIDTH);
    }

    /* prefix function name */
    if ((write_ptr < end_ptr) &&
        ((local_prefix & LOGGER_PFX_FUNCTION) != 0)) {
      write_ptr += logger_prefix_concatenate(write_ptr,
                                             end_ptr,
                                             function,
                                             LOGGER_FORMAT_STRING_FUNCTION_WIDTH);
    }

    /* prefix line number */
    if ((write_ptr < end_ptr) &&
        ((local_prefix & LOGGER_PFX_LINE) != 0)) {
      char linenumber[10];
      char *ptr = &linenumber[LOGGER_ELEMENTS(linenumber) - 1];

      /* convert line number to string, starting with the lowest digit at the end of the string */
      *ptr = '\0';
      while (line > 0) {
        int idx;

        idx  = line % 10;
        line = line / 10;
        *(--ptr) = logger_int_to_char[idx];
      }

      write_ptr += logger_prefix_concatenate(write_ptr,
                                             end_ptr,
                                             ptr,
                                             LOGGER_FORMAT_STRING_LINE_WIDTH);
    }

    /* append ' ' to prefix string */
    if (write_ptr < end_ptr) {
      *(write_ptr++) = ' ';
    }
    if (write_ptr < end_ptr) {
      *(write_ptr++) = '\0';
    }

    /* terminate prefix string if it is too small */
    if (write_ptr == end_ptr &&
        *(write_ptr - 1) != '\0') {
      *(--write_ptr) = '\0';
      *(--write_ptr) = ' ';
      *(--write_ptr) = '.';
      *(--write_ptr) = '.';
      *(--write_ptr) = '.';
    }
  }
  else {
    /* clear prefix */
    prefix[0] = '\0';
  }

  return(LOGGER_OK);
}


/***************************************************************************//**
 * \brief  String copy
 *
 * Copies string from `src` to `dest` until end on `src` is reached or `n`
 * characters are copied. The string in `dest` is '\0' terminated if length of
 * string in `src` is less than `n`. Otherwise the string will *not* be
 * terminated. The function returns the number of characters copied to `dest`
 * not including the '\0'.
 *
 * \param[out]    dest  Destination string.
 * \param[in]     src   Source string. Needs to be '\0' terminated.
 * \param[in]     n     Maximum amount of characters available in destination
 *                      string.
 *
 * \return     Number of characters written to destination string.
 ******************************************************************************/
LOGGER_INLINE size_t logger_string_copy(char       *dest,
                                        const char *src,
                                        size_t     n)
{
  size_t index;

  for (index = 0 ; index < n && src[index] != '\0' ; index++) {
    dest[index] = src[index];
  }

  if (index < n) {
    dest[index] = '\0';
  }

  return(index);
}


/***************************************************************************//**
 * \brief  String copy
 *
 * Copies string from `src` to `dest` until end on `src` is reached. Fill the
 * given width of string with spaces and append the prefix field separator. The
 * function returns the number of characters copied to `dest.
 *
 * This function is used to generate aligned prefixes of logger messages.
 *
 * @attention The returned string is not terminated with a '\0' character! This
 *            needs to be done externally
 *
 * \param[out]    dest       Destination string.
 * \param[in]     dest_end   Pointer to character after the absolute end of
 *                           destination string. Has nothing to do with width.
 * \param[in]     src        Source string. Needs to be '\0' terminated.
 * \param[in]     width      Width of field in destination string.
 *
 * \return     Number of characters written to destination string.
 ******************************************************************************/
LOGGER_INLINE size_t logger_prefix_concatenate(char       *dest,
                                               const char *dest_end,
                                               const char *src,
                                               size_t     width)
{
  size_t characters = 0;

  /* copy src to dest */
  while (dest != dest_end &&
         src != NULL &&
         *src != '\0') {
    *(dest++) = *(src++);
    characters++;
  }

  /* fill string with spaces */
  while (dest != dest_end &&
         characters < width) {
    *(dest++) = ' ';
    characters++;
  }

  /* append separator */
  if (dest != dest_end) {
    *(dest++) = LOGGER_FORMAT_STRING_SEPARATOR;
    characters++;
  }

  return characters;
}


/***************************************************************************//**
 * \brief  Format log message.
 *
 * Print message into provides memory.
 *
 * \param[in]     id            ID outputting this message.
 * \param[out]    message       Formatted message.
 * \param[in]     message_size  String length of formatted message.
 * \param[in]     format        \c printf() like format string.
 * \param[in]     argp          Argument list.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
LOGGER_INLINE logger_return_t logger_format_message(logger_id_t id,
                                                    char        *message,
                                                    uint16_t    message_size,
                                                    const char  *format,
                                                    va_list     argp)
{
  int16_t  characters = 0;
  size_t   rev_idx;
  char     *message_end;

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

  return(LOGGER_OK);
}


/***************************************************************************//**
 * \brief  Output log messages to streams
 *
 * Print the log message to all outputs registered. It is possible to do
 * repeated prints to the same line by omitting '\n' in the log message format
 * sting. In this case a subsequent call will be appended without prefix. Only
 * print the message if
 *
 *   - logging is globally enabled.
 *   - logging ID is enabled.
 *   - logging level is enabled.
 *
 * \param[in]     id        ID outputting this message.
 * \param[in]     level     Level of this message.
 * \param[in]     outputs   List of logger outputs.
 * \param[in]     size      Number of elements in output list.
 * \param[in]     prefix    Formatted message prefix.
 * \param[in]     message   Formatted message.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
LOGGER_INLINE logger_return_t logger_output(logger_id_t     id,
                                            logger_level_t  level,
                                            logger_output_t *outputs,
                                            size_t          size,
                                            const char      *prefix,
                                            const char      *message)
{
  size_t                index;
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
        ((outputs[index].level & level) != 0)) {
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
            prefix_color             = &logger_level_colors[logger_level_to_index(level)];

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
                prefix_color             = &logger_level_colors[logger_level_to_index(level)];

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

      /* initialize variables */
      line      = logger_line;
      line_size = sizeof(logger_line);

      if (prefix_color_print_begin == logger_true) {
        line_characters = logger_string_copy(line, prefix_color->begin, line_size);
        line           += line_characters;
        line_size      -= line_characters;
      }

      /* actually output prefix */
      line_characters = logger_string_copy(line, prefix, line_size);
      line           += line_characters;
      line_size      -= line_characters;

      if (prefix_color_print_end == logger_true) {
        line_characters = logger_string_copy(line, prefix_color->end, line_size);
        line           += line_characters;
        line_size      -= line_characters;
      }

      if (message_color_print_begin == logger_true) {
        line_characters = logger_string_copy(line, message_color->begin, line_size);
        line           += line_characters;
        line_size      -= line_characters;
      }

      /* actually output message */
      line_characters = logger_string_copy(line, message, line_size);
      line           += line_characters;
      line_size      -= line_characters;

      if (message_color_print_end == logger_true) {
        line_characters = logger_string_copy(line, message_color->end, line_size);
        line           += line_characters;
        line_size      -= line_characters;
      }

      /* print '\n' if needed. color reset needs to be printed before '\n', otherwise some terminals show wrong colors in next line */
      if (logger_control[id].append == logger_false) {
        line_characters = logger_string_copy(line, "\n", line_size);
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
          (void)fputs(logger_line, outputs[index].stream);
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

  return(LOGGER_OK);
}


/***************************************************************************//**
 * \brief  Print repeat message.
 *
 * When a message repeats several times the message is only printed once by
 * logger_implementation_common(). When a different message should be printed
 * or outputs change this function will print the number of repeats.
 ******************************************************************************/
LOGGER_INLINE void logger_repeat_message(void)
{
  if (logger_repeat.count > 0) {
    char repeat_prefix[LOGGER_PREFIX_STRING_MAX];
    char repeat_message[LOGGER_MESSAGE_STRING_MAX];

    /* generate an empty prefix */
    memset(repeat_prefix, ' ', logger_repeat.prefix_length);
    repeat_prefix[logger_repeat.prefix_length] = '\0';

    /* generate string that contains number of repeats */
    if (logger_repeat.count == 1) {
      (void)snprintf(repeat_message, sizeof(repeat_message), "  -> previous message repeated %" PRIu16 " more time", logger_repeat.count);
    }
    else {
      (void)snprintf(repeat_message, sizeof(repeat_message), "  -> previous message repeated %" PRIu16 " more times", logger_repeat.count);
    }

    /* output message */
    (void)logger_output(logger_repeat.id, logger_repeat.level, logger_control[logger_repeat.id].unified_outputs, LOGGER_ELEMENTS(logger_control[logger_repeat.id].unified_outputs), repeat_prefix, repeat_message);

    /* reset repeat members */
    logger_repeat.count = 0;
    logger_repeat.id    = logger_id_unknown;
    logger_repeat.level = LOGGER_UNKNOWN;
  }
}


/***************************************************************************//**
 * \brief  Print rate limit message message.
 *
 * When a message is not printed because of a rate limit print the number of
 * skipped messages.
 ******************************************************************************/
LOGGER_INLINE void logger_rate_limit_message(logger_bool_t force)
{
  /* check if message should be printed */
  if ((logger_limit.message_request == logger_true) ||
      ((force == logger_true) && (logger_limit.skipped > 0))) {
    char limit_prefix[LOGGER_PREFIX_STRING_MAX];
    char limit_message[LOGGER_MESSAGE_STRING_MAX];

    /* generate system empty prefix */
    strncpy(limit_prefix, "LOGGER SYSTEM MESSAGE: ", LOGGER_ELEMENTS(limit_prefix));

    /* generate string that contains number of repeats */
    if (logger_limit.skipped == 1) {
      (void)snprintf(limit_message, sizeof(limit_message), "%" PRIu16 " message skipped because of rate limit (%" PRIu16 " messages per second)", logger_limit.skipped, logger_limit.limit);
    }
    else {
      (void)snprintf(limit_message, sizeof(limit_message), "%" PRIu16 " messages skipped because of rate limit (%" PRIu16 " messages per second)", logger_limit.skipped, logger_limit.limit);
    }

    /* output message */
    (void)logger_output(logger_system_id, LOGGER_EMERG, logger_control[logger_system_id].unified_outputs, LOGGER_ELEMENTS(logger_control[logger_system_id].unified_outputs), limit_prefix, limit_message);

    /* reset message request */
    logger_limit.message_request = logger_false;

    /* reset skip counter */
    logger_limit.skipped = 0;
  }
}


/***************************************************************************//**
 * \brief  Check if rate limit is exceeded.
 *
 * Check if rate limit is exceeded and return `logger_true` in that case.
 * Additionally request a rate limit message.
 *
 * \return        \c logger_true if rate limit is exceeded.
 ******************************************************************************/
LOGGER_INLINE logger_bool_t logger_check_rate_limit(void)
{
  static time_t previous_time = 0;
  time_t        current_time;
  time_t        diff_time;

  /* get current time */
  current_time = time(NULL);

  /* time difference */
  diff_time = current_time - previous_time;

  /* check if time has changed */
  if (diff_time != 0) {
    if (logger_limit.exceeded == logger_true) {
      /* request message printing */
      logger_limit.message_request = logger_true;

      /* reset counter */
      logger_limit.counter = 0;
    }

    /* update previous time */
    previous_time = current_time;
  }

  /* increment counter */
  logger_limit.counter++;

  /* check if limit is exceeded */
  if ((logger_limit.limit > 0) &&
      ((logger_limit.counter / (diff_time + 1)) > logger_limit.limit)) {
    logger_limit.exceeded = logger_true;

      /* add number of skipped messages */
      logger_limit.skipped++;

  }
  else {
    logger_limit.exceeded = logger_false;
  }

  return(logger_limit.exceeded);
}


/***************************************************************************//**
 * \brief  Print log message.
 *
 * Print the log message to all outputs registered using a printf()-like format
 * string and variable argument list. It is possible to do repeated prints to
 * the same line by omitting '\n' in the log message format sting. In this case
 * a subsequent call will be appended without prefix. Only print the message if
 *
 *   - logging is globally enabled.
 *   - logging ID is enabled.
 *   - logging level is enabled in the logging level of the ID.
 *   - logging level is enabled in the logging level of a output.
 *
 * \param[in]     id        ID outputting this message.
 * \param[in]     level     Level of this message.
 * \param[in]     file      Name of file where this call happend.
 * \param[in]     function  Name of function where this call happend.
 * \param[in]     line      Line where this call happend.
 * \param[in]     format    \c printf() like format string.
 * \param[in]     argp      variable argument list pointer
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
LOGGER_INLINE logger_return_t logger_implementation_common(logger_id_t    id,
                                                           logger_level_t level,
                                                           const char     *file,
                                                           const char     *function,
                                                           uint32_t       line,
                                                           const char     *format,
                                                           va_list        argp)
{
  /* GUARD: check for valid ID */
  if ((id < 0) ||
      (id == logger_system_id) ||
      (id >= LOGGER_ELEMENTS(logger_control)) ||
      (logger_control[id].used == logger_false)) {
    return(LOGGER_ERR_ID_UNKNOWN);
  }

  /* GUARD: check for valid level */
  if ((level & ~LOGGER_ALL) != 0) {
    return(LOGGER_ERR_LEVEL_UNKNOWN);
  }

  /* GUARD: check for valid file */
  if (file == NULL) {
    return(LOGGER_ERR_FILE_INVALID);
  }

  /* GUARD: check for valid function */
  if (function == NULL) {
    return(LOGGER_ERR_FUNCTION_INVALID);
  }

  /* GUARD: check for valid format */
  if (format == NULL) {
    return(LOGGER_ERR_FORMAT_INVALID);
  }

  /* GUARD: check rate limit */
  if (logger_check_rate_limit() == logger_true) {
    return(LOGGER_ERR_RATE_LIMIT);
  }

  /* check if ID is enabled and level is enabled */
  if ((logger_enabled == logger_true) &&
      (logger_control[id].enabled == logger_true) &&
      ((logger_control[id].level & level) != 0)) {

    /* format date */
    (void)logger_format_date(logger_date, sizeof(logger_date));

    /* format prefix */
    (void)logger_format_prefix(id, logger_prefix, sizeof(logger_prefix), level, file, function, line);

    /* format message */
    (void)logger_format_message(id, logger_message, sizeof(logger_message), format, argp);

    /* check if message is the same as previous message */
    if ((strncmp(logger_message, logger_repeat.message, sizeof(logger_message)) == 0) &&
        (id == logger_repeat.id) &&
        (level == logger_repeat.level)) {
      /* increase repeat counter */
      logger_repeat.count++;

      /* don't output message */
    }
    else {
      char *message_part;
      char *message_end;

      /* store id and level */
      logger_repeat.id = id;
      logger_repeat.level = level;

      /* calculate length of prefix string */
      logger_repeat.prefix_length = strlen(logger_prefix);

      /* copy new message to storage */
      logger_string_copy(logger_repeat.message, logger_message, sizeof(logger_repeat.message));

      /* output repeat message */
      logger_repeat_message();

      /* output rate limit message */
      logger_rate_limit_message(logger_false);

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

        /* output message to id unified outputs */
        (void)logger_output(id, level, logger_control[id].unified_outputs, LOGGER_ELEMENTS(logger_control[logger_system_id].unified_outputs), logger_prefix, message_part);

        /* update message part for next loop */
        message_part = message_end;
      }
      while (message_part != NULL);
    }
  }

  return(LOGGER_OK);
}


/***************************************************************************//**
 * \brief  Print log message.
 *
 * Print the log message to all outputs registered using a printf()-like format
 * string and variable argument list. It is possible to do repeated prints to
 * the same line by omitting '\n' in the log message format sting. In this case
 * a subsequent call will be appended without prefix. Only print the message if
 *
 *   - logging is globally enabled.
 *   - logging ID is enabled.
 *   - logging level is enabled in the logging level of the ID.
 *   - logging level is enabled in the logging level of a output.
 *
 * \param[in]     id        ID outputting this message.
 * \param[in]     level     Level of this message.
 * \param[in]     file      Name of file where this call happend.
 * \param[in]     function  Name of function where this call happend.
 * \param[in]     line      Line where this call happend.
 * \param[in]     format    \c printf() like format string.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_implementation(logger_id_t    id,
                                      logger_level_t level,
                                      const char     *file,
                                      const char     *function,
                                      uint32_t       line,
                                      const char     *format,
                                      ...)
{
  logger_return_t ret;
  va_list         argp;

  va_start(argp, format);
  ret = logger_implementation_common(id, level, file, function, line, format, argp);
  va_end(argp);

  return(ret);
}


/***************************************************************************//**
 * \brief  Print log message with variable argument list.
 *
 * Print the log message to all outputs registered using a printf()-like format
 * string and variable argument list. It is possible to do repeated prints to
 * the same line by omitting '\n' in the log message format sting. In this case
 * a subsequent call will be appended without prefix. Only print the message if
 *
 *   - logging is globally enabled.
 *   - logging ID is enabled.
 *   - logging level is enabled in the logging level of the ID.
 *   - logging level is enabled in the logging level of a output.
 *
 * \param[in]     id        ID outputting this message.
 * \param[in]     level     Level of this message.
 * \param[in]     file      Name of file where this call happend.
 * \param[in]     function  Name of function where this call happend.
 * \param[in]     line      Line where this call happend.
 * \param[in]     format    \c printf() like format string.
 * \param[in]     argp      variable argument list pointer
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_implementation_va(logger_id_t    id,
                                         logger_level_t level,
                                         const char     *file,
                                         const char     *function,
                                         uint32_t       line,
                                         const char     *format,
                                         va_list        argp)
{
  logger_return_t ret;

  ret = logger_implementation_common(id, level, file, function, line, format, argp);

  return(ret);
}


/* helper functions */


/***************************************************************************//**
 * \brief  Convert logger levels to index for level arrays.
 *
 * Convert logger levels to an index that can be used to access a level based
 * array like level specific colors or level to name conversion.
 *
 * <table border>
 * <tr>  <th> level             </th>  <th> index </th>  </tr>
 * <tr>  <td> \c LOGGER_UNKNOWN </td>  <td> 0     </td>  </tr>
 * <tr>  <td> \c LOGGER_DEBUG   </td>  <td> 1     </td>  </tr>
 * <tr>  <td> \c LOGGER_INFO    </td>  <td> 2     </td>  </tr>
 * <tr>  <td> \c LOGGER_NOTICE  </td>  <td> 3     </td>  </tr>
 * <tr>  <td> \c LOGGER_WARNING </td>  <td> 4     </td>  </tr>
 * <tr>  <td> \c LOGGER_ERR     </td>  <td> 5     </td>  </tr>
 * <tr>  <td> \c LOGGER_CRIT    </td>  <td> 6     </td>  </tr>
 * <tr>  <td> \c LOGGER_ALERT   </td>  <td> 7     </td>  </tr>
 * <tr>  <td> \c LOGGER_EMERG   </td>  <td> 8     </td>  </tr>
 * <tr>  <td> \c LOGGER_ALL     </td>  <td> 0     </td>  </tr>
 * <tr>  <td> <i> other </i>    </td>  <td> 0     </td>  </tr>
 * </table>
 * \param[in]     level   Level to convert.
 *
 * \return        Index corresponding given level.
 ******************************************************************************/
size_t logger_level_to_index(const logger_level_t level)
{
  size_t index = 0;

  switch (level) {
    case LOGGER_UNKNOWN:
      index = 0;
      break;

    case LOGGER_DEBUG:
      index = 1;
      break;

    case LOGGER_INFO:
      index = 2;
      break;

    case LOGGER_NOTICE:
      index = 3;
      break;

    case LOGGER_WARNING:
      index = 4;
      break;

    case LOGGER_ERR:
      index = 5;
      break;

    case LOGGER_CRIT:
      index = 6;
      break;

    case LOGGER_ALERT:
      index = 7;
      break;

    case LOGGER_EMERG:
      index = 8;
      break;

    case LOGGER_ALL:
      index = 0;
      break;

    default:
      index = 0;
      break;
  }

  return(index);
}


/***************************************************************************//**
 * \brief  Convert logger level index to logger level.
 *
 * Convert logger level index to an level that can be used to set a level for
 * an ID or an output.
 *
 * <table border>
 * <tr>  <th> index          </th>  <th> level             </th>  </tr>
 * <tr>  <td> 0              </td>  <td> \c LOGGER_UNKNOWN </td>  </tr>
 * <tr>  <td> 1              </td>  <td> \c LOGGER_DEBUG   </td>  </tr>
 * <tr>  <td> 2              </td>  <td> \c LOGGER_INFO    </td>  </tr>
 * <tr>  <td> 3              </td>  <td> \c LOGGER_NOTICE  </td>  </tr>
 * <tr>  <td> 4              </td>  <td> \c LOGGER_WARNING </td>  </tr>
 * <tr>  <td> 5              </td>  <td> \c LOGGER_ERR     </td>  </tr>
 * <tr>  <td> 6              </td>  <td> \c LOGGER_CRIT    </td>  </tr>
 * <tr>  <td> 7              </td>  <td> \c LOGGER_ALERT   </td>  </tr>
 * <tr>  <td> 8              </td>  <td> \c LOGGER_EMERG   </td>  </tr>
 * <tr>  <td> <i> other </i> </td>  <td> \c LOGGER_UNKNOWN </td>  </tr>
 * </table>
 *
 * \param[in]     index   Level index to convert.
 *
 * \return        Level corresponding given level index.
 ******************************************************************************/
logger_level_t logger_index_to_level(const size_t index)
{
  logger_level_t level = LOGGER_UNKNOWN;

  switch (index) {
    case 0:
      level = LOGGER_UNKNOWN;
      break;

    case 1:
      level = LOGGER_DEBUG;
      break;

    case 2:
      level = LOGGER_INFO;
      break;

    case 3:
      level = LOGGER_NOTICE;
      break;

    case 4:
      level = LOGGER_WARNING;
      break;

    case 5:
      level = LOGGER_ERR;
      break;

    case 6:
      level = LOGGER_CRIT;
      break;

    case 7:
      level = LOGGER_ALERT;
      break;

    case 8:
      level = LOGGER_EMERG;
      break;

    default:
      level = LOGGER_UNKNOWN;
      break;
  }

  return(level);
}


/* legacy functions */


/***************************************************************************//**
 * \brief  *LEGACY FUNCTION* Change terminal text color and attributes.
 *
 * See logger_id_color_console_set().
 *
 * \param[in]     id      ID for setting level.
 * \param[in]     fg      Text foreground.
 * \param[in]     bg      Text background.
 * \param[in]     attr    Text attribute.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_id_color_set(const logger_id_t        id,
                                    const logger_text_fg_t   fg,
                                    const logger_text_bg_t   bg,
                                    const logger_text_attr_t attr)
{
  return(logger_id_color_console_set(id, fg, bg, attr));
}


/***************************************************************************//**
 * \brief  *LEGACY FUNCTION* Change terminal text color and attributes.
 *
 * See logger_id_color_console_set().
 *
 * \param[in]     id      ID for setting level.
 * \param[in]     fg      Text foreground.
 * \param[in]     bg      Text background.
 * \param[in]     attr    Text attribute.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_color_set(const logger_id_t        id,
                                 const logger_text_fg_t   fg,
                                 const logger_text_bg_t   bg,
                                 const logger_text_attr_t attr)
{
  return(logger_id_color_console_set(id, fg, bg, attr));
}


/***************************************************************************//**
 * \brief  *LEGACY FUNCTION* Reset terminal text color and attributes.
 *
 * Reset text color and attributes of given ID back to defaults. Use
 * logger_id_color_reset() instead!
 *
 * \param[in]     id      ID for setting level.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_color_reset(const logger_id_t id)
{
  return(logger_id_color_reset(id));
}


/***************************************************************************//**
 * \brief  *LEGACY FUNCTION* Change terminal text color and attributes for prefix.
 *
 * See logger_color_prefix_console_set()
 *
 * \param[in]     level   Level to set.
 * \param[in]     fg      Text foreground.
 * \param[in]     bg      Text background.
 * \param[in]     attr    Text attribute.
 *
 * \return        \c LOGGER_OK if no error occurred, error code otherwise.
 ******************************************************************************/
logger_return_t logger_color_prefix_set(const logger_level_t     level,
                                        const logger_text_fg_t   fg,
                                        const logger_text_bg_t   bg,
                                        const logger_text_attr_t attr)
{
  return(logger_color_prefix_console_set(level, fg, bg, attr));
}


#endif /* LOGGER_ENABLE */
