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

/* Functions and structures that allocate or manage generic memory. */

#include "c_shared.h"

/* When memory checking is enabled, this structure is prepended to every
   allocated block. There is also a no-mans-land chunk, filled with a specific
   byte, at the end of every allocated block as well. */
#define NO_MANS_LAND_BYTE 0x5a
#define NO_MANS_LAND_SIZE 64
typedef struct c_mem_tag {
        struct c_mem_tag *next;
        const char *alloc_func, *free_func;
        void *data;
        size_t size;
        int freed;
        char no_mans_land[NO_MANS_LAND_SIZE];
} c_mem_tag_t;

int c_mem_check;

static c_mem_tag_t *mem_root;
static size_t mem_bytes, mem_bytes_max;
static int mem_calls;

/******************************************************************************\
 Allocates new memory, similar to realloc_checked().
\******************************************************************************/
static void *malloc_checked(const char *function, size_t size)
{
        c_mem_tag_t *tag;
        size_t real_size;

        real_size = size + sizeof (c_mem_tag_t) + NO_MANS_LAND_SIZE;
        tag = malloc(real_size);
        tag->data = (char *)tag + sizeof (c_mem_tag_t);
        tag->size = size;
        tag->alloc_func = function;
        tag->freed = FALSE;
        memset(tag->no_mans_land, NO_MANS_LAND_BYTE, NO_MANS_LAND_SIZE);
        memset((char *)tag->data + size, NO_MANS_LAND_BYTE, NO_MANS_LAND_SIZE);
        tag->next = NULL;
        if (mem_root)
                tag->next = mem_root;
        mem_root = tag;
        mem_bytes += size;
        mem_calls++;
        if (mem_bytes > mem_bytes_max)
                mem_bytes_max = mem_bytes;
        return tag->data;
}

/******************************************************************************\
 Finds the memory tag that holds [ptr].
\******************************************************************************/
static c_mem_tag_t *find_tag(const void *ptr, c_mem_tag_t **prev_tag)
{
        c_mem_tag_t *tag, *prev;

        prev = NULL;
        tag = mem_root;
        while (tag && tag->data != ptr) {
                prev = tag;
                tag = tag->next;
        }
        if (prev_tag)
                *prev_tag = prev;
        return tag;
}

/******************************************************************************\
 Reallocate [ptr] to [size] bytes large. Abort on error. String all the
 allocated chunks into a linked list and tracks information about the
 memory and where it was allocated from. This is used later in C_free() and
 C_check_leaks() to detect various errors.
\******************************************************************************/
static void *realloc_checked(const char *function, void *ptr, size_t size)
{
        c_mem_tag_t *tag, *prev_tag, *old_tag;
        size_t real_size;

        if (!ptr)
                return malloc_checked(function, size);
        if (!(tag = find_tag(ptr, &prev_tag)))
                C_error_full(function,
                             "Trying to reallocate unallocated address (0x%x)",
                             ptr);
        old_tag = tag;
        real_size = size + sizeof (c_mem_tag_t) + NO_MANS_LAND_SIZE;
        tag = realloc((char *)ptr - sizeof (c_mem_tag_t), real_size);
        if (!tag)
                C_error("Out of memory, %s() tried to allocate %d bytes",
                        function, size );
        if (prev_tag)
                prev_tag->next = tag;
        if (old_tag == mem_root)
                mem_root = tag;
        mem_bytes += size - tag->size;
        if (size > tag->size) {
                mem_calls++;
                if (mem_bytes > mem_bytes_max)
                        mem_bytes_max = mem_bytes;
        }
        tag->size = size;
        tag->alloc_func = function;
        tag->data = (char *)tag + sizeof (c_mem_tag_t);
        memset((char *)tag->data + size, NO_MANS_LAND_BYTE, NO_MANS_LAND_SIZE);
        return tag->data;
}

/******************************************************************************\
 Reallocate [ptr] to [size] bytes large. Abort on error. When memory checking
 is enabled, this calls realloc_checked() instead.
\******************************************************************************/
void *C_realloc_full(const char *function, void *ptr, size_t size)
{
        if (c_mem_check)
                return realloc_checked(function, ptr, size);
        ptr = realloc(ptr, size);
        if (!ptr)
                C_error_full(function,
                             "Out of memory, tried to allocate %u bytes",
                             size);
        return ptr;
}

/******************************************************************************\
 Checks if a no-mans-land region has been corrupted.
\******************************************************************************/
static int check_no_mans_land(const char *ptr)
{
        int i;

        for (i = 0; i < NO_MANS_LAND_SIZE; i++)
                if (ptr[i] != NO_MANS_LAND_BYTE)
                        return FALSE;
        return TRUE;
}

/******************************************************************************\
 Frees memory. If memory checking is enabled, will check the following:
 - [ptr] was never allocated
 - [ptr] was already freed
 - [ptr] no-mans-land (upper or lower) was corrupted
\******************************************************************************/
void C_free_full(const char *function, void *ptr)
{
        c_mem_tag_t *tag, *prev_tag, *old_tag;

        if (!c_mem_check) {
                free(ptr);
                return;
        }
        if (!ptr)
                return;
        if (!(tag = find_tag(ptr, &prev_tag)))
                C_error_full(function,
                             "Trying to free unallocated address (0x%x)", ptr);
        if (tag->freed)
                C_error_full(function,
                             "Address (0x%x), %d bytes allocated by "
                             "%s(), already freed by %s()",
                             ptr, tag->size, tag->alloc_func, tag->free_func);
        if (!check_no_mans_land(tag->no_mans_land))
                C_error_full(function,
                             "Address (0x%x), %d bytes allocated by "
                             "%s(), overran lower boundary",
                             ptr, tag->size, tag->alloc_func);
        if (!check_no_mans_land((char *)ptr + tag->size))
                C_error_full(function,
                             "Address (0x%x), %d bytes allocated by "
                             "%s(), overran upper boundary",
                             ptr, tag->size, tag->alloc_func);
        tag->freed = TRUE;
        tag->free_func = function;
        old_tag = tag;
        tag = realloc(tag, sizeof (*tag));
        if (prev_tag)
                prev_tag->next = tag;
        if (old_tag == mem_root)
                mem_root = tag;
        mem_bytes -= tag->size;
}

/******************************************************************************\
 If memory checking is enabled, checks for memory leaks and prints warnings.
\******************************************************************************/
void C_check_leaks(void)
{
        c_mem_tag_t *tag;
        int tags;

        if (!c_mem_check)
                return;
        tags = 0;
        for (tag = mem_root; tag; tag = tag->next) {
                unsigned int i;

                tags++;
                if (tag->freed)
                        continue;
                C_warning("%s() leaked %d bytes", tag->alloc_func, tag->size);

                /* If we leaked a string, we can print it */
                if (tag->size < 1)
                        continue;
                for (i = 0; C_is_print(((char *)tag->data)[i]); i++) {
                        char buf[128];

                        if (i >= tag->size - 1 || i >= sizeof (buf) - 1 ||
                            !((char *)tag->data)[i + 1]) {
                                C_strncpy_buf(buf, tag->data);
                                C_debug("Looks like a string: '%s'", buf);
                                break;
                        }
                }
        }
        C_debug("%d allocation calls, high mark %.1fmb, %d tags",
                mem_calls, mem_bytes_max / 1048576.f, tags);
}

/******************************************************************************\
 Allocate zero'd memory.
\******************************************************************************/
void *C_recalloc_full(const char *function, void *ptr, size_t size)
{
        ptr = C_realloc_full(function, ptr, size);
        memset(ptr, 0, size);
        return ptr;
}

