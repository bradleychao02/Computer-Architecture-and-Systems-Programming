/* Name: file_IO.c
 * Purpose: Implementation of file_IO which deals with reading pnm images and
 * compressed binary files into memory, and also deals with writing pnm images
 * and compressed binary files to files
 * By: Bradley Chao and Kevin Prou
 * Date: October 21, 2022
 */

#include "pnm.h"
#include "a2methods.h"
#include "a2plain.h"
#include "file_IO.h"
#include "bitpack.h"

#include <stdio.h>
#include <assert.h>

/*****************************************************************************
 *                            Compression Methods                            * 
 *****************************************************************************/

/* Name: read_ppm
 * Purpose: Reads a trimmed ppm image file into memory 
 * Parameters: Pointer to file and pointer to plain operations struct
 * Returns: Pointer to a pnm_ppm
 * Effects: Checked runtime error if supplied pointers are null
 */
Pnm_ppm read_ppm(FILE *file, A2Methods_T plain)
{
        assert(file != NULL && plain != NULL);

        Pnm_ppm image = Pnm_ppmread(file, plain);

        trim_image(&image, plain);

        return image;
}

/* Name: trim_image
 * Purpose: This function trims a column or a row to produce a Pnm_ppm with 
 * even rows and even columns
 * Parameters: The Pnm_ppm by reference and the plain operations
 * Returns: Void
 * Effects: Checked runtime error if pointers are null
 */
void trim_image(Pnm_ppm *image, A2Methods_T plain)
{
        assert(image != NULL && *image != NULL && plain != NULL);

        /* Create new trimmed 2D pixels to replace/trim the original */
        size_t new_width = (*image)->width;
        size_t new_height = (*image)->height;

        /* Adjust widths and heights if they are odd by deducting one */
        if ((*image)->width % 2 != 0) new_width--;
        if ((*image)->height % 2 != 0) new_height--;

        /* Sanity check that the raster is even by even */
        assert(new_width % 2 == 0 && new_height % 2 == 0);

        /* If dimensions were affected, we need to conduct a trim */
        if (new_width != (*image)->width || new_height != (*image)->height) 
        {
                A2Methods_UArray2 new_pixels 
                                     = plain->new(new_width, new_height, 
                                                       sizeof(struct Pnm_rgb));

                for (size_t c = 0; c < new_width; c++)
                {
                        for (size_t r = 0; r < new_height; r++)
                        {
                                Pnm_rgb old 
                                    = plain->at((*image)->pixels, c, r);

                                *(Pnm_rgb) (plain->at(new_pixels, c, r))
                                                                        = *old;
                        }
                }

                /* Free original raster and update member variables */
                plain->free(&((*image)->pixels));
                (*image)->pixels = new_pixels;
                (*image)->width = new_width;
                (*image)->height = new_height;
        }
}

/* Name: print_binary_file
 * Purpose: Traverses a 2D array of packed codewords in row major order and
 * prints them to stdout in big endian order 
 * Parameters: Pointer to 2D array of packed codewords and plain operations
 * Returns: Void
 * Effects: Checked runtime error if parameters are null
 */
void print_binary_file(A2Methods_UArray2 packed_words, A2Methods_T plain)
{
        assert(packed_words != NULL && plain != NULL);

        /* Adjust for step when dimensions were cut in half from CV to Word */
        unsigned width = plain->width(packed_words) * 2;
        unsigned height = plain->height(packed_words) * 2;

        fprintf(stdout, "COMP40 Compressed image format 2\n%u %u\n", width, 
                height);

        plain->map_row_major(packed_words, print_codeword, NULL);
}

/* Name: print_codeword
 * Purpose: Helper apply function for printing individual codeword to output
 * Parameters: The current col, row, 2D array instance, elem, and closure
 * Returns: Void
 * Effects: None
 */
void print_codeword(int col, int row, A2Methods_UArray2 array2, void *curr, 
                    void *cl)
{
        (void) col;
        (void) row;
        (void) array2;
        (void) cl;

        /* Cast storage to 32 bit integer */
        uint32_t bit32_codeword = (uint32_t) (*(uint64_t *) curr);

        /* Index by each byte/8 bits and putchar */
        for (int lsb = 24; lsb >= 0; lsb -= 8)
        {
                putchar(Bitpack_getu(bit32_codeword, 8, lsb));
        }
}

/*****************************************************************************
 *                            Decompression Methods                          *
 *****************************************************************************/

/* Name: read_binary
 * Purpose: Reads binary file into memory as a 2D array of 64 bit codewords
 * Parameters: File pointer and plain operations
 * Returns: 2D array of 64 bit codewords
 * Effects: Checked runtime error if parameters are null 
 */
A2Methods_UArray2 read_binary(FILE *fp, A2Methods_T plain)
{
        assert(fp != NULL);
        assert(plain != NULL);

        unsigned height, width;
        int read = fscanf(fp, "COMP40 Compressed image format 2\n%u %u", 
                                                              &width, &height);
        assert(read == 2);
        int c = getc(fp);
        assert(c == '\n');

        /* Dimensions need to be halved to adjust to Word dimensions */
        height /= 2;
        width /= 2;

        A2Methods_UArray2 packed_words = plain->new(width, height, 
                                                             sizeof(uint64_t));

        plain->map_row_major(packed_words, read_bin_apply, fp);

        return packed_words;
}

/* Name: read_bin_apply
 * Purpose: Helper apply function that reads by 32 bits and converts to word
 * Parameters: The current col, row, instance of array2, elem, and file to 
 * read from
 * Returns: Void
 * Effects: None
 */
void read_bin_apply(int col, int row, A2Methods_UArray2 array2, void *curr, 
                    void *cl)
{
        (void) col;
        (void) row;
        (void) array2;

        FILE *fp = (FILE *) cl;
        assert(fp != NULL);
        
        uint64_t *codeword = (uint64_t *) curr;
        *codeword = 0;

        uint64_t single_byte;

        for (int lsb = 24; lsb >= 0; lsb -= 8)
        {
                /* Gets 8 bits of data */
                single_byte = getc(fp);
                *codeword = Bitpack_newu(*codeword, 8, lsb, single_byte);
        }
}




