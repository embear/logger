logger
======

Logger is a simple logging facility for the C language. It is possible to
output logging information to different output stream and to use different
logging severities.

Introduction
------------

Logger needs to be enabled using the global define `LOGGER_ENABLE` when
compiling the sources. Otherwise all functions will be excluded from
compilation.

Logger supports multiple IDs. A logger ID is requested with a call to
`logger_id_request()` and released with `logger_id_release()`. Multiple
calls to `logger_id_request()` with the same name as argument result in the
same ID. Thus it inherits all already done configurations. A ID needs to be
enabled by `logger_id_enable()` and could be disabled by
`logger_id_disable()`.

Each ID has its own severity which needs to be set by `logger_id_level_set()`.
Only calls to `logger()` with the same or higher severity are considered for
output. This way it is possible to enable logging only for parts of a program.
The function `logger_id_level_get()` can be used to get the current value,
store it, increase the level for some part of thew program and restore it
afterwards.

**By default an ID has no level assigned and thus generates no output until
a level is defined using `logger_id_level_set()`.**

Each ID has its own prefix assigned. By default each ID prints the prefix
`"LOGGER_ID_NAME:LOGGER_LEVEL:FUNCTION:LINE:"`. The prefix is changed by
`logger_id_prefix_set()` and it is possible to get the current value for later
restore with `logger_id_prefix_get()`. The prefix elements could be combined
using `|`. Available prefix elements are:

  Prefix element        | Description
  --------------------- | --------------------------------------------------------
  `LOGGER_PFX_EMPTY`    | `""`
  `LOGGER_PFX_DATE`     | `"DATE"`
  `LOGGER_PFX_NAME`     | `"LOGGER_ID_NAME:"`
  `LOGGER_PFX_LEVEL`    | `"LOGGER_LEVEL:"`
  `LOGGER_PFX_FILE`     | `"FILE_NAME:"`
  `LOGGER_PFX_FUNCTION` | `"FUNCTION_NAME:"`
  `LOGGER_PFX_LINE`     | `"LINE:"`
  `LOGGER_PFX_ALL`      | `"DATE:LOGGER_ID_NAME:LOGGER_LEVEL:FILE:FUNCTION:LINE:"`

For backward compatibility the following prefix enums are also supported:

  Prefix enum              | Generated prefix
  ------------------------ | ---------------------------------------------------
  `LOGGER_PREFIX_EMPTY`    | `""`
  `LOGGER_PREFIX_NAME`     | `"LOGGER_ID_NAME:"`
  `LOGGER_PREFIX_SHORT`    | `"LOGGER_ID_NAME:LOGGER_LEVEL:"`
  `LOGGER_PREFIX_FUNCTION` | `"LOGGER_ID_NAME:LOGGER_LEVEL:FUNCTION:LINE:"`
  `LOGGER_PREFIX_FILE`     | `"LOGGER_ID_NAME:LOGGER_LEVEL:FILE:LINE:"`
  `LOGGER_PREFIX_FULL`     | `"LOGGER_ID_NAME:LOGGER_LEVEL:FILE:FUNCTION:LINE:"`

It is possible to give each logger ID different foreground and background color
as well as text attributes with `logger_id_color_set()` and reset them with
`logger_id_color_reset()`. Colors are changed by escape sequences and only
work on `STDOUT` and `STDOUT`. File output does not contain any escape sequences by default.

To get any messages an output needs to be registered to logger. Any stream of
type `FILE` is supported and multiple outputs can be registered. Registration
is done with `logger_output_register()` deregistration with
`logger_output_deregister()`. To check if an output had already been registered
the function `logger_output_is_registered()` can be used. Like IDs every output
gets a severity. This makes it possible to write all messages with
`LOGGER_INFO` to a file and only output messages with `LOGGER_ERR` to `STDOUT`.
Setting the level ist done with `logger_output_level_set()`. As for IDs the
current value can be get with `logger_output_level_get()`. Additional it is
possible to enable and disable colors for each output using the functions
`logger_output_color_enable()` and `logger_output_color_disable()`. There is
also a function `logger_output_color_is_enabled()` to check the current state.
By default only `STDOUT` and `STDERR` use color. For other file streams color is
disabled. Beside the file stream output it is possible to register a user
defined callback function to handle the formatted message string. This is
useful for example to keep the messages in a ring buffer in memory or to show
them in a message window of a graphical toolkit. The functions
`logger_output_function_register()`, `logger_output_function_deregister()`,
`logger_output_function_is_registered()` `logger_output_function_level_set()`,
`logger_output_function_level_get()`, `logger_output_function_color_enable()`,
`logger_output_function_color_disable()`,
`logger_output_function_color_is_enabled()` can be used to control the function
callback output.

**By default an output has no level assigned and thus outputs no messages until
a level is defined using `logger_output_level_set()`.**

Additionally there are functions `logger_id_output_register()`,
`logger_id_output_deregister()`, `logger_id_output_is_registered()`,
`logger_id_output_level_set()`, `logger_id_output_level_get()`,
`logger_id_output_color_enable()`, `logger_id_output_disable()` and
`logger_id_output_color_is_enabled()` functions to handle ID specific file
stream outputs. There are also the user specific callback functions
`logger_id_output_function_register()`,
`logger_id_output_function_is_registered()`
`logger_id_output_function_deregister()`,
`logger_id_output_function_level_set()`,
`logger_id_output_function_level_get()`,
`logger_id_output_function_color_enable()`,
`logger_id_output_function_disable()` and
`logger_id_output_function_color_is_enabled()` available. This way it is
possible to write output of a specific subsystem to a separate file.

**By default an ID specific output has no level assigned and thus outputs no
messages until a level is defined using `logger_id_output_level_set()`.**

Keep in mind that a message is only printed if the severity of the
`logger()` call is higher or equal to the level of the according ID *AND* the
severity is higher or equal to the level of the output.

When logger is compiled with `LOGGER_FORCE_FLUSH` defined it forcibly flushes
every stream after every output. This will produce horribly bad performance but
might be helpful to debug aborting programs.

Example
-------

Below is a basic example for using logger. Store it as `main.c` and compile
with:

`gcc -DLOGGER_ENABLE -Iinclude -o main main.c src/logger.c`


```c
#include "logger.h"
#include <time.h>
#include <stdio.h>

static void printer(const char *string)
{
  char   date[200];
  time_t t;

  t = time(NULL);
  strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", localtime(&t));
  printf("%s: %s", date, string);
}


int main(void)
{
  FILE        *stream;
  logger_id_t id = logger_id_unknown;

  /* check logger version */
  if (LOGGER_VERSION != logger_version()) {
    return(1);
  }

  /* initialize logger */
  logger_init();

  /* open a file as output for all messages */
  stream = fopen("logfile", "w");
  logger_output_register(stream);
  logger_output_level_set(stream, LOGGER_DEBUG);

  /* open stdout as output for messages above LOGGER_ERR */
  logger_output_register(stdout);
  logger_output_level_set(stdout, LOGGER_ERR);

  /* register user function for messages above LOGGER_INFO */
  logger_output_function_register(printer);
  logger_output_function_level_set(printer, LOGGER_INFO);

  /* get a logging id, enable it and set log level */
  id = logger_id_request("logger_test_id");
  logger_id_enable(id);
  logger_id_level_set(id, LOGGER_INFO);
  logger_id_prefix_set(id, (LOGGER_PFX_DATE | LOGGER_PFX_NAME | LOGGER_PFX_LEVEL | LOGGER_PFX_FILE | LOGGER_PFX_FUNCTION | LOGGER_PFX_LINE));
  logger_color_prefix_enable();
  logger_color_message_enable();
  logger_color_set(id, LOGGER_FG_GREEN, LOGGER_BG_BLACK, LOGGER_ATTR_RESET);

  /* do the logging */
  logger(id, LOGGER_DEBUG,    "id %d - LOGGER_DEBUG   in line %d\n", id, __LINE__); /* nothing written */
  logger(id, LOGGER_INFO,     "id %d - LOGGER_INFO    in line %d\n", id, __LINE__); /* written to logfile */
  logger(id, LOGGER_NOTICE,   "id %d - LOGGER_NOTICE  in line %d\n", id, __LINE__); /* written to logfile */
  logger(id, LOGGER_WARNING,  "id %d - LOGGER_WARNING in line %d\n", id, __LINE__); /* written to logfile */
  logger(id, LOGGER_ERR,      "id %d - LOGGER_ERR     in line %d\n", id, __LINE__); /* written to logfile and stdout */
  logger(id, LOGGER_CRIT,     "id %d - LOGGER_CRIT    in line %d\n", id, __LINE__); /* written to logfile and stdout */
  logger(id, LOGGER_ALERT,    "id %d - LOGGER_ALERT   in line %d\n", id, __LINE__); /* written to logfile and stdout */
  logger(id, LOGGER_EMERG,    "id %d - LOGGER_EMERG   in line %d\n", id, __LINE__); /* written to logfile and stdout */

  /* release id */
  logger_id_release(id);

  /* deregister user function */
  logger_output_function_deregister(printer);

  /* deregister stdout output */
  logger_output_deregister(stdout);

  /* deregister file output */
  logger_output_deregister(stream);

  /* close file */
  fclose(stream);

  return(0);
}
```
