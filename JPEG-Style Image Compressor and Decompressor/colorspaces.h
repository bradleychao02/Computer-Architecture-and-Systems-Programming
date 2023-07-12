/* Name: colorspace.h
 * Purpose: This module was made to handle the conversion between the rgb 
 * and component video colorspaces
 * By: Bradley Chao and Kevin Prou
 * Date: October 22, 2022
 */

#ifndef COLORSPACES_H
#define COLORSPACES_H

#include "pnm.h"
#include "a2methods.h"
#include "uarray2b.h"
#include <stdio.h>
#include <assert.h>

/* Move this back to colorspaces.c when done testing */
typedef struct CV
{
        float Y, Pb, Pr;
} *CV;

/*****************************************************************************
 *                            Decompression Methods                          *
 *****************************************************************************/

/* Name: PPM_to_CVCS
 * Purpose: Converts 2D arr from RGB to component video colorspace
 * Parameters: Pointer to ppm pixmap and to operation methods
 * Returns: Pointer to new component video space 2D arr
 * Effects: Checked runtime error if given pointers are null
 */
A2Methods_UArray2 PPM_to_CVCS(Pnm_ppm image, A2Methods_T block);


/* Name: RGB_to_CV
 * Purpose: Helper apply function for converting individual RGB structs to CV
 * structs
 * Parameters: Current cur, row, and instance of array2. Curr refers to the
 * current element which is an RGB struct and the closure is the CV_array2 
 * where the transformed CVs are stored
 * Returns: None
 * Effects: None
 */
void RGB_to_CV(int col, int row, A2Methods_UArray2 pixels, void *curr, 
                                                                void *cl);

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
                                                         A2Methods_T plain);

/* Name: CV_to_RGB
 * Purpose: Helper apply function for converting individual CV struct to RGB
 * Parameters: Current col, row, instance of array2. Curr elem refers to CV 
 * struct and closure refers to the new RGB 2D array 
 * Returns: Void
 * Effects: Checked runtime error if parameters are null 
 */

void CV_to_RGB(int col, int row, A2Methods_UArray2 array2, void *curr, 
                                                                void *cl);

#endif