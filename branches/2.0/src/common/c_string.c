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

/* Classes and functions that manipulate strings. */

#include "c_shared.h"

/******************************************************************************\
 These functions parse variable argument lists into a static buffer and return
 a pointer to it. You can also pass a pointer to an integer to get the length
 of the output. Not thread safe, but very convenient. Note that after repeated
 calls to these functions, C_vanv will eventually run out of buffers and start
 overwriting old ones. The idea for these functions comes from the Quake 3
 source code.
\******************************************************************************/
char *C_vanv(int *plen, const char *fmt, va_list va)
{
	static char buffer[C_VA_BUFFERS][C_VA_BUFFER_SIZE];
	static int which;
	int len;

	which++;
	if (which >= C_VA_BUFFERS)
	        which = 0;
	len = vsnprintf(buffer[which], sizeof (buffer[which]), fmt, va);
	if (plen)
		*plen = len;
	return buffer[which];
}

char *C_van(int *plen, const char *fmt, ...)
{
	va_list va;
	char *string;

	va_start(va, fmt);
	string = C_vanv(plen, fmt, va);
	va_end(va);
	return string;
}

char *C_va(const char *fmt, ...)
{
	va_list va;
	char *string;

	va_start(va, fmt);
	string = C_vanv(NULL, fmt, va);
	va_end(va);
	return string;
}

/******************************************************************************\
 Skips any space characters in the string.
\******************************************************************************/
char *C_skip_spaces(const char *str)
{
        char ch;

        ch = *str;
        while (C_is_space(ch))
                ch = *(++str);
        return (char *)str;
}

/******************************************************************************\
 Equivalent to the standard library strncpy, but ensures that there is always
 a NUL terminator. Sometimes known as "strncpyz". Can copy overlapped strings.
 Returns the length of the source string.
\******************************************************************************/
int C_strncpy(char *dest, const char *src, int len)
{
        int src_len;

        if (!dest)
                return 0;
        if (!src) {
                if (len > 0)
                        dest[0] = NUL;
                return 0;
        }
        src_len = (int)strlen(src);
        if (src_len > --len) {
                C_debug("dest (%d bytes) too short to hold src (%d bytes)",
                        len, src_len, src);
                src_len = len;
        }
        memmove(dest, src, src_len);
        dest[src_len] = NUL;
        return src_len;
}

/******************************************************************************\
 Equivalent to the standard library strlen but returns zero if the string is
 NULL.
\******************************************************************************/
int C_strlen(const char *string)
{
        if (!string)
                return 0;
        return (int)strlen(string);
}

/******************************************************************************\
 Allocates new heap memory for and duplicates a string. Doesn't crash if the
 string passed in is NULL.
\******************************************************************************/
char *C_strdup_full(const char *function, const char *str)
{
        char *new_str;
        size_t len;

        if (!str)
                return NULL;
        len = strlen(str) + 1;
        new_str = C_realloc_full(function, NULL, len);
        memcpy(new_str, str, len);
        return new_str;
}

/******************************************************************************\
 Wraps a string in double-quotes and escapes any special characters. Returns
 a statically allocated string.
\******************************************************************************/
char *C_escape_string(const char *str)
{
        static char buf[4096];
        char *pbuf;

        buf[0] = '"';
        for (pbuf = buf + 1; *str && pbuf <= buf + sizeof (buf) - 3; str++) {
                if (*str == '"' || *str == '\\')
                        *pbuf++ = '\\';
                else if (*str == '\n') {
                        *pbuf++ = '\\';
                        *pbuf = 'n';
                        continue;
                } else if(*str == '\t') {
                        *pbuf++ = '\\';
                        *pbuf = 't';
                        continue;
                } else if (*str == '\r')
                        continue;
                *pbuf++ = *str;
        }
        *pbuf++ = '"';
        *pbuf = NUL;
        return buf;
}

/******************************************************************************\
 Returns the number of bytes in a single UTF-8 character:
 http://www.cl.cam.ac.uk/%7Emgk25/unicode.html#utf-8
\******************************************************************************/
int C_utf8_size(unsigned char first_byte)
{
        /* U-00000000 – U-0000007F (ASCII) */
        if (first_byte < 192)
                return 1;

        /* U-00000080 – U-000007FF */
        else if (first_byte < 224)
                return 2;

        /* U-00000800 – U-0000FFFF */
        else if (first_byte < 240)
                return 3;

        /* U-00010000 – U-001FFFFF */
        else if (first_byte < 248)
                return 4;

        /* U-00200000 – U-03FFFFFF */
        else if (first_byte < 252)
                return 5;

        /* U-04000000 – U-7FFFFFFF */
        return 6;
}

/******************************************************************************\
 Returns the index of the [n]th UTF-8 character in [str].
\******************************************************************************/
int C_utf8_index(char *str, int n)
{
        int i, len;

        for (i = 0; n > 0; n--)
                for (len = C_utf8_size(str[i]); len > 0; len--, i++)
                        if (!str[i])
                                return i;
        return i;
}

/******************************************************************************\
 Equivalent to the standard library strlen but returns zero if the string is
 NULL. If [chars] is not NULL, the number of UTF-8 characters will be output
 to it. Note that if the number of UTF-8 characters is not needed, C_strlen()
 is preferrable to this function.
\******************************************************************************/
int C_utf8_strlen(const char *str, int *chars)
{
        int i, str_len, char_len;

        if (!str || !str[0]) {
                if (chars)
                        *chars = 0;
                return 0;
        }
        char_len = C_utf8_size(*str);
        for (i = 0, str_len = 1; str[i]; char_len--, i++)
                if (char_len < 1) {
                        str_len++;
                        char_len = C_utf8_size(str[i]);
                }
        if (chars)
                *chars = str_len;
        return i;
}

/******************************************************************************\
 One UTF-8 character from [src] will be copied to [dest]. The index of the
 current position in [dest], [dest_i] and the index in [src], [src_i], will
 be advanced accordingly. [dest] will not be allowed to overrun [dest_sz]
 bytes. Returns the size of the UTF-8 character or 0 if there is not enough
 room or if [src] starts with NUL.
\******************************************************************************/
int C_utf8_append(char *dest, int *dest_i, int dest_sz, const char *src)
{
        int len, char_len;

        if (!*src)
                return 0;
        char_len = C_utf8_size(*src);
        if (*dest_i + char_len > dest_sz)
                return 0;
        for (len = char_len; *src && len > 0; len--)
                dest[(*dest_i)++] = *src++;
        return char_len;
}

/******************************************************************************\
 Convert a Unicode token to a UTF-8 character sequence. The length of the
 token in bytes is output to [plen], which can be NULL.
\******************************************************************************/
char *C_utf8_encode(unsigned int unicode, int *plen)
{
        static char buf[7];
        int len;

        /* ASCII is an exception */
        if (unicode <= 0x7f) {
                buf[0] = unicode;
                buf[1] = NUL;
                if (plen)
                        *plen = 1;
                return buf;
        }

        /* Size of the sequence depends on the range */
        if (unicode <= 0xff)
                len = 2;
        else if (unicode <= 0xffff)
                len = 3;
        else if (unicode <= 0x1fffff)
                len = 4;
        else if (unicode <= 0x3FFFFFF)
                len = 5;
        else if (unicode <= 0x7FFFFFF)
                len = 6;
        else {
                C_warning("Invalid Unicode character 0x%x", unicode);
                buf[0] = NUL;
                if (plen)
                        *plen = 0;
                return buf;
        }
        if (plen)
                *plen = len;

        /* The first byte is 0b*110x* and the rest are 0b10xxxxxx */
        buf[0] = 0xfc << (6 - len);
        while (--len > 0) {
                buf[len] = 0x80 + (unicode & 0x3f);
                unicode >>= 6;
        }
        buf[0] += unicode;
        return buf;
}

/******************************************************************************\
 Returns TRUE if the string contains path-modifying characters such as slashes
 and lone or double dots.
\******************************************************************************/
int C_is_path(const char *s)
{
        if (!s)
                return FALSE;

        /* Single dot */
        if (!s[1] && s[0] == '.')
                return TRUE;

        /* Double dot */
        if (!s[2] && s[1] == '.' && s[0] == '.')
                return TRUE;

        /* Scan for POSIX or Windows slashes */
        for (; *s; s++)
                if (*s == '/' || *s == '\\')
                        return TRUE;

        return FALSE;
}

