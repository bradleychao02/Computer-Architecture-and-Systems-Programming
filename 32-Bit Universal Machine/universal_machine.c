/* Name: universal_machine.c
 * This module is responsible for implementing the universal data structure
 * Notable methods include querying, writing, and mapping/unmapping values
 * from the data structure
 * Bradley Chao and Matthew Soto
 * November 18, 2022
 */

#include "universal_machine.h"

/* Name: new_UM
*  Purpose: create instance of universal machine
*  Parameters: program_instructions
*  Returns: universal machine
*  Effects: creates object and checks 
*           Checked runtime error if program_instructions, 
*           UM, or any of UM's fields are null
*           
*/
universal_machine new_UM(Seq_T program_instructions)
{
        assert(program_instructions != NULL);

        /* Allocate 56 bytes of space on heap */
        universal_machine UM = malloc(sizeof(*UM));
        assert(UM != NULL);

        /* Initialize registers as zero */
        for (size_t i = 0; i < 8; i++) {
                UM->registers[i] = 0;
        }

        /* Program counter starts at zero, allocate sequences */
        UM->program_counter = 0;

        UM->unmapped_IDs = Seq_new(100);
        assert((UM->unmapped_IDs) != NULL);

        UM->segments = Seq_new(100);
        assert((UM->segments)!= NULL);

        /* Allocate segment zero on heap */
        segment segment_zero = malloc(sizeof(*segment_zero));
        assert(segment_zero != NULL);

        /* Segment zero has now been "mapped" */
        segment_zero->valid = true;

        segment_zero->instructions = program_instructions;

        Seq_addhi(UM->segments, (void *) segment_zero);

        return UM;
}

/* Name: free_UM
 * Purpose: Frees heap allocated data associated with UM data structure
 * Parameters: Address of pointer to instance of UM
 * Returns: Void
 * Effects: Clears heap memory and checked runtime error is UM or *UM are null
 */
void free_UM(universal_machine *UM)
{
        assert(UM != NULL && *UM != NULL);

        /* Stores a pointer to the UM struct on the stack */
        universal_machine stack_copy = *UM;

        for (int i = 0; i < Seq_length(stack_copy->segments); i++) {

                /* Frees the instruction sequence member of segment struct
                   Avoid double freeing by checking whether the segment has
                   been unmapped already */
                segment target = (segment) Seq_get(stack_copy->segments, i);
                if (target->valid) {
                        Seq_free(&((
                                (segment) Seq_get(stack_copy->segments, i))
                                                        ->instructions));
                }
                
                /* Frees the malloced pointer to the segment struct */
                free((segment) Seq_get(stack_copy->segments, i));
        }

        /* Frees the sequence of 32-bit IDs */
        Seq_free(&(stack_copy->unmapped_IDs));

        /* Frees the container of the sequence of segments */
        Seq_free(&(stack_copy->segments));

        /* Frees malloced pointer to the UM struct */
        free(stack_copy);
}

/* Name: get_instruction
*  Purpose: get instruction based on ID and offset
*  Parameters: UM, ID, offset
*  Returns: UM_instruction
*  Effects: Checked runtime error if segment ID or offset is out of bounds and
*  if the segment is invalid
*/
UM_instruction get_instruction(universal_machine UM, uint32_t ID, 
                                                        uint32_t offset)
{
        assert(UM != NULL);
        
        /* (3) Check whether ID is within bounds of addressable segments */
        assert(ID < (uint32_t) Seq_length(UM->segments));

        /* Hanson assertion will occur if there is a get on allocated 
           segment space but has not been initialized to value */
        segment seg = (segment) Seq_get(UM->segments, ID);

        /* (7) If segment has not been mapped, checked runtime error */
        assert((seg->valid));

        /* (4) Check that the offset is within bounds of the segment */
        assert(offset < (uint32_t) Seq_length(seg->instructions));

        /* Implicit cast 32-bit uintptr_t to UM_instruction */
        /* Hanson assertion will occur if get is called on allocated segment
           which has not been allocated */
        UM_instruction instruction = (uintptr_t) 
                                        Seq_get(seg->instructions, offset);

        return instruction;
}

/* Name: set_instruction
*  Purpose: set instruction in the UM based on ID and offset
*  Parameters: UM, ID, offset, instruction
*  Returns: none
*  Effects: Checked runtime error if segment or offset is out of bounds or
*  has been unmapped
*/
void set_instruction(universal_machine UM, uint32_t ID, uint32_t offset, 
                                                UM_instruction instruction)
{
        assert(UM != NULL);

        assert(ID < (uint32_t) Seq_length(UM->segments));

        segment seg = (segment) Seq_get(UM->segments, ID);

        assert((seg->valid));

        assert(offset < (uint32_t) Seq_length(seg->instructions));

        Seq_put(seg->instructions, offset, (void *) (uintptr_t) instruction);
}

/* Name: get_register
*  Purpose: get value of register
*  Parameters: UM, register_ID
*  Returns: uint32_t
*  Effects: will give error if UM is Null or Register ID is more than 8
*/
uint32_t get_register(universal_machine UM, uint32_t register_ID)
{
        assert(UM != NULL);
        assert(register_ID < 8);

        return (UM->registers)[register_ID];
}

/* Name: set_register
*  Purpose: assing a value to a register
*  Parameters: UM, register_ID, value
*  Returns: none
*  Effects: Checked runtime error if UM is null or register ID is more than 8
*/
void set_register(universal_machine UM, uint32_t register_ID, uint32_t value)
{
        assert(UM != NULL);
        assert(register_ID < 8);

        (UM->registers)[register_ID] = value;
}

/* Name: map_segment
*  Purpose: A new segment is created with a number of
*  words equal to the value in $r[C]. Each word in
*  the new segment is initialized to 0. A bit pattern
*  that is not all zeroes and that does not identify
*  any currently mapped segment is placed in $r[B].
*  The new segment is mapped as $m[$r[B]].
*  Parameters: The segment length
*  Returns: Index of newly mapped segment
*  Effects: Checked runtime error is UM, UM_instructions are null
*/
uint32_t map_segment(universal_machine UM, uint32_t segment_length)
{
        assert(UM != NULL);

        Seq_T new_instructions = Seq_new(segment_length);
        assert(new_instructions != NULL);

        /* Initialize all word members to be zero */
        UM_instruction zero_word = 0;

        for (size_t i = 0; i < segment_length; i++) {
                Seq_addhi(new_instructions, (void *) (uintptr_t) zero_word);
        }

        /* Case 1: If there are no unmapped IDs */
        if (Seq_length(UM->unmapped_IDs) == 0) {

                /* Allocate new segment with all words initialized to zero */
                segment new_segment = malloc(sizeof(*new_segment));
                assert(new_segment != NULL);

                new_segment->valid = true;
                new_segment->instructions = new_instructions;

                /* Enqueue the new segment */
                Seq_addhi(UM->segments, (void *) new_segment);

                /* The index would be the last segment element */
                return Seq_length(UM->segments) - 1;
        }
        /* Case 2: There are unmapped IDs available for use */
        else {
                /* Dequeue next unmapped ID and map the new segment there */
                uint32_t segment_ID = (uint32_t) (uintptr_t) 
                                                Seq_remlo(UM->unmapped_IDs);

                segment to_replace = (segment) 
                                        Seq_get(UM->segments, segment_ID);
                assert(to_replace != NULL);

                to_replace->valid = true;
                /* Delegated the freeing the instructions to unmap */
                to_replace->instructions = new_instructions;

                return segment_ID;
        }
}

/* Name: unmap_segment
*  Purpose: The segment $m[$r[c]] is unmapped. Future Map
*  future map instructions may reuse the identifier $r[c]
*  Parameters: Segment ID to unmap
*  Returns: Void
*  Effects: Checked runtime error if unmap is called on an out of bounds
*  segment or a segment that has already been unmapped
*/
void unmap_segment(universal_machine UM, uint32_t segment_ID)
{
        assert(UM != NULL);
        
        /* (5) Can't unmap segment zero */
        assert(segment_ID != 0);

        /* Get the targeted segment to unmap */
        /* (5) Hanson Seq checked runtime error if segment ID DNE */
        segment unmapped_segment = (segment) Seq_get(UM->segments, segment_ID);
        assert(unmapped_segment != NULL);

        /* Cannot unmap a segment that is invalid */
        assert(unmapped_segment->valid);

        /* Set boolean to false so user cannot re-access this location */
        unmapped_segment->valid = false;

        /* Free the data associated with this segment */
        Seq_free(&(unmapped_segment->instructions));

        /* This index in memory is no available for new use */
        Seq_addhi(UM->unmapped_IDs, (void *) (uintptr_t) segment_ID);
}