/* Name: ppmdiff.c
 * Purpose: Compares two ppm images and runs diff
 * By: Bradley Chao and Kevin Prou
 * Date: October 15, 2022
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "pnm.h"
#include "a2methods.h"
#include "a2plain.h"
#include "assert.h"
#include "uarray2.h"

bool check_dimensions(Pnm_ppm image_one, Pnm_ppm image_two);
float find_difference(Pnm_ppm image_one, Pnm_ppm image_two);
size_t smaller(size_t dim_one, size_t dim_two);
void compute_sum_squares(Pnm_rgb pixel_one, Pnm_rgb pixel_two, double *sum);

/* Name: main
 * Purpose: Driver function for ppmdiff test which computes the root mean 
 * square difference between two pixmap images
 * Parameters: The number of command line arguments and the command line itself
 * Returns: Exits success unconditionally
 * Effects: Checked runtime error if command line arguments are inappropriate
 */
int main(int argc, char *argv[])
{
        assert(argc == 3);

        /* Default to to UArray2 Methods */
        A2Methods_T methods = uarray2_methods_plain;
        assert(methods);

        /* Default to best map */
        A2Methods_mapfun *map = methods->map_default; 
        assert(map);

        FILE *file_one = fopen(argv[1], "rb");
        assert(file_one != NULL);

        FILE *file_two = fopen(argv[2], "rb");
        assert(file_two != NULL);

        /* Read both images with the default row mapping order */
        Pnm_ppm image_one, image_two;
        image_one = Pnm_ppmread(file_one, methods);
        image_two = Pnm_ppmread(file_two, methods);

        /* If width and height differences don't exceed one */
        if (check_dimensions(image_one, image_two)) {
                printf("%1.4f\n", find_difference(image_one, image_two));
        } 
        else {
                fprintf(stderr, "Inconsistent Dimensions\n");
                printf("1.0\n");
        }

        /* Free pixmaps and close file pointers */
        Pnm_ppmfree(&image_one);
        Pnm_ppmfree(&image_two);

        fclose(file_one);
        fclose(file_two);

        return EXIT_SUCCESS;
}

/* Name: find_differences
 * Purpose: Finds the root mean square difference between two pixmaps structs
 * Parameters: Pointers to both pixmap structs
 * Returns: 6-point Floating point root mean square difference 
 * Effects: Checked runtime error if either or both pointers are null 
 */
float find_difference(Pnm_ppm image_one, Pnm_ppm image_two)
{
        assert(image_one != NULL && image_two != NULL);

        size_t smaller_width = smaller(image_one->width, image_two->width);
        size_t smaller_height = smaller(image_one->height, image_two->height);

        /* Match appropriate function operator struct */
        const struct A2Methods_T *methods_one = image_one->methods;
        const struct A2Methods_T *methods_two = image_two->methods;

        double sum_squares = 0;

        /* Compute sum of squares */
        for (size_t r = 0; r < smaller_height; r++) {
                for (size_t c = 0; c < smaller_width; c++) {
                        Pnm_rgb pixel_one 
                                     = methods_one->at(image_one->pixels, c, r);
                        Pnm_rgb pixel_two 
                                     = methods_two->at(image_two->pixels, c, r);
                        
                        compute_sum_squares(pixel_one, pixel_two, &sum_squares);
                }
        }
        
        return (float) sqrt(sum_squares / (3 * smaller_width * smaller_height));
}

/* Name: compute_sum_squares
 * Purpose: Updates the sum reference variable with every loop 
 * Parameters: Pointers to the rgb structs and reference to the sum variable
 * Returns: Void
 * Effects: Checked runtime error if any of the parameters are null
 */
void compute_sum_squares(Pnm_rgb pixel_one, Pnm_rgb pixel_two, double *sum)
{
        assert(pixel_one != NULL && pixel_two != NULL);

        float max = 255;

        double red_diff = ((int) pixel_one->red - (int) pixel_two->red);
        red_diff = pow(red_diff/max, 2);

        double green_diff = ((int) pixel_one->green - (int) pixel_two->green);
        green_diff = pow(green_diff/max, 2);

        double blue_diff = ((int) pixel_one->blue - (int) pixel_two->blue);
        blue_diff = pow(blue_diff/max, 2);

        *sum += red_diff + green_diff + blue_diff;
}

/* Name: smaller
 * Purpose: Computes and returns the smaller of the two given dimensions
 * Parameters: Two unsigned integers that represents dimensions of two pixmaps
 * Returns: Returns the smaller of the two given dimensions
 * Effects: None
 */
size_t smaller(size_t dim_one, size_t dim_two)
{
        if (dim_one < dim_two) {
                return dim_one;
        }

        return dim_two;
}

/* Name: check_dimensions
 * Purpose: Compares two pixmaps and determines whether the differences in
 * dimensions are significant enough to yield an error
 * Parameters: The two pointers to pixmap structs
 * Returns: True if the dimensions are within bounds, false otherwise
 * Effects: Checked runtime error if either or both pointers are null
 */
bool check_dimensions(Pnm_ppm image_one, Pnm_ppm image_two)
{
        assert(image_one != NULL && image_two != NULL);

        int width_diff = abs((int) image_one->width - (int) image_two->width);
        int height_diff 
                       = abs((int) image_one->height - (int) image_two->height);

        if (width_diff > 1 || height_diff > 1) {
                return false;
        }

        return true;
}