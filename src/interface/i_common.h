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

#include "c_common.h"
#include "i_shared.h"

/* Maximum number of matches a cell accepts */
#define I_MATCHES 5

/* The number of columns the mini-keyboard takes up */
#define I_MINI_KEYS_COLS 4

/* Pixel border width between cells and mini-keyboard */
#define I_MINI_KEYS_BORDER 6

/* Colors */
typedef enum {
        I_C_ACTIVE,
        I_C_INACTIVE,
        I_C_INK,
        I_C_SELECT,
        I_COLORS
} i_color_t;

/* Structure for matches */
typedef struct i_match {
        hcr_sample_t sample;
        c_vec2_t offset;
        gunichar2 ch;
        float quality;
} i_match_t;

/* Structure for an input cell */
typedef struct i_cell {
        i_match_t matches[I_MATCHES];
        hcr_sample_t input;
        int match;
        bool dirty, shifted, show_ink, sample_stored, marginal;
} i_cell_t;

/* Grid structure */
typedef struct i_grid {
        i_cell_t *cells;
        int rows, cols, offset;
        bool clear;
} i_grid_t;

/* i_cell.c */
c_vec2_t I_cell_coords(int cell);
void I_cell_dirty(int cell);
void I_cell_draw_point(int cell, hcr_sample_t, int stroke,
                       c_vec2_t *offset);
void I_cell_draw_line(int cell, hcr_sample_t, int stroke, int segment,
                      c_vec2_t *offset);
void I_cell_draw_sample(int cell, bool match);
c_vec2_t I_hcr_to_cell_coords(c_vec2_t v);
int I_cell_offscreen(int cell);

/* i_grid.c */
void I_redraw_grid(int x, int y, int w, int h);

extern i_grid_t i_grid;
extern cairo_t *i_grid_cairo;
extern GdkPixmap *i_grid_pixmap;
extern GdkGC *i_grid_pixmap_gc;
extern int i_current_cell;
extern bool i_inserting, i_invalid, i_training;

