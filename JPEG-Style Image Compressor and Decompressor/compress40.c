/* Name: compress40.c
 * Purpose: This module is responsible for compressing full-color portable 
 * pixmaps into compressed binary files and also decompressing the binary files
 * back into pixmaps
 * By: Bradley Chao and Kevin Prou
 * Date: October 16, 2022
 */

#include "compress40.h"
#include "word.h"
#include "pnm.h"
#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "uarray2b.h"
#include "file_IO.h"
#include "colorspaces.h"
#include "word.h"
#include "pack.h"

#include <stdlib.h>
#include <stdio.h>

typedef A2Methods_UArray2 A2;

/* Name: compress40
 * Purpose: Compresses ppm into binary file and prints results to stdout
 * Parameters: Pointer to the ppm file
 * Returns: Void
 * Effects: Checked runtime error if provided file is null
 */
void compress40(FILE *input)
{
        /* Load A2 operation struct pointers */
        A2Methods_T plain_methods = uarray2_methods_plain;
        assert(plain_methods != NULL);
        A2Methods_T blocked_methods = uarray2_methods_blocked;
        assert(blocked_methods != NULL);

        /* Loads a trimmed ppm pixmap into memory */
        Pnm_ppm pixmap = read_ppm(input, plain_methods);

        /* Convert from RGB colorspace to CV colorspace */
        A2 CV_array2 = PPM_to_CVCS(pixmap, blocked_methods);

        /* Convert from CV colorspace to compressed DCT word space */
        A2 compressed = CVCS_to_DCTS(CV_array2, blocked_methods);

        /* Packs the word structs into a 2D arr of 64 bit code words */
        A2 packed_words = pack_wordarray(compressed, plain_methods);

        /* Commits results to standard output */
        print_binary_file(packed_words, plain_methods);

        /* Free Data */
        Pnm_ppmfree(&pixmap);
        blocked_methods->free(&CV_array2);
        plain_methods->free(&compressed);
        plain_methods->free(&packed_words);      
}


/* Name: decompress40
 * Purpose: Decompresses binary file into ppm and prints results to stdout
 * Parameters: Pointer to the ppm file
 * Returns: Void
 * Effects: Checked runtime error if provided file is null
 */
void decompress40(FILE *input)
{
        /* Load A2 operation struct pointers */
        A2Methods_T plain_methods = uarray2_methods_plain;
        assert(plain_methods);
        A2Methods_T blocked_methods = uarray2_methods_blocked;
        assert(blocked_methods);

        /* Loads compressed binary file into memory */
        A2 packed_words = read_binary(input, plain_methods);

        /* Unpacks the codewords into 2D arr of Word structs */
        A2 compressed = unpack_wordarray(packed_words, plain_methods);

        /* "De-quantizes" by converting 2D Word structs to 2D CV structs */
        A2 CV_array2 = DCTS_to_CVCS(compressed, blocked_methods, plain_methods);

        /* Translates back to RGB colorspace and obtains PPM */
        Pnm_ppm PPM = CVCS_to_PPM(CV_array2, blocked_methods, plain_methods);

        /* Commit results to standard output */
        Pnm_ppmwrite(stdout, PPM);

        /* Free Data */
        plain_methods->free(&packed_words);
        plain_methods->free(&compressed);
        blocked_methods->free(&CV_array2);
        Pnm_ppmfree(&PPM);
}
