/* Name: word.h
 * Purpose: Header file for word module which is responsible for the 
 * transformations between the pixel space and the compact DCT space
 * By: Bradley Chao and Kevin Prou
 * Date: October 16, 2022
 */

#ifndef WORD_H
#define WORD_H

#include "a2methods.h"
#include "a2blocked.h"
#include "uarray.h"
#include <stdlib.h>

typedef struct Word 
{
        unsigned a, Pba, Pra;
        signed b, c, d;
} *word;

/*****************************************************************************
 *                            Compression Methods                            *
 *****************************************************************************/

/* Name: CVCS_to_DCTS
 * Purpose: Maps a 2D arr from component video space to cosine space
 * Parameters: 2D CV arr and block operations
 * Returns: 2D arr of unpacked Words
 * Effects: Checked runtime error if parameters are null
 */
A2Methods_UArray2 CVCS_to_DCTS(A2Methods_UArray2 CV_array2, A2Methods_T block);

/* Name: CV_to_DCT
 * Purpose: Helper apply function for converting CV to Word struct
 * Parameters: Current col, row, instance of array2. Curr elem refers to CV 
 * and the Word 2D arr to save the data to is passed as closure
 * Returns: Void
 * Effects: None
 */
void CV_to_DCT(int col, int row, A2Methods_UArray2 array2, void *curr, 
               void *cl);

/* Name: compute_sub_index
 * Purpose: Helper function for computing the block_index of an element while
 * mapping through the 2D arr of CV structs
 * Parameters: The current col and row
 * Returns: Integer block_index 0 for UL, 1 for UR, 2 for LL, and 3 for LR
 * Effects: None
 */
int compute_sub_index(size_t col, size_t row);

/* Name: DCT
 * Purpose: Conversion function between a size 4 array of CV structs and a Word
 * Parameters: Size 4 array of CV structs
 * Returns: word struct value
 * Effects: Checked runtime error if block is null
 */
struct Word DCT(UArray_T block);

/* Name: quantize
 * Purpose: Keeps b, c, and d floating point numbers between -0.3 and 0.3
 * Parameters: Address of floats b, c, and d
 * Returns: Void
 * Effects: Modifies original values through pass by reference
 */
void quantize(float *b, float *c, float *d);

/* Name: scale_round
 * Purpose: Scale and round given float to a signed integer
 * Parameters: Floating point number to round and scaling factor
 * Returns: Signed integer that has been scaled and rounded
 * Effects: None
 */
signed scale_round(float x, int scale);

/*****************************************************************************
 *                            Decompression Methods                          *
 *****************************************************************************/

/* Name: DCTS_to_CVCS
 * Purpose: Maps 2D arr of Word structs to a 2D arr of CV structs
 * Parameters: 2D arr of Word structs, block and plain operation methods
 * Returns: 2D arr of CV structs
 * Effects: Checked runtime error if parameters are null
 */
A2Methods_UArray2 DCTS_to_CVCS(A2Methods_UArray2 word_array2, 
                                        A2Methods_T block, A2Methods_T plain);

/* Name: DCT_to_CV
 * Purpose: Helper apply function for mapping Word struct to CV struct
 * Parameters: Current col, row, instance of 2D arr. Curr elem refers to the
 * curr word and closure represents the 2D arr of CV structs to store data to
 * Returns: Void
 * Effects: None
 */
void DCT_to_CV(int col, int row, A2Methods_UArray2 array2, void *curr,
               void *cl);

/* Name: Word_to_CV
 * Purpose: Maps a Word struct to an array of 4 CV structs
 * Parameters: Pointer to word struct
 * Returns: Array with 4 CV structs in the order of: UL, UR, LL, LR
 * Effects: Checked runtime error if parameters are null
 */
UArray_T Word_to_CV(word unpacked_word);

#endif