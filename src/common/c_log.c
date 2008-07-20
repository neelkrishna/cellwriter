/******************************************************************************\
 CellWriter (GPL) - Copyright (C) 2008 - Michael Levin

 CellWriter is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at your option) any later
 version.

 CellWriter is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 CellWriter. If not, see <http://www.gnu.org/licenses/>.
\******************************************************************************/

/* This file implements the logging system */

#include "c_shared.h"

/* Path to log file or NULL */
char *c_log_filename;

/* Log print threshold */
c_log_level_t c_log_level = C_LOG_WARNING;

/* Special log handler */
c_log_event_f c_log_func;

/* Log blocking mode */
c_log_mode_t c_log_mode;

static FILE *log_file;

/******************************************************************************\
 Close the log file to conserve file handles.
\******************************************************************************/
void C_cleanup_log(void)
{
        if (!log_file)
                return;
        C_debug("Closing log-file");
        fclose(log_file);
}

/******************************************************************************\
 Wraps log-formatted text. [margin] amount of padding is added to each line
 following the first. If [plen] is not NULL, it is set to the length of the
 returned string.
\******************************************************************************/
char *C_wrap_log(const char *src, int margin, int wrap, int *plen)
{
        static char dest[320];
        int i, j, k, last_break, last_line, cols, char_len;

        /* Make sure the margin and wrap settings are sane */
        if (wrap < 20)
                wrap = 20;
        if (margin > wrap / 2)
                margin = 4;

        /* Take care of leading newlines here to prevent padding them */
        for (j = i = 0; src[i] == '\n'; i++)
                dest[j++] = '\n';

        for (last_break = last_line = cols = 0; ; i += char_len, cols++) {
                if (!(char_len = C_utf8_append(dest, &j, sizeof (dest) - 2,
                                               src + i)))
                        break;

                /* Track the last breakable spot */
                if (src[i] == ' ' || src[i] == '\t' || src[i] == '-' ||
                    src[i] == '/' || src[i] == '\\')
                        last_break = i;
                if (src[i] == '\n') {
                        last_break = i;
                        j--;
                }

                /* Wrap text and pad up until the margin length */
                if (cols >= wrap || src[i] == '\n') {
                        if (last_break == last_line)
                                last_break = i;
                        j -= i - last_break;
                        i = last_break;
                        if (j >= (int)sizeof (dest) - margin - 3)
                                break;
                        dest[j++] = '\n';
                        dest[j++] = ':';
                        for (k = 0; k < margin - 1; k++)
                                dest[j++] = ' ';
                        cols = margin;
                }
        }
        dest[j++] = '\n';
        dest[j] = NUL;
        if (plen)
                *plen = j;
        return dest;
}

/******************************************************************************\
 The actual logging function. Doesn't parse variable argument list.
\******************************************************************************/
static void log_write(c_log_level_t level, const char *function,
                      const char *message)
{
        const char *wrapped;
        int margin, len;
        char buffer[640];

        if (level > C_LOG_ERROR && level > c_log_level)
                return;

        /* Format the message depending on log level */
        if (c_log_level <= C_LOG_STATUS) {
                if (level >= C_LOG_STATUS) {
                        snprintf(buffer, sizeof(buffer), "%s", message);
                        margin = 0;
                } else if (level == C_LOG_WARNING) {
                        snprintf(buffer, sizeof(buffer), "* %s", message);
                        margin = 2;
                } else {
                        snprintf(buffer, sizeof(buffer), "*** %s", message);
                        margin = 4;
                }
        } else if (c_log_level == C_LOG_DEBUG) {
                char func_buf[64];

                margin = 2;

                /* We may or may not have a function depending on where this
                   message came from originally */
                func_buf[0] = NUL;
                if (function)
                        margin += snprintf(func_buf, sizeof (func_buf),
                                           "%s(): ", function);

                if (level >= C_LOG_DEBUG)
                        snprintf(buffer, sizeof(buffer), "| %s%s",
                                 func_buf, message);
                else if (level == C_LOG_STATUS)
                        snprintf(buffer, sizeof(buffer), "\n%s%s --",
                                 func_buf, message);
                else if (level == C_LOG_WARNING)
                        snprintf(buffer, sizeof(buffer), "* %s%s",
                                 func_buf, message);
                else {
                        snprintf(buffer, sizeof(buffer), "*** %s%s",
                                 func_buf, message);
                        margin += 2;
                }
        }

        /* Wrap and print the text */
        wrapped = C_wrap_log(buffer, margin, C_LOG_WRAP_COLS, &len);
        if (log_file)
                fwrite(wrapped, 1, len, log_file);
        else
                fputs(wrapped, stdout);

        /* Errors abort */
        if (level == C_LOG_ERROR)
                abort();

        /* Pass the (unwrapped) message to any special log handler */
        if (c_log_mode != C_LM_NORMAL)
                return;
        c_log_mode = C_LM_NO_FUNC;
        if (c_log_func)
                c_log_func(level, margin, buffer);
        if (c_log_mode == C_LM_NO_FUNC)
                c_log_mode = C_LM_NORMAL;
}

/******************************************************************************\
 Prints a string to the log file or to standard output. The output detail
 can be controlled using [c_log_level]. Debug calls without any text are
 considered traces.
\******************************************************************************/
void C_log(c_log_level_t level, const char *function, const char *fmt, ...)
{
        char buffer[640];
        va_list va;

        va_start(va, fmt);
        vsnprintf(buffer, sizeof (buffer), fmt, va);
        va_end(va);
        log_write(level, function, buffer);
}

/******************************************************************************\
 Log function used with GLib.
\******************************************************************************/
static void glib_log_func(const gchar *domain, GLogLevelFlags glevel,
                          const gchar *message)
{
        c_log_level_t level;

        /* Convert GLib log levels to ours */
        switch (glevel) {
        case G_LOG_LEVEL_ERROR:
                level = C_LOG_ERROR;
                break;
        case G_LOG_LEVEL_CRITICAL:
        case G_LOG_LEVEL_WARNING:
                level = C_LOG_WARNING;
                break;
        case G_LOG_LEVEL_MESSAGE:
        case G_LOG_LEVEL_INFO:
                level = C_LOG_STATUS;
                break;
        default:
                level = C_LOG_DEBUG;
                break;
        }
        log_write(level, NULL, message);
}

/******************************************************************************\
 Opens the log file or falls back to standard error output.
\******************************************************************************/
void C_init_log(void)
{
        /* Setup GTK log handler */
        g_log_set_handler(NULL, -1, (GLogFunc)glib_log_func, NULL);

        /* Open log file */
        if (!c_log_filename || !c_log_filename[0])
                return;
        log_file = fopen(c_log_filename, "w");
        if (!log_file) {
                C_warning("Failed to open log file '%s'", c_log_filename);
                return;
        }
        C_debug("Opened log file '%s'", c_log_filename);
}

/******************************************************************************\
 If [boolean] is FALSE then generates an error. The error message contains the
 actual assertion statement and source location information.
\******************************************************************************/
void C_assert_full(const char *function, int boolean, const char *message)
{
        if (!boolean)
                return;
        C_error_full(function, "Assertion failed: %s", message);
}

