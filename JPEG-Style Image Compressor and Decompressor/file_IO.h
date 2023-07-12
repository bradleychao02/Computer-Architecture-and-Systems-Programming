/* Name: file_IO.h
 * Purpose: Header file for file_IO module which deals with reading pnm images 
 * and compressed binary files into memory, and also deals with writing pnm 
 * images and compressed binary files to files
 */

#ifndef FILE_IO_H
#define FILE_IO_H

#include "a2methods.h"
#include "a2plain.h"
#include "pnm.h"

/*****************************************************************************
 *                            Compression Methods                            *
 *****************************************************************************/

/* Name: read_ppm
 * Purpose: Reads a trimmed ppm image file into memory 
 * Parameters: Pointer to file and pointer to plain operations struct
 * Returns: Pointer to a pnm_ppm
 * Effects: Checked runtime error if supplied pointers are null
 */
Pnm_ppm read_ppm(FILE *file, A2Methods_T plain);

/* Name: trim_image
 * Purpose: This function trims a column or a row to produce a Pnm_ppm with 
 * even rows and columns
 * Parameters: The Pnm_ppm by reference and the plain operations
 * Returns: Void
 * Effects: Checked runtime error if pointers are null
 */
void trim_image(Pnm_ppm *image, A2Methods_T plain);

/* Name: print_binary_file
 * Purpose: Traverses a 2D arr of packed codewords in row major order and
 * prints them to stdout in big endian order 
 * Parameters: Pointer to 2D arr of packed codewords and plain operations
 * Returns: Void
 * Effects: Checked runtime error if parameters are null
 */
void print_binary_file(A2Methods_UArray2 packed_words, A2Methods_T plain);

/* Name: print_codeword
 * Purpose: Helper apply function for printing individual codeword to output
 * Parameters: The current col, row, 2D arr instance, elem, and closure
 * Returns: Void
 * Effects: None
 */
void print_codeword(int col, int row, A2Methods_UArray2 array2, void *curr, 
                    void *cl);

/*****************************************************************************
 *                            Decompression Methods                          *
 *****************************************************************************/


/* Name: read_binary
 * Purpose: Reads binary file into memory as a 2D array of 64 bit codewords
 * Parameters: File pointer and plain operations
 * Returns: 2D array of 64 bit codewords
 * Effects: Checked runtime error if parameters are null 
 */
A2Methods_UArray2 read_binary(FILE *fp, A2Methods_T plain);


/* Name: read_bin_apply
 * Purpose: Helper apply function that reads by 32 bits and converts to word
 * Parameters: The current col, row, instance of array2, elem, and file to 
 * read from
 * Returns: Void
 * Effects: None
 */
void read_bin_apply(int col, int row, A2Methods_UArray2 array2, void *curr, 
                    void *cl);

#endif
