/* Name: pack.c
 * Purpose: This module is responsible for conversions between unpacked words
 * and packed 64 bit words
 * By: Bradley Chao and Kevin Prou
 * Date: October 16, 2022
 */

#include "pack.h"
#include "a2methods.h"
#include "a2plain.h"
#include "bitpack.h"
#include "word.h"
#include "assert.h"

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
                                                A2Methods_T plain)
{
        assert(compressed_array2 != NULL && plain != NULL);

        int width = plain->width(compressed_array2);
        int height = plain->height(compressed_array2);

        A2Methods_UArray2 packed_array = plain->new(width, height, 
                                                            sizeof(uint64_t));

        plain->map_row_major(compressed_array2, pack_word, packed_array);

        return packed_array;
}

/* Name: pack_word
 * Purpose: Helper apply function for packing a word struct into 64 bit int
 * Parameters: Curr col, row, array2. Curr elem refers to a word struct and the
 * closure represents the 2D arr of 64-bit integers to store data to
 * Returns: Void
 * Effects: None
 */
void pack_word(int col, int row, A2Methods_UArray2 array2, void *curr, 
                                                                void *cl)
{
        (void) array2;
        
        A2Methods_T plain = uarray2_methods_plain;
        assert(plain);

        A2Methods_UArray2 packed_array = (A2Methods_UArray2) cl;

        word unpacked_word = (word) curr;

        uint64_t packed_result = pack(unpacked_word);

        *(uint64_t *) plain->at(packed_array, col, row) = packed_result;
}

/* Name: pack
 * Purpose: Helper function for running the bitwise calculations to pack the
 * member variables of a word struct
 * Parameters: Pointer to word struct
 * Returns: Packed unsigned 64-bit integer 
 * Effects: Checked runtime error if parameter is null
 */
uint64_t pack(word unpacked_word)
{
        assert(unpacked_word != NULL);

        uint64_t packed_result = 0;

        packed_result = Bitpack_newu(packed_result, 6, 26, unpacked_word->a);
        packed_result = Bitpack_news(packed_result, 6, 20, unpacked_word->b);
        packed_result = Bitpack_news(packed_result, 6, 14, unpacked_word->c);
        packed_result = Bitpack_news(packed_result, 6, 8, unpacked_word->d);
        packed_result = Bitpack_newu(packed_result, 4, 4, unpacked_word->Pba);
        packed_result = Bitpack_newu(packed_result, 4, 0, unpacked_word->Pra);

        return packed_result;
}

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
                                                        A2Methods_T plain)
{
        assert(packed_words != NULL && plain != NULL);

        int width = plain->width(packed_words);
        int height = plain->height(packed_words);

        A2Methods_UArray2 unpacked_wordarray = plain->new(width, 
                                                height, sizeof(struct Word));

        plain->map_row_major(packed_words, unpack_word, 
                                                           unpacked_wordarray);

        return unpacked_wordarray;
}

/* Name: unpack_word
 * Purpose: Helper apply function for unpacking each word
 * Parameters: Curr col, row, array2. Curr refers to 64-bit int and cl refers
 * to a 2D arr of unpacked Word structs to save data to
 * Returns: Void
 * Effects: None
 */
void unpack_word(int col, int row, A2Methods_UArray2 array2, void *curr, 
                                                                void *cl)
{
        (void) array2;

        A2Methods_T plain = uarray2_methods_plain;

        A2Methods_UArray2 unpacked_wordarray = (A2Methods_UArray2) cl;

        uint64_t codeword = *(uint64_t *) curr;

        struct Word unpacked_word = unpack(codeword);


        *(word) plain->at(unpacked_wordarray, col, row) = unpacked_word;
}

/* Name: unpack
 * Purpose: Computation function for converting a 64-bit codeword into a word
 * Parameters: 64-bit packed integer value
 * Returns: Word struct value
 * Effects: None
 */
struct Word unpack(uint64_t codeword)
{
        unsigned a = Bitpack_getu(codeword, 6, 26);
        signed b = Bitpack_gets(codeword, 6, 20);
        signed c = Bitpack_gets(codeword, 6, 14);
        signed d = Bitpack_gets(codeword, 6, 8);
        unsigned Pba = Bitpack_getu(codeword, 4, 4);
        unsigned Pra = Bitpack_getu(codeword, 4, 0);

        struct Word unpacked_word = { .a = a, .b = b, .c = c, .d = d, 
                                                     .Pba = Pba, .Pra = Pra };

        return unpacked_word;
}