/* Name: instruction_set.c
 * Purpose: This module is responsible for executing the 32-bit word 
 * instructions, calling methods in the UM ADT to do so, abstracting runtime
 * details from the data structure itself.
 * By: Bradley Chao and Matthew Soto
 * Date: 11/16/2022
 */

#include "instruction_set.h"

/* This constant is equivalent to 2^32 and is used for modulus operation to 
   keep all arithmetic operation results in the range of 0, 2^32 - 1 */
#define mod_limit 4294967296;

/* Name: conditional_move
*  Purpose: if $r[C] != 0 then $r[A] := $r[B]
*  Parameters: Register indices
*  Returns: none
*  Effects: Checked runtime error is UM is null or
*           if A, B, or C are bigger than 8
*/
void conditional_move(universal_machine UM, UM_Reg A, UM_Reg B, UM_Reg C)
{
        assert(UM != NULL);
        assert(A < 8 && B < 8 && C < 8);

        if (get_register(UM, C) != 0) {
                uint32_t B_value = get_register(UM, B);
                set_register(UM, A, B_value);
        }
}

/* Name: segmented_load
*  Purpose: $r[A] := $m[$r[B]][$r[C]]
*  Parameters: B stores the segment ID, C stores the offset, A stores result
*  Returns: none
*  Effects: none
*/
void segmented_load(universal_machine UM, UM_Reg A, UM_Reg B, UM_Reg C)
{
        uint32_t segment_ID = get_register(UM, B);
        uint32_t offset = get_register(UM, C);

        UM_instruction word = get_instruction(UM, segment_ID, offset);
        set_register(UM, A, word);
}

/* Name: segmented_store
*  Purpose:  Segmented Store $m[$r[A]][$r[B]] := $r[C]
*  Parameters: A holds the segment ID, B stores the offset, C stores value to
*  store in UM
*  Returns: none
*  Effects: none
*/
void segmented_store(universal_machine UM, UM_Reg A, UM_Reg B, UM_Reg C)
{
        uint32_t C_value = get_register(UM, C);
        uint32_t segment_ID = get_register(UM, A);
        uint32_t offset = get_register(UM, B);

        set_instruction(UM, segment_ID, offset, C_value);
}
/* Name: addition
*  Purpose: Add registers to update one
*  Parameters: UM, A, red_B, C
*  Returns: none
*  Effects: none
*/
void addition(universal_machine UM, UM_Reg A, UM_Reg B, UM_Reg C)
{
        uint32_t B_value = get_register(UM, B);
        uint32_t C_value = get_register(UM, C);

        uint32_t sum = (B_value + C_value) % mod_limit;

        set_register(UM, A, sum);
}

/* Name: multiplication
*  Purpose: multiply registers to update one 
*  Parameters: UM, A, red_B, C
*  Returns: none
*  Effects: none
*/
void multiplication(universal_machine UM, UM_Reg A, UM_Reg B, UM_Reg C)
{
        uint32_t B_value = get_register(UM, B);
        uint32_t C_value = get_register(UM, C);

        uint32_t product = (B_value * C_value) % mod_limit;

        set_register(UM, A, product);
}

/* Name: division
*  Purpose: divide register to update one
*  Parameters: UM, A, red_B, C
*  Returns: none
*  Effects: Checked runtime error for divide by 0
*/
void division(universal_machine UM, UM_Reg A, UM_Reg B, UM_Reg C)
{
        uint32_t B_value = get_register(UM, B);
        uint32_t C_value = get_register(UM, C);

        /* (6) Can't divide by zero */
        assert(C_value != 0);

        uint32_t quotient = B_value / C_value;

        set_register(UM, A, quotient);
}

/* Name: bitwise_nand
*  Purpose: get the not of register B and C
*  Parameters: UM, A, red_B, C
*  Returns: none
*  Effects: updates register A
*/
void bitwise_nand(universal_machine UM, UM_Reg A, UM_Reg B, UM_Reg C)
{
        uint32_t B_value = get_register(UM, B);
        uint32_t C_value = get_register(UM, C);
        
        uint32_t NAND_result = ~(B_value & C_value);
        
        set_register(UM, A, NAND_result);
}

/* Name: map_segment
*  Purpose: new segment is created
*  Parameters: UM, A, red_B, C
*  Returns: none
*  Effects: new segment is created
*/
void map(universal_machine UM, UM_Reg B, UM_Reg C)
{
        uint32_t num_words = get_register(UM, C);
        uint32_t index = map_segment(UM, num_words);
        set_register(UM, B, index);
}

/* Name: unmap_segment
*  Purpose: segment $m[$r[c]] is unmapped
*  Parameters: UM, A, red_B, C
*  Returns: none
*  Effects: segment $m[$r[c]] is unmapped
*/
void unmap(universal_machine UM, UM_Reg C)
{
        uint32_t index_to_unmap = get_register(UM, C);

        unmap_segment(UM, index_to_unmap);
}

/* Name: unmap_segment
*  Purpose: segment $m[$r[c]] is unmapped
*  Parameters: UM, A, red_B, C
*  Returns: none
*  Effects: Checked runtime error if value from register c
*           is more than 255
*/
void output(universal_machine UM, UM_Reg C)
{
        uint32_t int_value = get_register(UM, C);

        /* (8) Can't output value > 255 */
        assert(int_value <= 255);

        putchar(int_value);
}

/* Name: input
*  Purpose: Universal machine awaits input from I/O devise
*  Parameters: UM, A, red_B, C
*  Returns: none
*  Effects: instruction depend on I/O
*           Checked runtime error if value is
*.          out of range (has to be between 0 and 255)
*/
void input(universal_machine UM, UM_Reg C)
{
        int int_value = getchar();

        if (int_value == EOF) {
                uint32_t all_ones = ~0;
                set_register(UM, C, all_ones);
        }
        else {
                assert(int_value >= 0 && int_value <= 255);

                set_register(UM, C, int_value);
        }
}

/* Name: load_program
*  Purpose: segment $m[$r[B]] is duplicated and replaces $m[0]
*  Parameters: UM, A, red_B, C
*  Returns: none
*  Note: Program counter is redirected in another module 
*        Checked runtime if target or duplicates are NULL 
*/
void load_program(universal_machine UM, UM_Reg B)
{
        uint32_t B_value = get_register(UM, B);

        /* Not allowed to load segment zero into segment zero */
        if (B_value != 0) {
                /* Checked runtime error if segment B_value DNE */
                segment target = (segment) Seq_get(UM->segments, B_value);
                assert(target != NULL);
                
                /* Perform deep copy of the $m[$r[B]] word instructions */
                Seq_T duplicates = Seq_new(Seq_length(target->instructions));
                assert(duplicates != NULL);

                for (int i = 0; i < Seq_length(target->instructions); i++) {
                        uint32_t word = get_instruction(UM, B_value, i);

                        Seq_addhi(duplicates, (void *) (uintptr_t) word);
                }

                /* Free instructions in segment zero and replace with new 
                   instructions */
                segment segment_zero = (segment) Seq_get(UM->segments, 0);

                Seq_free(&(segment_zero->instructions));

                segment_zero->instructions = duplicates;
        }       
}

/* Name: load_value
*  Purpose: set $r[A] to value
*  Parameters: UM, A, red_B, C
*  Returns: none
*  Effects: changes register A
*/
void load_value(universal_machine UM, UM_Reg A, uint32_t value)
{
        set_register(UM, A, value);
}