/* Name: uarray2b.c
*  Purpose: Implementation of the blocked 2D array which allows elements in 
*  specified blocks to be stored in close proxitmity in memeory, taking 
*  advantage of spacial locality to improve performance
*  George McGurkin and Bradley Chao
*  October 13, 2022
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "uarray2b.h"
#include "uarray2.h"
#include "uarray.h"
#include <math.h>

/* UArray2b_T struct

 * Components:
 *      1) A UArray2_T, containing a different block at each element. Each 
 *         block is represented as a UArray_T
 *      2) The total number of columns / width of the UArray2b
 *      3) The total number of rows / height of the UArray2b
 *      4) The block size, which represents the height/width of each block. In 
 *         other words, the total number of elements in each block is 
 *         block_size * block_siz
 *      5) The size of the element tpye stored in the UArray2b
 */
struct UArray2b_T {
        UArray2_T block_array;
        int num_cols;
        int num_rows;
        int block_size;
        int elem_size;
};

typedef void ua2b_apply(int col, int row, UArray2b_T uarray2b, void *curr,
                        void *cl);

/* small_cl struct

 * Purpose: This structs serves as a means to package multiple variables into 
 * one closure variable, so when we perform the mapping function, we can call 
 * row major on the block array, and pass the user given apply function and 
 * closure, as well as the uarray2b to the apply function for each block

 * Components: 
 *      1) The uarray2b object, used to access the width, height, and block 
 *         size.
 *      2) A pointer to an apply function
 *      3) A void pointer used to hold the user-given closure
 */
struct small_cl {
        UArray2b_T uarray2b;
        ua2b_apply *apply;
        void *cl;
};

/* helper function definitions; function contracts found below */
int ceiling(int a, int b);

void initialize_block_array(int col, int row, UArray2_T uarray2, void *curr, 
                            void *cl);

void free_block_array(int col, int row, UArray2_T uarray2, void *curr, 
                      void *cl);

void block_apply(int block_col, int block_row, UArray2_T uarray2, 
                 void *curr, void *cl);

/* Name: UArray2b_new

 * Purpose: Creates a 2D unboxed array with dimensions and element sizes,
 * and block size specified by the four parameters. 
 
 * Parameters: Integer width (columns), height (rows), element size, and 
 * block size (sqrt of num elements per block)
 
 * Returns: Pointer to newly initialized, empty unboxed array (UArray2b)

 * Expectations: 
 * Check Runtime Error for the following:
 *      Any of the four parameter values are less than or equal to zero
 *      Memory allocation fails
 */
UArray2b_T UArray2b_new(int width, int height, int size, int blocksize) 
{
        assert(width >= 0 && height >= 0 && size > 0 && blocksize > 0);

        UArray2b_T uarray2b = malloc(sizeof(*uarray2b));
        assert(uarray2b != NULL);

        uarray2b->num_cols = width;
        uarray2b->num_rows = height;
        uarray2b->block_size = blocksize;
        uarray2b->elem_size = size;
        
        /* declare a UArray2 to hold each of the blocks of the array */
        uarray2b->block_array = UArray2_new(
                                ceiling(width, blocksize), 
                                ceiling(height, blocksize),
                                sizeof(UArray_T));
        
        /* initalize all the blocks to be empty UArrays */
        UArray2_map_row_major(uarray2b->block_array, 
                              initialize_block_array, 
                              uarray2b);

        return uarray2b;
}

/* Name: ceiling

*  Purpose: Find the ceiling of two integers (i.e. rounding up after divison)

*  Parameters: The two integers to perform the operation on

*  Returns: The ceiling quotient result

*  Effects: Checked runtime error if any of the integers are not positive
*/
int ceiling(int a, int b)
{
        assert(a > 0 && b > 0);
    
        if (a % b != 0) {
            return (a / b) + 1;
        }

        return a / b;
}

/* Name: initialize_block_array

*  Purpose: Apply function for initializing a new block array

*  Parameters: The current col, row, the array2 to make a blocked version of
*  the given 2D array

*  Returns: Void

*  Effects: Checked runtime error if the current array or the 2B array is null
*/
void initialize_block_array(int col, int row, UArray2_T uarray2, void *curr, 
                                                                 void *cl) 
{
        (void) col;
        (void) row; 
        (void) uarray2;
        
        assert(curr != NULL);
        assert(cl != NULL);
        
        UArray2b_T uarray2b = (UArray2b_T) cl;
        /* assign the curr block to be an newly initialized UArray*/
        *((UArray_T *) curr) = UArray_new(uarray2b->block_size * 
                                          uarray2b->block_size, 
                                          uarray2b->elem_size);
}

/* Name: UArray2b_new_64k_block

 * Purpose: Creates a 2D unboxed array with dimensions and element sizes
 * specified by the three parameters. Block size is set to the largest 
 * amount possible while still have the total size of each block be less than
 * 64 kilobytes of memory
 
 * Parameters: Integer width (columns), height (rows), and element size
 
 * Returns: Pointer to newly initialized, empty unboxed array (UArray2b)

 * Expectations: 
 * Check Runtime Error for the following:
 *      Any of the three parameter values are less than or equal to zero
 *      Memory allocation fails
 */
UArray2b_T UArray2b_new_64K_block(int width, int height, int size) 
{
        assert(width >= 0 && height >= 0 && size > 0);
        
        /* calculate block size by dividing 64K over the elem size */
        int blocksize = sqrt((64 * 1024) / size);
        /* if the calculate blocksize is 0 (meaning size > 64k) set the 
           block sizze to 1 */
        if (blocksize == 0) { 
                blocksize = 1;
        }
        
        return UArray2b_new(width, height, size, blocksize);
}

/* Name: UArray2b_free

 * Purpose: Frees the memory associated with the 2D block array 

 * Parameters: The address of the 2D block array to free

 * Returns: Void

 * Effects: Checked runtime error if the 2D blocked array is null 
 */
void UArray2b_free(UArray2b_T *uarray2b)
{
        assert(uarray2b != NULL);

        /* free all blocks via an apply function */
        UArray2_map_row_major((*(UArray2b_T *) uarray2b)->block_array, 
                              free_block_array, NULL);

        /* free the block array*/
        UArray2_free(&(*(UArray2b_T *) uarray2b)->block_array);

        /* free the UArray2b itself */
        free(*((UArray2b_T *) uarray2b));
}

/* Name: free_block_array

 * Purpose: Apply function used to free all the all the blocks in the block
 * array

 * Parameters: The current block column and row, pointer to the 2D block array,
 * a pointer to the current block, and a closure pointer

 * Returns: void

 * Effects: Frees the memory stored at the current block
 */
void free_block_array(int col, int row, UArray2_T uarray2, void *curr, void *cl)
{
        (void) cl;
        (void) col;
        (void) row;
        (void) uarray2;
        
        UArray_free((UArray_T *) curr);
}

/* Name: UArray2b_width

 * Purpose: Gets the width/number of columns of the UArray2b provided by the 
 * parameter variable.

 * Parameters: A pointer to a UArray2b.

 * Returns: The width/number of columns in the form of an integer.

 * Expectations: Checked runtime error if given UArray2 parameter is null.
 */
int UArray2b_width(UArray2b_T uarray2b) 
{
        assert(uarray2b != NULL);
        return uarray2b->num_cols;
}

/* Name: UArray2b_height

 * Purpose: Gets the height/number of rows of the UArray2b provided by the 
 * parameter variable.
 
 * Parameters: A pointer to a UArray2b.

 * Returns: The height/number of rows in the form of an integer.
 
 * Expectations: Checked runtime error if given UArray2b parameter is null.
 */
int UArray2b_height(UArray2b_T uarray2b) 
{
        assert(uarray2b != NULL);
        return uarray2b->num_rows;
} 

/* Name: UArray2b_size

 * Purpose: Gets the element size of the UArray2b provided by the 
 * parameter variable.
 
 * Parameters: A pointer to a UArray2b.

 * Returns: The element size in the form of an integer.
 
 * Expectations: Checked runtime error if given UArray2b parameter is null.
 */
int UArray2b_size(UArray2b_T uarray2b)
{
        assert(uarray2b != NULL);
        return uarray2b->elem_size;
}

/* Name: UArray2b_blocksize

 * Purpose: Gets the blocksize of the UArray2b provided by the 
 * parameter variable.
 
 * Parameters: A pointer to a UArray2b.

 * Returns: The blocksize in the form of an integer.
 
 * Expectations: Checked runtime error if given UArray2b parameter is null.
 */
int UArray2b_blocksize(UArray2b_T uarray2b)
{
        assert(uarray2b != NULL);
        return uarray2b->block_size;
}

/* Name: UArray2b_at

 * Purpose: Retrieves the data of the 2D array specified by the first parameter
 * at the index values provided by the second and third parameters, where the 
 * second specifies the column, and the third specifies the row.
 
 * Parameters: A pointer to a UArray2b, the element's column index in the form 
 * of an integer, and the element's row index in the form of an integer

 * Returns: A void pointer to the data specified by the parameters. 
 
 * Expectations: 
 * Checked runtime error for the following:
 *      The given UArray2b parameter is null
 *      The column or row indices are outside of range of the 2D array
 */
void *UArray2b_at(UArray2b_T uarray2b, int column, int row)
{
        assert(uarray2b != NULL);
        assert(column >= 0 && column < uarray2b->num_cols);
        assert(row >= 0 && row < uarray2b->num_rows);

        /* calulate the block indices in which the desired element is stored */
        int block_col = column / uarray2b->block_size;
        int block_row = row / uarray2b->block_size;

        /* get the index in the block based off of the given two row and col 
           indices */
        int block_index = ((row % uarray2b->block_size) * 
                          uarray2b->block_size) + 
                          (column % uarray2b->block_size);
        
        UArray_T *block = UArray2_at(uarray2b->block_array, 
                                               block_col, block_row);

        return UArray_at(*block, block_index);
}

/* Name: UArray2b_map

 * Purpose: Iterates through the supplied unboxed array in block-major order
 * and calls the specified apply function on each element. An optional 
 * closure argument allows the user to thread a stored variable throughout
 * this function's execution.

 * Note: In block-major order, all the elements in one block are visited before 
 * visiting a new block

 * Parameters: The 2-D UArray to iterate through, pointer to apply function,
 * and pointer to closure variable.
 
 * Returns: Void

 * Expectations: 
 * Checked runtime error for the following:
 *      The given UArray2b parameter is null
 *      The given apply function is null
 */
void UArray2b_map(UArray2b_T uarray2b, 
                          void apply(int col, int row, UArray2b_T array2b,
                                     void *elem, void *cl), 
                          void *cl)
{       
        assert(uarray2b != NULL && apply != NULL);

        /* package the required pointer for mapping using the small cl struct */
        struct small_cl block_cl = { uarray2b, apply, cl };
        /* map through the block array and call block apply on each*/
        UArray2_map_row_major(uarray2b->block_array, block_apply, &block_cl);
}

/* Name: block_apply

 * Purpose: Apply function used to iterate through each element in a block.
 * Only performs the user given apply function if the current index is in
 * range.

 * Parameters: The current block column and row indices, a pointer the block 
 * array, a pointer to the current block, and a closure, which will contain
 * a small closure struct

 * Returns: void

 * Expectations: Checked runtime error if curr is ever NULL. Will apply the 
 * user given apply function to all in-range elements the current block
 */
void block_apply(int block_col, int block_row, UArray2_T uarray2, 
                 void *curr, void *cl)
{
        (void) uarray2;
        assert(curr != NULL);

        struct small_cl *block_cl = cl;
        
        UArray_T block = *((UArray_T *) curr);
        /* calculate the first index in the block in terms of the UArray2b */
        int start_col_index = block_col * block_cl->uarray2b->block_size;
        int start_row_index = block_row * block_cl->uarray2b->block_size;
        /* iterate through the block, and call apply if the element is in 
           range */
        for (int i = 0; i < UArray_length(block); i++) {
                int col_index = start_col_index + 
                                (i % block_cl->uarray2b->block_size);
                int row_index = start_row_index + 
                                (i / block_cl->uarray2b->block_size);

                if (col_index < block_cl->uarray2b->num_cols &&
                    row_index < block_cl->uarray2b->num_rows) {
                        block_cl->apply(col_index, row_index, 
                                        block_cl->uarray2b, UArray_at(block, i),
                                        block_cl->cl);
                }
        }
}



