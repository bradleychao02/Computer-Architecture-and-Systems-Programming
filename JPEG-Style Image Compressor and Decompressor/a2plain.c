/* Name: a2plain.c
*  Purpose: Implementation of the interface methods generalized by A2
*  Bradley Chao and Kevin Prou
*  October 13, 2022
*/

#include <string.h>
#include <a2plain.h>
#include "uarray2.h"

/************************************************/
/* Define a private version of each function in */
/* A2Methods_T that we implement.               */
/************************************************/

/* Name: new
*  Purpose: Constructs and returns new uarray2
*  Parameters: The width, height, and size
*  Returns: New UArray2 with appropriate dimensions and size
*  Effects: Checked runtime error for invalid dimensions
*/
static A2Methods_UArray2 new(int width, int height, int size)
{
        return UArray2_new(width, height, size);
}

/* Name: new_with_blocksize
*  Purpose: Constructs and returns new UArray but with a block size
*  Parameters: Width, height, size, blocksize
*  Returns: New UArray2 with appropraite dimensions
*  Effects: Checked runtime error for invalid dimensions
*/
static A2Methods_UArray2 new_with_blocksize(int width, int height, int size, 
                                            int blocksize)
{
        (void) blocksize;
        return UArray2_new(width, height, size);
}

/* Name: a2free
*  Purpose: Free the current array2
*  Parameters: Address of a pointer to UArray2 to free its value
*  Returns: Void
*  Effects: Checked runtime error if the array to free is null
*/
static void a2free(A2Methods_UArray2 *uarray2p)
{
        UArray2_free((UArray2_T *) uarray2p);
}

/* Name: width
*  Purpose: Obtain width of the uarray2
*  Parameters: Address of the uarray2
*  Returns: The width of the uarray2
*  Effects: Checked runtime error if array is null
*/
static int width(A2Methods_UArray2 uarray2)
{
        return UArray2_width(uarray2);
}

/* Name: height
*  Purpose: Obtain height of the uarray2
*  Parameters: Address of the uarray2
*  Returns: The height of the uarray2
*  Effects: Chekced runtime error if array is null
*/
static int height(A2Methods_UArray2 uarray2)
{
        return UArray2_height(uarray2);
}

/* Name: size
*  Purpose: Obtain size of the uarray2
*  Parameters: Address of the uarray2
*  Returns: The size of the uarray2
*  Effects: Chekced runtime error if array is null
*/
static int size(A2Methods_UArray2 uarray2)
{
        return UArray2_size(uarray2);
}

/* Name: blocksize
*  Purpose: Obtain blocksize of the uarray2
*  Parameters: Address of the uarray2
*  Returns: The blocksize of the uarray2
*  Effects: Chekced runtime error if array is null
*/
static int blocksize(A2Methods_UArray2 uarray2)
{
        (void) uarray2;
        return 1;
}

/* Name: at
*  Purpose: Obtain the object at a certain column and row index
*  Parameters: Address of the uarray2, column, and row
*  Returns: Pointer to the A2Method object
*  Effects: Chekced runtime error if array is null and index out of bounds
*/
static A2Methods_Object *at(A2Methods_UArray2 uarray2, int col, int row)
{
        return UArray2_at(uarray2, col, row);
}

typedef void UArray2_applyfun(int col, int row, UArray2_T uarray2, void *curr,
                              void *cl);

/* Name: map_row_major
*  Purpose: Mapping function to traverse the given UArray2
*  Parameters: The uarray2 to traverse, the apply function, and threaded 
*              closure
*  Returns: Void
*  Effects: Checked runtime error if uarray2 is null
*/
static void map_row_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        UArray2_map_row_major(uarray2, (UArray2_applyfun*)apply, cl);
}

/* Name: map_col_major
*  Purpose: Mapping function to traverse the given UArray2
*  Parameters: The uarray2 to traverse, the apply function, and threaded 
*              closure
*  Returns: Void
*  Effects: Checked runtime error if uarray2 is null
*/
static void map_col_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        UArray2_map_col_major(uarray2, (UArray2_applyfun*)apply, cl);
}

struct small_closure {
        A2Methods_smallapplyfun *apply; 
        void *cl;
};

/* Name: apply_small
*  Purpose: Mapping function to traverse the given UArray2
*  Parameters: The uarray2 to traverse, the apply function, and threaded 
*              closure
*  Returns: Void
*  Effects: Checked runtime error if uarray2 is null
*/
static void apply_small(int i, int j, UArray2_T uarray2,
                        void *elem, void *vcl)
{
        struct small_closure *cl = vcl;
        (void)i;
        (void)j;
        (void)uarray2;
        cl->apply(elem, cl->cl);
}


/* Name: small_map_row_major
*  Purpose: Mapping function to traverse the given UArray2
*  Parameters: The uarray2 to traverse, the apply function, and threaded 
*              closure
*  Returns: Void
*  Effects: Checked runtime error if uarray2 is null
*/
static void small_map_row_major(A2Methods_UArray2 a2,
                                A2Methods_smallapplyfun apply,
                                void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_row_major(a2, apply_small, &mycl);
}


/* Name: small_map_col_major
*  Purpose: Mapping function to traverse the given UArray2
*  Parameters: The uarray2 to traverse, the apply function, and threaded 
*              closure
*  Returns: Void
*  Effects: Checked runtime error if uarray2 is null
*/
static void small_map_col_major(A2Methods_UArray2 a2,
                                A2Methods_smallapplyfun apply,
                                void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_col_major(a2, apply_small, &mycl);
}

/* This struct contains pointers to all the functions */
static struct A2Methods_T uarray2_methods_plain_struct = {
        new,
        new_with_blocksize,
        a2free, 
        width,
        height,
        size,
        blocksize,
        at, 
        map_row_major,
        map_col_major,
        NULL,
        map_row_major,
        small_map_row_major,
        small_map_col_major,
        NULL,
        small_map_row_major,
};

/* finally the payoff: here is the exported pointer to the struct */

A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;