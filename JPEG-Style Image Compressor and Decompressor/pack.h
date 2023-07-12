/* Name: pack.h
 * Purpose: This module is the header file which is responsible for 
 * conversions between unpacked words and packed 64 bit words
 * By: Bradley Chao and Kevin Prou
 * Date: October 16, 2022
 */

#ifndef PACK_H
#define PACK_H

#include "a2methods.h"
#include "a2plain.h"
#include <stdint.h>
#include "word.h"

/*****************************************************************************
 *                            Compression Methods                            *
 *****************************************************************************/

/* Name: pack_wordarray
 * Purpose: This function maps a 2D arr of unpacked words to a 2D arr of 
 * packed 64 bit unsigned integers
 * Parameters: 2D arr of Words and plain operations
 * Returns: 2D arr of packed 64 bit unsigned integers
 * Effects: Checked runtime error if parameters are null
 */
A2Methods_UArray2 pack_wordarray(A2Methods_UArray2 compressed_array2, 
                                                            A2Methods_T plain);

/* Name: pack_word
 * Purpose: Helper apply function for packing a word struct into 64 bit int
 * Parameters: Curr col, row, array2. Curr elem refers to a word struct and the
 * closure represents the 2D arr of 64-bit integers to store data to
 * Returns: Void
 * Effects: None
 */
void pack_word(int col, int row, A2Methods_UArray2 array2, void *curr, 
                                                                void *cl);
/* Name: pack
 * Purpose: Helper function for running the bitwise calculations to pack the
 * member variables of a word struct
 * Parameters: Pointer to word struct
 * Returns: Packed unsigned 64-bit integer 
 * Effects: Checked runtime error if parameter is null
 */
uint64_t pack(word unpacked_word);

/*****************************************************************************
 *                            Decompression Methods                          *
 *****************************************************************************/

/* Name: unpack_wordarray
 * Purpose: Maps 2D arr of packed 64-bit integers to a 2D arr of word structs
 * Parameters: 2D arr of 64-bit integers and plain operations
 * Returns: 2D arr of word structs
 * Effects: Checked runtime error if parameters are null
 */
A2Methods_UArray2 unpack_wordarray(A2Methods_UArray2 packed_words, 
                                                            A2Methods_T plain);
                                                            
/* Name: unpack_word
 * Purpose: Helper apply function for unpacking each word
 * Parameters: Curr col, row, array2. Curr refers to 64-bit int and cl refers
 * to a 2D arr of unpacked Word structs to save data to
 * Returns: Void
 * Effects: None
 */
void unpack_word(int col, int row, A2Methods_UArray2 array2, void *curr, 
                                                                void *cl);

/* Name: unpack
 * Purpose: Computation function for converting a 64-bit codeword into a word
 * Parameters: 64-bit packed integer value
 * Returns: Word struct value
 * Effects: None
 */
struct Word unpack(uint64_t codeword);

#endif