/* Name: uarray2.h
*  Header file for UArray2, a representation of a 2-D unboxed array
*  Bradley Chao and Kevin Prou
*  October 15, 2022
*/

#ifndef U_ARRAY_2_H
#define U_ARRAY_2_H

#include <stdio.h>

typedef struct UArray2_T *UArray2_T;

/* Name: UArray2_new
 * Purpose: Creates a 2D unboxed array with dimensions and element sizes
 * specified by the three parameters. 
 
 * Parameters: Integer width (rows), height (columns), and size of the 
                                                                elements.
 
 * Returns: Pointer to newly initialized, empty unboxed array (UArray2)

 * Expectations: 
 * Check Runtime Error for the following:
 *      Any of the three parameters are null
 *      Memory allocation fails
 */
UArray2_T UArray2_new (int width, int height, 
                       int element_size);

/* Name: UArray2_free
 * Purpose: Frees the allocated memory associated with the provided unboxed
 * array container

 * Parameter: Address of a UArray2_T to be freed
 
 * Expectations: Checked Runtime Error if provided UArray pointer is null.
 */
void UArray2_free(UArray2_T *uarray2);

/* Name: UArray2_width

 * Purpose: Gets the width/number of columns of the UArray2 provided by the 
 * parameter variable

 * Parameters: A pointer to a UArray2

 * Returns: The width/number of columns in the form of an integer

 * Expectations: Checked runtime error if given UArray2 parameter is null.
 */
int UArray2_width (UArray2_T uarray2);

/* Name: UArray2_height

 * Purpose: Gets the height/number of rows of the UArray2 provided by the 
 * parameter variable
 
 * Parameters: A pointer to a UArray2

 * Returns: The height/number of rows in the form of an integer.
 
 * Expectations: Checked runtime error if given UArray2 parameter is null.
 */
int UArray2_height (UArray2_T uarray2);

/* Name: UArray2_size

 * Purpose: Gets the size of the data type stored at each index of the 2D
 * array
 
 * Parameters: A pointer to a UArray2

 * Returns: The element size in the form of an integer
 
 * Expectations: Checked runtime error if given UArray2 parameter is null.
 */
int UArray2_size (UArray2_T uarray2);

/* Name: UArray2_at

 * Purpose: Retrieves the data of the 2D array specified by the first parameter
 * at the index values provided by the second and third parameters, where the 
 * second specifies the column, and the third specifies the row.
 
 * Parameters: A pointer to a UArray2, the element's column index in the form 
 * of an integer, and and the element's row index in the form of an integer

 * Returns: A void pointer to the data specified by the parameters 
 
 * Expectations: 
 * Checked runtime error for the following:
 *      The given UArray2 parameter is null
 *      The column or row indices are outside of range of the 2D array
 */
void *UArray2_at (UArray2_T uarray2, int column, int row);

/* Name: UArray2_map_col_major

 * Purpose: Iterates through the supplied unboxed array in column-major order
 * and calls the specified apply function on each element. An optional 
 * closure argument allows the user to thread a stored variable throughout
 * this function's execution.

 * Note: In column-major order, the consecutive elemnts of a column reside 
 * next to each other.

 * Parameters: The 2D UArray to iterate through, pointer to apply function,
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
        void *cl);

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
void UArray2_map_row_major (UArray2_T uarray2, 
        void apply(int col, int row, UArray2_T uarray2, void *curr_element, 
                   void *cl),
        void *cl);

#endif