/* Name: colorspaces.c
 * Purpose: Implementation of colorspaces module which handles the conversion
 * between rgb and component video colorspaces
 * By: Bradley Chao and Kevin Prou
 * Date: October 22, 2022
 */

#include "colorspaces.h"
#include "pnm.h"
#include "a2methods.h"
#include "a2blocked.h"
#include "a2plain.h"

#include <stdlib.h>
#include <assert.h>

/*****************************************************************************
 *                            Compression Methods                            *
 *****************************************************************************/

/* Name: PPM_to_CVCS
 * Purpose: Converts 2D arr from RGB to component video colorspace
 * Parameters: Pointer to ppm pixmap and to operation methods
 * Returns: Pointer to new component video space 2D arr
 * Effects: Checked runtime error if given pointers are null
 */
A2Methods_UArray2 PPM_to_CVCS(Pnm_ppm pixmap, A2Methods_T block)
{
        assert(pixmap != NULL && block != NULL);

        A2Methods_UArray2 CV_array2 = block->new_with_blocksize(pixmap->width,
                                      pixmap->height, sizeof(struct CV), 2);

        /* Need to pass pixmap as closure to obtain denominator member var */
        block->map_block_major(CV_array2, RGB_to_CV, pixmap);

        return CV_array2;
}

/* Name: RGB_to_CV
 * Purpose: Helper apply function for converting individual RGB structs to CV
 * structs
 * Parameters: Current cur, row, and instance of array2. Curr refers to the
 * current element which is an RGB struct and the closure is the CV_array2 
 * where the transformed CVs are stored
 * Returns: None
 * Effects: None
 */
void RGB_to_CV(int col, int row, A2Methods_UArray2 array2, void *curr, 
                                                        void *cl)
{
        (void) array2;

        /* Obtain closure data */
        const struct A2Methods_T *block = ((Pnm_ppm) cl)->methods;
        Pnm_ppm pixmap = (Pnm_ppm) cl;

        /* Grab the individual pixel */
        Pnm_rgb pixel = (Pnm_rgb) (block->at(pixmap->pixels, col, row));

        float r = (float) pixel->red / (float) (pixmap->denominator);
        float g = (float) pixel->green / (float) (pixmap->denominator);
        float b = (float) pixel->blue / (float) (pixmap->denominator);
        
        float Y = 0.299 * r + 0.587 * g + 0.114 * b;
        float Pb = -0.168736 * r - 0.331264 * g + 0.5 * b;
        float Pr = 0.5 * r - 0.418688 * g - 0.081312 * b;

        ((CV) curr)->Y = Y;
        ((CV) curr)->Pb = Pb;
        ((CV) curr)->Pr = Pr;
}

/*****************************************************************************
 *                            Decompression Methods                          *
 *****************************************************************************/

/* Name: CVCS_to_PPM
 * Purpose: Converts from component video colorspace back to RGB colorspace
 * Parameters: 2D CV array, and blocked and plain operations
 * Returns: A pixmap with a 2D array of pixels as one of its members
 * Effects: Checked runtime error if parameters are null 
 */
Pnm_ppm CVCS_to_PPM(A2Methods_UArray2 CV_array2, A2Methods_T block,
                                                             A2Methods_T plain)
{
        //CV array is in blocked form but we want a new plain form
        assert(CV_array2 != NULL);
        assert(block != NULL && plain != NULL);

        unsigned width = block->width(CV_array2);
        unsigned height = block->height(CV_array2);

        /* Malloc and pass pointer to pixmap around since it's large size */
        Pnm_ppm pixmap = malloc(sizeof(*pixmap));
        assert(pixmap);

        struct Pnm_ppm insert = { .width = width, .height = height, 
                        .denominator = 255, .pixels = NULL, .methods = plain };
        
        *pixmap = insert;

        pixmap->pixels = plain->new(width, height, sizeof(struct Pnm_rgb));

        block->map_block_major(CV_array2, CV_to_RGB, pixmap);

        return pixmap;
}

/* Name: CV_to_RGB
 * Purpose: Helper apply function for converting individual CV struct to RGB
 * Parameters: Current col, row, instance of array2. Curr elem refers to CV 
 * struct and closure refers to the new RGB 2D array 
 * Returns: Void
 * Effects: Checked runtime error if parameters are null 
 */
void CV_to_RGB(int col, int row, A2Methods_UArray2 array2, void *curr, 
                                                        void *cl)
{
        (void) array2;

        A2Methods_T plain = uarray2_methods_plain;

        assert(plain != NULL);

        Pnm_ppm pixmap = (Pnm_ppm) cl;

        float Y = ((CV) curr)->Y;
        float Pb = ((CV) curr)->Pb;
        float Pr = ((CV) curr)->Pr;

        float r = 1.0 * Y + 0.0 * Pb + 1.402 * Pr;
        float g = 1.0 * Y - 0.344136 * Pb - 0.714136 * Pr;
        float b = 1.0 * Y + 1.772 * Pb + 0.0 * Pr;

        /* if below 0 should be 0 */
        /* if above denoinator floor to denominator */
        if (r < 0) r = 0; 
        if (g < 0) g = 0;
        if (b < 0) b = 0;

        if (r > 1) r = 1;
        if (g > 1) g = 1;
        if (b > 1) b = 1;

        unsigned red = (unsigned) (r * pixmap->denominator);
        unsigned green = (unsigned) (g * pixmap->denominator);
        unsigned blue = (unsigned) (b * pixmap->denominator);

        Pnm_rgb change = 
                       (Pnm_rgb) (plain->at(((Pnm_ppm) cl)->pixels, col, row));

        change->red = red;
        change->green = green;
        change->blue = blue;
}