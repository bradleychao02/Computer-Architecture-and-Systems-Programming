/* Name: test_bitpack.c
 * Purpose:
 * By: Bradley Chao and Kevin Prou
 * Date: October 16, 2022
 */




#include <stdio.h>
#include <stdlib.h>
#include "bitpack.h"
#include "assert.h"



/* Name: main
 * Purpose: 
 * Parameters: argc and argv 
 * Returns: 
 * Effects: 
 */
int main(int argc, char *argv[])
{
        (void) argc;
        (void) argv;

        assert(Bitpack_fitsu(12, 4));

        /* Test fit unsigned */
        assert(Bitpack_fitsu(5, 3));
        assert(!Bitpack_fitsu(5, 2));

        /* Test fit signed */
        fprintf(stderr, "signed 3 fit into 2 bits?: %d\n", 
                                                        Bitpack_fitss(3, 2));
        fprintf(stderr, "TEST2: %d\n", Bitpack_fitss(-1, 2));


        /* Test unsigned get */
        
        //General Case from Spec
        assert(Bitpack_getu(1012, 6, 2) == 61);

        // When width is max width of 64 and the word is an int max
        uint64_t max_val = ~0;
        assert(Bitpack_getu(max_val, 64, 0) == max_val);

        assert(Bitpack_getu(0, 64, 0) == 0);

        // Get goes past the most significant one of the original
        assert(Bitpack_getu(768, 3, 8) == 3);

        // Completely goes past original 
        assert(Bitpack_getu(768, 54, 10) == 0);

        //Attempting to get bits out of range
        //Bitpack_getu(768, 55, 10);

        /* Test signed get */

        //General Cases
        assert(Bitpack_gets(21, 2, 4) == 1);
        assert(Bitpack_gets(21, 3, 2) == -3);
        assert(Bitpack_gets(1012, 6, 2) == -3);
        assert(Bitpack_gets(1012, 4, 0) == 4);


        /* Test bitpack new unsigned */

        //General Cases
        assert(Bitpack_newu(768, 2, 8, 0) == 0);

        assert(Bitpack_newu(63, 6, 0, 1) == 1);

        //New value doesn't fit in width bits
        //Bitpack_newu(768, 2, 7, 4);

        /* test bitpack new signed */
        assert(Bitpack_news(768, 2, 8, -1) == 768);


       
        #define word 555
        #define  w 5
        #define lsb 6
        #define val 15
        #define w2 9
        #define lsb2 10

        /* Test Identities */
        assert(Bitpack_getu(Bitpack_newu(word, w, lsb, val), w, lsb) == val);


        assert(Bitpack_getu(Bitpack_newu(word, w, lsb, val), w2, lsb2) == 
                                                Bitpack_getu(word, w2, lsb2));

        return 0;
}