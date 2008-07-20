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

/* Vector type declarations and static inline definitions for the vector
   operations. Requires math.h to be included. */

typedef struct {
        float x, y;
} c_vec2_t;

/******************************************************************************\
 Constructor.
\******************************************************************************/
static inline c_vec2_t C_vec2(float x, float y)
{
        c_vec2_t result = { x, y };
        return result;
}

/******************************************************************************\
 Component-wise binary operators with another vector.
\******************************************************************************/
static inline c_vec2_t C_vec2_add(c_vec2_t a, c_vec2_t b)
{
        return C_vec2(a.x + b.x, a.y + b.y);
}

static inline c_vec2_t C_vec2_sub(c_vec2_t a, c_vec2_t b)
{
        return C_vec2(a.x - b.x, a.y - b.y);
}

static inline c_vec2_t C_vec2_scale(c_vec2_t a, c_vec2_t b)
{
        return C_vec2(a.x * b.x, a.y * b.y);
}

static inline c_vec2_t C_vec2_div(c_vec2_t a, c_vec2_t b)
{
        return C_vec2(a.x / b.x, a.y / b.y);
}

/******************************************************************************\
 Binary operators with a scalar.
\******************************************************************************/
static inline c_vec2_t C_vec2_addf(c_vec2_t a, float f)
{
        return C_vec2(a.x + f, a.y + f);
}

static inline c_vec2_t C_vec2_subf(c_vec2_t a, float f)
{
        return C_vec2(a.x - f, a.y - f);
}

static inline c_vec2_t C_vec2_scalef(c_vec2_t a, float f)
{
        return C_vec2(a.x * f, a.y * f);
}

static inline c_vec2_t C_vec2_divf(c_vec2_t a, float f)
{
        return C_vec2(a.x / f, a.y / f);
}

/******************************************************************************\
 Vector dot and cross products.
\******************************************************************************/
static inline float C_vec2_dot(c_vec2_t a, c_vec2_t b)
{
        return a.x * b.x + a.y * b.y;
}

static inline float C_vec2_cross(c_vec2_t a, c_vec2_t b)
{
        return a.y * b.x - a.x * b.y;
}

/******************************************************************************\
 Vector dimensions squared and summed. That is, magnitude without the square
 root operation.
\******************************************************************************/
static inline float C_vec2_square(c_vec2_t p)
{
        return p.x * p.x + p.y * p.y;
}

/******************************************************************************\
 Vector magnitude/length.
\******************************************************************************/
static inline float C_vec2_len(c_vec2_t p)
{
        return (float)sqrt(C_vec2_square(p));
}

/******************************************************************************\
 Vector normalization.
\******************************************************************************/
static inline c_vec2_t C_vec2_norm(c_vec2_t p)
{
        return C_vec2_divf(p, C_vec2_len(p));
}

/******************************************************************************\
 Vector comparison.
\******************************************************************************/
static inline int C_vec2_eq(c_vec2_t a, c_vec2_t b)
{
        return a.x == b.x && a.y == b.y;
}

/******************************************************************************\
 Vector interpolation.
\******************************************************************************/
static inline c_vec2_t C_vec2_lerp(c_vec2_t a, float lerp, c_vec2_t b)
{
        return C_vec2(a.x + lerp * (b.x - a.x), a.y + lerp * (b.y - a.y));
}

/******************************************************************************\
 Truncate vector values down to whole numbers. Negative values are also
 truncated down (-2.1 is rounded to -3).
\******************************************************************************/
static inline float C_clamp(float value, float unit)
{
        if (value < 0)
                return (int)(value * -unit) / -unit;
        return (int)(value * unit) / unit;
}

static inline c_vec2_t C_vec2_clamp(c_vec2_t v, float u)
{
        return C_vec2(C_clamp(v.x, u), C_clamp(v.y, u));
}

/******************************************************************************\
 Returns the vector with absolute valued members.
\******************************************************************************/
static inline c_vec2_t C_vec2_abs(c_vec2_t v)
{
        if (v.x < 0.f)
                v.x = -v.x;
        if (v.y < 0.f)
                v.y = -v.y;
        return v;
}

/******************************************************************************\
 Returns TRUE if the two rectangles intersect.
\******************************************************************************/
static inline int C_rect_intersect(c_vec2_t o1, c_vec2_t s1,
                                   c_vec2_t o2, c_vec2_t s2)
{
        return o1.x <= o2.x + s2.x && o1.y <= o2.y + s2.y &&
               o1.x + s1.x >= o2.x && o1.y + s2.y >= o2.y;
}

