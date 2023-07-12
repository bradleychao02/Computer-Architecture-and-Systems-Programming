/* Name: uarray2.c
 * Implementation for UArray2
 * Bradley Chao and Kevin Prou
 * October 15, 2022
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <uarray.h>
#include <assert.h>
#include "uarray2.h"

/* UArray2_T struct

 * Components: 
 *    1) Pointer Hanson UArray which is a one dimensional representation of 
 *       the two dimensional array. All the "rows" of a normal 2D array are 
 *       linked together one dimensionally
 *    2) num_cols refers to the number of columns 
 *    3) num_rows refers to the number of rows 
 *    4) elem_size refers to the size of the elements store in bytes
 */
struct UArray2_T {
        UArray_T uarray;
        int num_cols;
        int num_rows;
        int elem_size;
};

/* Name: UArray2_new

 * Purpose: Creates a 2D unboxed array with dimensions and element sizes
 * specified by the three parameters. Memory for all pointees is allocated by
 * the unboxed array container.
 
 * Parameters: Integer width (columns), height (rows), and size of the 
               elements.
 
 * Returns: Pointer to newly initialized, empty unboxed array (UArray2)

 * Expectations: 
 * Check Runtime Error for the following:
 *      Any of the three parameter values are less than or equal to zero
 *      Memory allocation fails
 */
UArray2_T UArray2_new(int width, int height, int element_size)
{
        assert(width >= 0 && height >= 0 && element_size >= 0);
        UArray2_T uarray2 = malloc(sizeof(struct UArray2_T));
        assert(uarray2 != NULL);

        uarray2->uarray = UArray_new(width * height, element_size);
        uarray2->num_cols = width;
        uarray2->num_rows = height;
        uarray2->elem_size = element_size;

        return uarray2;
}

/* Name: UArray2_free

 * Purpose: Frees the allocated memory associated with the provided unboxed
 * array container. Unboxed arrays do not need to worry about freeing pointees.

 * Parameters: Address of pointer to UArray2_T struct to free the underlying
 * data as well as the outer pointer

 * Returns: Void
 
 * Expectations: Checked Runtime Error if provided UArray pointer is null.
 */
void UArray2_free(UArray2_T *uarray2)
{       
        /*Make sure pointer to 2D struct and the underlying 1D data are valid*/
        assert((*uarray2)->uarray != NULL && uarray2 != NULL);
        UArray_free(&((*uarray2)->uarray));
        free(*uarray2);
}

/* Name: UArray2_width

 * Purpose: Gets the width/number of columns of the UArray2 provided by the 
 * parameter variable.

 * Parameters: A pointer to a UArray2.

 * Returns: The width/number of columns in the form of an integer.

 * Expectations: Checked runtime error if given UArray2 parameter is null.
 */
int UArray2_width(UArray2_T uarray2)
{
        assert(uarray2 != NULL);
        return uarray2->num_cols;
}

/* Name: UArray2_height

 * Purpose: Gets the height/number of rows of the UArray2 provided by the 
 * parameter variable.
 
 * Parameters: A pointer to a UArray2.

 * Returns: The height/number of rows in the form of an integer.
 
 * Expectations: Checked runtime error if given UArray2 parameter is null.
 */
int UArray2_height(UArray2_T uarray2)
{
        assert(uarray2 != NULL);
        return uarray2->num_rows;
}

/* Name: UArray2_size

 * Purpose: Gets the size of the data type stored at each index of the 2D
 * array
 
 * Parameters: A pointer to a UArray2.

 * Returns: The element size in the form of an integer.
 
 * Expectations: Checked runtime error if given UArray2 parameter is null.
 */
int UArray2_size(UArray2_T uarray2)
{
        assert(uarray2 != NULL);
        return uarray2->elem_size;
}

/* Name: UArray2_at

 * Purpose: Retrieves the data of the 2D array specified by the first parameter
 * at the index values provided by the second and third parameters, where the 
 * second specifies the column, and the third specifies the row.
 
 * Parameters: A pointer to a UArray2, the element's column index in the form 
 * of an integer, and the element's row index in the form of an integer

 * Returns: A void pointer to the data specified by the parameters. 
 
 * Expectations: 
 * Checked runtime error for the following:
 *      The given UArray2 parameter is null
 *      The column or row indices are outside of range of the 2D array
 */
void *UArray2_at(UArray2_T uarray2, int column, int row)
{
        assert(uarray2 != NULL);
        assert(column >= 0 && row >= 0);

        if (column >= UArray2_width(uarray2) || row >= UArray2_height(uarray2)) 
        {
                printf("ERROR: COL: %d, ROW: %d\n", column, row);
        }

        assert(column < UArray2_width(uarray2) 
               && row < UArray2_height(uarray2));


        /* Finds and returns corresponding 1D index from row and col */
        return UArray_at(uarray2->uarray, (uarray2->num_cols * row) + column);
}

/* Name: UArray2_map_col_major

 * Purpose: Iterates through the supplied unboxed array in column-major order
 * and calls the specified apply function on each element. An optional 
 * closure argument allows the user to thread a stored variable throughout
 * this function's execution.

 * Note: In column-major order, the consecutive elemnts of a column reside 
 * next to each other.

 * Parameters: The 2-D UArray to iterate through, pointer to apply function,
 * and pointer to closure variable.
 *
 * Returns: Void
 * Expectations: 
 * Checked runtime error for the following:
 *      The given UArray2 parameter is null
 *      The given apply function is null
 */
void UArray2_map_col_major (UArray2_T uarray2, 
        void apply(int col, int row, UArray2_T uarray2, void *curr_element, 
                   void *cl),
        void *cl)
{
        assert(uarray2 != NULL && apply != NULL);

        for (int c = 0; c < uarray2->num_cols; c++) {
                for (int r = 0; r < uarray2->num_rows; r++) {
                        apply(c, r, uarray2, UArray2_at(uarray2, c, r), cl);
                }
        }
}

/* Name: UArray2_map_row_major

 * Purpose: Iterates through the supplied unboxed array in row-major order
 * and calls the specified apply function on each element. An optional 
 * closure argument allows the user to thread a stored variable throughout
 * this function's execution.

 * Note: In row-major order, the consecutive elemnts of a row reside next to 
 * each other.

 * Parameters: The 2-D UArray to iterate through, pointer to apply function,
 * and pointer to closure variable.
 *
 * Returns: Void
 * Expectations: 
 * Checked runtime error for the following:
 *      The given UArray2 parameter is null
 *      The given apply function is null
 */
void UArray2_map_row_major(UArray2_T uarray2, 
        void apply(int col, int row, UArray2_T uarray2, void *curr_element, 
                   void *cl),
        void *cl)
{       
        assert(uarray2 != NULL && apply != NULL);

        for (int r = 0; r < uarray2->num_rows; r++) {
                for (int c = 0; c < uarray2->num_cols; c++) {
                        apply(c, r, uarray2, UArray2_at(uarray2, c, r), cl);
                }
        }
}