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

/* Configuration parameters */
#include "config.h"

/* Standard library */
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

/* Interface is written in GTK */
#include <gtk/gtk.h>

/* HCR library */
#include "hcr.h"

/* Vectors */
#include "c_vectors.h"

/* Ensure common definitions */
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef NULL
#define NULL ((void*)0)
#endif
#ifndef NUL
#define NUL '\0'
#endif
#ifndef INFINITY
#define INFINITY 2147483647
#endif

/* Log files are wrapped to this many columns */
#define C_LOG_WRAP_COLS 79

/* If you are going to use the C_va* functions, keep in mind that after calling
   any of those functions [C_VA_BUFFERS] times, you will begin overwriting
   the buffers starting from the first. Each buffer also has a fixed size.
   Note that [C_VA_BUFFERS] * [C_VA_BUFFER_SIZE] has to be less than 32k or
   the function will not compile on some systems. */
#define C_VA_BUFFERS 16
#define C_VA_BUFFER_SIZE 2000

/* Debug log levels, errors are fatal and will always abort */
typedef enum {
        C_LOG_ERROR = 0,
        C_LOG_WARNING,
        C_LOG_STATUS,
        C_LOG_DEBUG,
} c_log_level_t;

/* Log modes */
typedef enum {
        C_LM_NORMAL,
        C_LM_NO_FUNC,
        C_LM_CLEANUP,
} c_log_mode_t;

/* Define a compact boolean type. This actually does not make much of an impact
   as far as memory usage is concerned but is helpful to denote usage. */
typedef unsigned char bool;

/* Callback for GUI log handler */
typedef void (*c_log_event_f)(c_log_level_t, int margin, const char *);

/* c_log.c */
#define C_assert(s) C_assert_full(__func__, !(s), #s)
void C_assert_full(const char *function, int boolean, const char *message);
void C_cleanup_log(void);
#define C_debug(fmt, ...) C_log(C_LOG_DEBUG, __func__, fmt, ## __VA_ARGS__)
#define C_debug_full(fn, fmt, ...) C_log(C_LOG_DEBUG, fn, fmt, ## __VA_ARGS__)
#define C_error(fmt, ...) C_log(C_LOG_ERROR, __func__, fmt, ## __VA_ARGS__)
#define C_error_full(fn, fmt, ...) C_log(C_LOG_ERROR, fn, fmt, ## __VA_ARGS__)
void C_log(c_log_level_t level, const char *function, const char *fmt, ...);
void C_init_log(void);
#define C_status(fmt, ...) C_log(C_LOG_STATUS, __func__, fmt, ## __VA_ARGS__)
#define C_status_full(fn, fmt, ...) C_log(C_LOG_STATUS, fn, fmt, ## __VA_ARGS__)
#define C_warning(fmt, ...) C_log(C_LOG_WARNING, __func__, fmt, ## __VA_ARGS__)
#define C_warning_full(fn, fmt, ...) C_log(C_LOG_WARNING, fn, fmt, \
                                           ## __VA_ARGS__)
char *C_wrap_log(const char *, int margin, int wrap, int *plen);

extern c_log_event_f c_log_func;
extern c_log_mode_t c_log_mode;
extern c_log_level_t c_log_level;
extern char *c_log_filename;

/* c_memory.c */
#define C_calloc(s) C_recalloc_full(__func__, NULL, s)
void C_check_leaks(void);
#define C_free(p) C_free_full(__func__, p)
void C_free_full(const char *function, void *ptr);
#define C_malloc(s) C_realloc(NULL, s)
#define C_realloc(p, s) C_realloc_full(__func__, p, s)
void *C_realloc_full(const char *function, void *ptr, size_t size);
void *C_recalloc_full(const char *function, void *ptr, size_t size);
#define C_zero(s) memset(s, 0, sizeof (*(s)))
#define C_zero_buf(s) memset(s, 0, sizeof (s))

extern int c_mem_check;

/* c_string.c */
#define C_bool_string(b) ((b) ? "TRUE" : "FALSE")
char *C_escape_string(const char *);
#define C_is_digit(c) (((c) >= '0' && (c) <= '9') || (c) == '.' || (c) == '-')
int C_is_path(const char *);
#define C_is_print(c) ((c) > 0 && (c) < 0x7f)
#define C_is_space(c) ((c) > 0 && (c) <= ' ')
char *C_skip_spaces(const char *str);
#define C_strdup(s) C_strdup_full(__func__, s)
char *C_strdup_full(const char *func, const char *);
int C_strlen(const char *);
int C_strncpy(char *dest, const char *src, int len);
#define C_strncpy_buf(d, s) C_strncpy(d, s, sizeof (d))
int C_utf8_append(char *dest, int *dest_i, int dest_sz, const char *src);
char *C_utf8_encode(unsigned int unicode, int *len);
int C_utf8_index(char *str, int n);
int C_utf8_size(unsigned char first_byte);
int C_utf8_strlen(const char *, int *utf8_chars);
char *C_va(const char *fmt, ...);
char *C_van(int *output_len, const char *fmt, ...);
char *C_vanv(int *output_len, const char *fmt, va_list);

