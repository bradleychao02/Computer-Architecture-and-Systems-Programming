/* Name: bitpack.c
 * Purpose: handles the transformation going both ways
 * between the field components and the single-byte words
 * By: Bradley Chao and Kevin Prou
 * Date: October 16, 2022
 */

#include "bitpack.h"
#include "assert.h"
#include <except.h>
#include <stdio.h>

Except_T Bitpack_Overflow = { "Overflow packing bits" };

/* Name: unsigned_shift_left
 * Purpose: Private helper function for shifting left by shift bits
 * Parameters: Word to shift left and magnitude of shift
 * Returns: New 64-bit integer with shift 
 * Effects: Checked runtime error if shift is greater than 64
 */
uint64_t unsigned_shift_left(uint64_t word, unsigned shift)
{
        assert(shift <= 64);

        if (shift == 64) return 0;

        uint64_t shifted_word = word << shift;

        return shifted_word;
}

/* Name: unsigned_shift_right
 * Purpose: Private helper function for shifting right by shift bits
 * Parameters: Word to shift right and magnitude of shift
 * Returns: New 64-bit integer with shift
 * Effects: Checked runtime error if shift is greater than 64
 */
uint64_t unsigned_shift_right(uint64_t word, unsigned shift)
{
        assert(shift <= 64);

        if (shift == 64) return 0;

        uint64_t shifted_word = word >> shift;

        return shifted_word;

}

/* Name: signed_shift_left
 * Purpose: Private helper function for shifting a signed integer left by shift
 * Parameters: Signed int to shift and shift magnitude
 * Returns: New signed integer with shift
 * Effects: Checked runtime error if shift is greater than 64
 */
int64_t signed_shift_left(int64_t word, unsigned shift)
{
        assert(shift <= 64);

        if (shift == 64) return 0;

        int64_t shifted_word = word << shift;

        return shifted_word;
}

/* Name: Bitpack_fitsu
 * Purpose: Determines whether unsigned integer n can be represented in width
 * bits
 * Parameters: Unsigned integer and width
 * Returns: True if integer can fit false otherwise
 * Effects: Checked runtime error if width is greater than 64
 */
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        assert(width <= 64);

        uint64_t max_val = ~0;
        max_val = unsigned_shift_left(max_val, width);

        /* Everything is 0 except width bits, which is the max val */
        max_val = ~max_val;

        if (n <= max_val) return true;
        
        return false;
}

/* Name: Bitpack_fitss
 * Purpose: Checks whether signed integer n can fit in width bits
 * Parameters: Signed integer and width
 * Returns: True if integer can fit false otherwise
 * Effects: Checked runtime error if width is greater than 64
 */
bool Bitpack_fitss(int64_t n, unsigned width)
{
        assert(width <= 64);

        /* Obtains max negative 2's complement */
        int64_t max_neg = ~0;
        max_neg = signed_shift_left(max_neg, width - 1);

        int64_t max_pos = ~0;
        max_pos = unsigned_shift_left(max_pos, width - 1);
        max_pos = ~max_pos;

        if (n >= max_neg && n <= max_pos) return true;

        return false;
}

/* Name: Bitpack_getu
 * Purpose: Obtains a bit substring of given unsigned word starting from lsb 
 * and taking the next width - 1 values and returns it as an unsigned integer
 * Parameters: The whole word, the width, and least significant bit
 * Returns: The bit substring as an unsigned integer
 * Effects: Checked runtime error if bit substring is out of range
 */
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= 64);
        assert(width + lsb <= 64);

        /* Spec: Fields of width zero are defined to contain the value zero */
        if (width == 0) return 0;

        uint64_t mask = ~0;
       
        /* Isolate width number of 1's */
        mask = unsigned_shift_left(mask, 64 - width);

        /* Shift width 1's into the location of the word */
        mask = unsigned_shift_right(mask, 64 - width - lsb);

        /* Obtain original width bit values */
        word = mask & word;

        /* Adjust for trailing zeros */
        word = unsigned_shift_right(word, lsb);

        return word;
}

/* Name: Bitpack_gets
 * Purpose: Obtains a bit substring of a given signed word starting from lsb
 * and taking the next width - 1 values and return it as a signed integer
 * Parameters: The whole word, the width, and least significant bit
 * Returns: The bit substring as a signed integer
 * Effects: Checked runtime error if bit substring is out of range
 */
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{       
        assert(width <= 64);
        assert(width + lsb <= 64);

        if (width == 0) return 0;

        /* Obtain string of interest */
        word = Bitpack_getu(word, width, lsb);

        /* If leading bit of the word is a 1 it's negative */
        if (Bitpack_getu(word, 1, width - 1) == 1)
        {
                /* Lay 1's in front */
                int64_t mask = signed_shift_left(~0, width);

                word = mask | word;

                return (int64_t) word;
        }
        
        return (int64_t) word;
}

/* Name: Bitpack_newu
 * Purpose: Replaces bit index lsb to lsb + width - 1 with value and returns it
 * as a new unsigned integer
 * Parameters: The whole word, width, least significant bit, and replace value
 * Returns: New replaced unsigned integer
 * Effects: Checked runtime error if bit substring is out of bounds and 
 * overflow exception if value cannot fit in width bits
 */
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, 
                                                        uint64_t value)
{
        assert(width <= 64);
        assert(width + lsb <= 64);

        /* If value can't fit within width raise error */
        if (!Bitpack_fitsu(value, width))
        {
                RAISE(Bitpack_Overflow);
                return 0;
        }
        else
        {
              /* Nullify the bits of interest to 0 */
              uint64_t bits_to_replace = Bitpack_getu(word, width, lsb);
              bits_to_replace = unsigned_shift_left(bits_to_replace, lsb);
              word = word ^ bits_to_replace;

              return word | unsigned_shift_left(value, lsb);
        }
}

/* Name: Bitpack_news
 * Purpose: Replaces bit index lsb to lsb + width - 1 with value and returns it
 * as a new signed integer
 * Parameters: The whole word, width, least significant bit, and replace value
 * Returns: New replaced signed integer
 * Effects: Checked runtime error if bit substring is out of bounds and 
 * overflow exception if value cannot fit in width bits
 */
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, 
                                                        int64_t value)
{
        assert(width <= 64);
        assert(width + lsb <= 64);

        if (!Bitpack_fitss(value, width))
        {
                RAISE(Bitpack_Overflow);
                return 0;
        }
        else
        {
                /* Nullify the bits of interest to 0 */
                uint64_t bits_to_replace = Bitpack_getu(word, width, lsb);
                bits_to_replace = unsigned_shift_left(bits_to_replace, lsb);
                word = word ^ bits_to_replace;

                /* Get rid of leading 1s for 2's complement */
                uint64_t mask = Bitpack_getu(value, width, 0);
                
                return word | unsigned_shift_left(mask, lsb);
        }
}