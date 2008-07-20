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

/* This sample index is reserved for the input sample */
#define HCR_INPUT 0

/* Type used for sample handles */
typedef int hcr_sample_t;

/* Type used for classes; this can be anything you want */
typedef int hcr_class_t;

/* TRUE and FALSE as used in the library */
typedef enum {
        HCR_FALSE = 0,
        HCR_TRUE = 1,
} hcr_bool_t;

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
hcr_bool_t HCR_get(hcr_enum_t, int *value);
hcr_bool_t HCR_set(hcr_enum_t, int value);

/* To begin inputting new data, call HCR_sample_delete(HCR_INPUT) to clear the
   old input. Start new strokes with HCR_input_stroke() and add points with
   HCR_input() until done. Point [x] and [y] values range from 0.0 to 1.0
   inclusive. */
void HCR_input(float x, float y);
void HCR_input_stroke(void);

/* When the sample is finished, save it with HCR_input_store() -- this function
   returns the sample's handle in the database. Note that samples entered this
   way start out disabled. Set their class (defaults to 0) and enable the
   sample after input. */
hcr_sample_t HCR_input_store(void);

/* Recognition can be aided by providing the characters around the input sample.
   Input a list of words and the number of times they appear in the list via
   HCR_context_train(). The context training data can be reset by calling
   HCR_context_reset(). Before calling HCR_recognize(), call HCR_context()
   to input the characters before and after the input sample.*/
void HCR_context(const char *before, const char *after);
void HCR_context_train(const char *word, int quantity);
void HCR_context_reset(void);

/* Samples in the database can be deleted using HCR_sample_delete() or disabled
   (ignored during matching) using HCR_sample_enable(). To retrieve a sample's
   class call HCR_sample_get_class() and set it call HCR_sample_set_class(). */
void HCR_sample_delete(hcr_sample_t);
void HCR_sample_enable(hcr_sample_t, hcr_bool_t);
void HCR_sample_set_class(hcr_sample_t, hcr_class_t);
hcr_class_t HCR_sample_get_class(hcr_sample_t);

/* It is also possible to delete or disable every sample belonging to a
   particular class using HCR_class_delete() and HCR_class_enable(). Both of
   these functions return the number of samples belonging to that class. */
int HCR_class_delete(hcr_class_t);
int HCR_class_enable(hcr_class_t, hcr_bool_t);

/* Once you have input a sample, you can ask the system to recognize it with
   HCR_classify(). HCR_classify() returns the number of matches produced. The
   sample's class is not set by HCR_classify(). To get at the matches
   themselves, call HCR_get_match(). Returns non-zero if a valid index is
   specified and [sample] and [quality] were set.*/
int HCR_classify(hcr_sample_t);
int HCR_get_match(int index, hcr_sample_t *sample, float *quality);

/* If given a negative sample handle, HCR_length() returns the number of samples
   stored in the library. Otherwise it returns the number of strokes in the
   given sample. */
int HCR_length(hcr_sample_t);

/* To read back sample data, first select the desired sample and stroke. If the
   given sample or stroke is invalid, HCR_select() returns zero. Get point data
   by calling HCR_get_point() with a point index. HCR_get_point() returns zero
   if [index] is invalid. */
hcr_bool_t HCR_select(hcr_sample_t, int stroke);
hcr_bool_t HCR_get_point(int index, float *x, float *y);

/* When finished using the library, call HCR_cleanup(). Note that you cannot
   make any more calls to the library after calling this. */
void HCR_cleanup(void);

/* Allows usage of library with C++ */
#ifdef __cplusplus
}
#endif

