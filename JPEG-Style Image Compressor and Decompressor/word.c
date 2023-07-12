/* Name: word.c
 * Purpose: This module is responsible for conversions between component video
 * colorspace and unpacked word space
 * By: Bradley Chao and Kevin Prou
 * Date: October 16, 2022
 */

#include "word.h"
#include "a2blocked.h"
#include "a2plain.h"
#include "a2methods.h"
#include "uarray2.h"
#include "colorspaces.h"
#include "arith40.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

/* This struct was made to thread multiple data members through closure 
 * The block_index indexes the position in a 2 x 2 block where the UL is 0,
   UR is 1, LL, is 2, and LRis 3
 * The 1D curr_block array holds 4 CVs which represent 1 Word together 
 * The compressed_words is 2D arr of Word structs 
 */

typedef struct small_close 
{
        size_t block_index;
        UArray_T curr_block;
        A2Methods_UArray2 compressed_words;
} *small_close;

/*****************************************************************************
 *                            Compression Methods                            *
 *****************************************************************************/

/* Name: CVCS_to_DCTS
 * Purpose: Maps a 2D arr from component video space to cosine space
 * Parameters: 2D CV arr and block operations
 * Returns: 2D arr of unpacked Words
 * Effects: Checked runtime error if parameters are null
 */
A2Methods_UArray2 CVCS_to_DCTS(A2Methods_UArray2 CV_array2, A2Methods_T block)
{
        assert(CV_array2 != NULL && block != NULL);

        assert(block->width(CV_array2) % 2 == 0);
        assert(block->height(CV_array2) % 2 == 0);

        size_t new_width = block->width(CV_array2) / 2;
        size_t new_height = block->height(CV_array2) / 2;

        A2Methods_T plain = uarray2_methods_plain;

        UArray_T block_arr = UArray_new(4, sizeof(struct CV));
        
        A2Methods_UArray2 compressed_words 
                      = plain->new(new_width, new_height, sizeof(struct Word));

        struct small_close small = { .block_index = 0, .curr_block = block_arr, 
                                          .compressed_words = compressed_words};

        block->map_block_major(CV_array2, CV_to_DCT, &small);

        UArray_free(&block_arr);

        return compressed_words;
}

/* Name: CV_to_DCT
 * Purpose: Helper apply function for converting CV to Word struct
 * Parameters: Current col, row, instance of array2. Curr elem refers to CV 
 * and the Word 2D arr to save the data to is passed as closure
 * Returns: Void
 * Effects: None
 */
void CV_to_DCT(int col, int row, A2Methods_UArray2 array2, void *curr, 
               void *cl)
{
        (void) array2;

        /* Break down closure into local components */
        small_close thread = (small_close) cl;
        UArray_T curr_block = thread->curr_block;
        A2Methods_UArray2 compressed_words = thread->compressed_words;

        /* Add CV0, CV1, CV2, and CV3 to the block */
        *(CV) UArray_at(curr_block, compute_sub_index(col, row)) = *(CV) curr;

        /* If we hit the end of the local block */
        if (compute_sub_index(col, row) == 3)
        {
                /* Convert the curr array of 4 CV structs into 1 word struct */
                struct Word unpacked_word = DCT(curr_block);

                A2Methods_T plain = uarray2_methods_plain;
                assert(plain);
                
                /* Floor division to find 2D index from original */
                size_t compressed_col = col / 2;
                size_t compressed_row = row / 2;

                /* Update Word value */
                *(word) plain->at(compressed_words, compressed_col, 
                                                compressed_row) = unpacked_word;

                /* Clear and rest the temporary Array */
                UArray_resize(curr_block, 0);
                UArray_resize(curr_block, 4);
        }
}

/* Name: compute_sub_index
 * Purpose: Helper function for computing the block_index of an element while
 * mapping through the 2D arr of CV structs
 * Parameters: The current col and row
 * Returns: Integer block_index 0 for UL, 1 for UR, 2 for LL, and 3 for LR
 * Effects: None
 */
int compute_sub_index(size_t col, size_t row)
{
        if (col % 2 == 0 && row % 2 == 0) return 0;
        if (col % 2 == 1 && row % 2 == 0) return 1;
        if (col % 2 == 0 && row % 2 == 1) return 2;
        else return 3;
}

/* Name: DCT
 * Purpose: Conversion function between a size 4 array of CV structs and a Word
 * Parameters: Size 4 array of CV structs
 * Returns: word struct value
 * Effects: Checked runtime error if block is null
 */
struct Word DCT(UArray_T block)
{
        assert(block != NULL);

        float Y1 = ((CV) UArray_at(block, 0))->Y;
        float Pb1 = ((CV) UArray_at(block, 0))->Pb;
        float Pr1 = ((CV) UArray_at(block, 0))->Pr;

        float Y2 = ((CV) UArray_at(block, 1))->Y;
        float Pb2 = ((CV) UArray_at(block, 1))->Pb;
        float Pr2 = ((CV) UArray_at(block, 1))->Pr;

        float Y3 = ((CV) UArray_at(block, 2))->Y;
        float Pb3 = ((CV) UArray_at(block, 2))->Pb;
        float Pr3 = ((CV) UArray_at(block, 2))->Pr;

        float Y4 = ((CV) UArray_at(block, 3))->Y;
        float Pb4 = ((CV) UArray_at(block, 3))->Pb;
        float Pr4 = ((CV) UArray_at(block, 3))->Pr;

        /* Convert lumina to coefficients */
        float a = ((Y4 + Y3 + Y2 + Y1) / 4.0);
        float b = ((Y4 + Y3 - Y2 - Y1) / 4.0);
        float c = ((Y4 - Y3 + Y2 - Y1) / 4.0);
        float d = ((Y4 - Y3 - Y2 + Y1) / 4.0);

        /* Rounds all 3 values to be between -0.3 and 0.3 inclusive */
        quantize(&b, &c, &d);

        unsigned qa = (signed) scale_round(a, 63);
        signed qb = scale_round(b, 103);
        signed qc = scale_round(c, 103);
        signed qd = scale_round(d, 103);

        /* Convert and quantize chroma values */
        float Pb_avg = (Pb1 + Pb2 + Pb3 + Pb4) / 4.0;
        float Pr_avg = (Pr1 + Pr2 + Pr3 + Pr4) / 4.0;

        assert(Pb_avg >= -0.5 && Pb_avg <= 0.5);
        assert(Pr_avg >= -0.5 && Pr_avg <= 0.5);

        unsigned Pb_index = Arith40_index_of_chroma(Pb_avg);
        unsigned Pr_index = Arith40_index_of_chroma(Pr_avg);

        struct Word unpacked_word = { .a = qa, .b = qb, .c = qc, .d = qd, 
                                            .Pba = Pb_index, .Pra = Pr_index };

        return unpacked_word;
}

/* Name: quantize
 * Purpose: Keeps b, c, and d floating point numbers between -0.3 and 0.3
 * Parameters: Address of floats b, c, and d
 * Returns: Void
 * Effects: Modifies original values through pass by reference
 */
void quantize(float *b, float *c, float *d)
{
        if (*b < -0.3) *b = -0.3;
        if (*b > 0.3) *b = 0.3;

        if (*c < -0.3) *c = -0.3;
        if (*c > 0.3) *c = 0.3;

        if (*d < -0.3) *d = -0.3;
        if (*d > 0.3) *d = 0.3;
}

/* Name: scale_round
 * Purpose: Scale and round given float to a signed integer
 * Parameters: Floating point number to round and scaling factor
 * Returns: Signed integer that has been scaled and rounded
 * Effects: None
 */
signed scale_round(float x, int scale)
{
        if (x >= 0)
        {
                return (signed) ((x * scale) + 0.5);
        }
        else
        {
                return (signed) ((x * scale) - 0.5);
        }
}

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
                                          A2Methods_T block, A2Methods_T plain)
{
        assert(word_array2 != NULL && block != NULL && plain != NULL);

        /* Multiply by 2 as we are going from 1 Word to 2 x 2 CV structs */
        size_t new_width = plain->width(word_array2) * 2;
        size_t new_height = plain->height(word_array2) * 2;

        A2Methods_UArray2 CV_array2 = block->new_with_blocksize(new_width, 
                                             new_height, sizeof(struct CV), 2);
        
        plain->map_row_major(word_array2, DCT_to_CV, CV_array2);
        
        return CV_array2;
}

/* Name: DCT_to_CV
 * Purpose: Helper apply function for mapping Word struct to CV struct
 * Parameters: Current col, row, instance of 2D arr. Curr elem refers to the
 * curr word and closure represents the 2D arr of CV structs to store data to
 * Returns: Void
 * Effects: None
 */
void DCT_to_CV(int col, int row, A2Methods_UArray2 array2, void *curr, void *cl)
{
        (void) array2;

        A2Methods_UArray2 CV_array2 = (A2Methods_UArray2) cl;

        UArray_T CV_shortarr = Word_to_CV((word) curr);

        A2Methods_T block = uarray2_methods_blocked;
        assert(block != NULL);

        /* Multiply by 2 as the new array is double size */
        /* Col and row of the word arr should map to the upper left hand corner
         * of the new CV arr */
        *(CV) block->at(CV_array2, 2 * col, 2 * row) = 
                                               *(CV) UArray_at(CV_shortarr, 0);

        *(CV) block->at(CV_array2, 2 * col + 1, 2 * row) = 
                                               *(CV) UArray_at(CV_shortarr, 1);

        *(CV) block->at(CV_array2, 2 * col, 2 * row + 1) = 
                                               *(CV) UArray_at(CV_shortarr, 2);

        *(CV) block->at(CV_array2, 2 * col + 1, 2 * row + 1) = 
                                               *(CV) UArray_at(CV_shortarr, 3);

        UArray_free(&CV_shortarr);
}

/* Name: Word_to_CV
 * Purpose: Maps a Word struct to an array of 4 CV structs
 * Parameters: Pointer to word struct
 * Returns: Array with 4 CV structs in the order of: UL, UR, LL, LR
 * Effects: Checked runtime error if parameters are null
 */
UArray_T Word_to_CV(word unpacked_word)
{
        assert(unpacked_word != NULL);

        UArray_T CV_shortarr = UArray_new(4, sizeof(struct CV));

        float a = (((float) (unpacked_word->a)) / 63);
        float b = (((float) (unpacked_word->b)) / 103);
        float c = (((float) (unpacked_word->c)) / 103);
        float d = (((float) (unpacked_word->d)) / 103);

        float Pb = Arith40_chroma_of_index(unpacked_word->Pba);
        float Pr = Arith40_chroma_of_index(unpacked_word->Pra);

        float Y1 = a - b - c + d;
        float Y2 = a - b + c - d;
        float Y3 = a + b - c - d;
        float Y4 = a + b + c + d;

        struct CV CV1 = { .Y = Y1, .Pb = Pb, .Pr = Pr };
        struct CV CV2 = { .Y = Y2, .Pb = Pb, .Pr = Pr };
        struct CV CV3 = { .Y = Y3, .Pb = Pb, .Pr = Pr };
        struct CV CV4 = { .Y = Y4, .Pb = Pb, .Pr = Pr };

        *(CV) UArray_at(CV_shortarr, 0) = CV1;
        *(CV) UArray_at(CV_shortarr, 1) = CV2;
        *(CV) UArray_at(CV_shortarr, 2) = CV3;
        *(CV) UArray_at(CV_shortarr, 3) = CV4;

        return CV_shortarr;
}