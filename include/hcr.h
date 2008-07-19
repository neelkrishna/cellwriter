/******************************************************************************\
 HCR Library (LGPL) - Copyright (C) 2008 - Michael Levin

 The HCR Library is free software: you can redistribute it and/or modify it
 under the terms of the GNU Lesser General Public License as published by the
 Free Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 The HCR Library is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License along
 with the HCR Library. If not, see <http://www.gnu.org/licenses/>.
\******************************************************************************/

/* TRUE and FALSE as used in the library */
#define HCR_TRUE 1
#define HCR_FALSE 0

/* This sample index is reserved for the input sample */
#define HCR_INPUT 0

/* Generic enumerations for HCR_get()/HCR_set() */
typedef enum {

        /* Set this to HCR_TRUE to match input to samples with a different
           number of strokes. This option makes recognition signficantly
           slower. */
        HCR_ANY_LENGTH,

        /* Set this to HCR_TRUE to match input strokes to sample strokes that
           were drawn in a different direction (reversed). This option may
           make recognition slightly slower. */
        HCR_ANY_DIRECTION,

        /* Set to HCR_TRUE to enable debug print-outs from the library */
        HCR_DEBUG,

        HCR_ENUMS
} hcr_enum_t;

/* Allows usage of library with C++ */
#ifdef __cplusplus
extern "C" {
#endif

/* Initialize the library by calling HCR_init() */
void HCR_init(void);

/* Use HCR_get() and HCR_set() to specify and retreive integer parameters from
   the library. Both functions return non-zero if [value] was set. */
int HCR_get(hcr_enum_t, int *value);
int HCR_set(hcr_enum_t, int value);

/* To begin inputting new data, call HCR_input_reset(). Start new strokes with
   HCR_input_stroke() and add points with HCR_input() until done. Point [x] and
   [y] values range from 0.0 to 1.0 inclusive. */
void HCR_input_reset(void);
void HCR_input_stroke(void);
void HCR_input(float x, float y);

/* Recognition can be aided by providing the characters around the input sample.
   Input a list of words and the number of times they appear in the list via
   HCR_context_train(). The context training data can be reset by calling
   HCR_context_reset(). Before calling HCR_recognize(), call HCR_context()
   to input the characters before and after the input sample.*/
void HCR_context(const char *before, const char *after);
void HCR_context_train(const char *word, int quantity);
void HCR_context_reset(void);

/* Once you have input a sample, you can recognize it with HCR_recognize()
   or train on it with HCR_train(). You can call both functions on the same
   input. HCR_recognize() returns the number of matches produced by recognition.
   To clear all trained samples, call HCR_reset(). */
void HCR_train(void);
int HCR_recognize(void);
void HCR_reset(void);

/* To get at the list of matches from the last call to HCR_recognize(), call
   HCR_get_match(). Returns non-zero if a valid index is specified and
   [sample] and [quality] were set. The match [quality] will range from
   0.0 (worst possible match) to 1.0 (best possible match) inclusive. */
int HCR_get_match(int index, int *sample, float *quality);

/* If [sample] is negative, HCR_length() returns the number of samples stored
   in the library. Otherwise it returns the number of strokes in the given
   [sample]. A [sample] value of HCR_INPUT refers to the last input sample. */
int HCR_length(int sample);

/* To read back sample data, first select the desired sample and stroke. If the
   given sample or stroke is invalid, HCR_select() returns zero. Get point data
   by calling HCR_get_point() with a point index. HCR_get_point() returns zero
   if [index] is invalid. */
int HCR_select(int sample, int stroke);
int HCR_get_point(int index, float *x, float *y);

/* When finished using the library, call HCR_cleanup(). Note that you cannot
   make any more calls to the library after calling this. */
void HCR_cleanup(void);

/* Allows usage of library with C++ */
#ifdef __cplusplus
}
#endif

