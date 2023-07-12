/* Name: universal_machine.h
 * Interface for universal_machine.c
 * Bradley Chao and Matthew Soto
 * November 18, 2022
 */


#ifndef UNIVERSAL_MACHINE
#define UNIVERSAL_MACHINE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <seq.h>
#include <uarray.h>
#include <assert.h>
#include <stdbool.h>

typedef uint32_t UM_instruction;

typedef struct universal_machine {
        uint32_t registers[8]; /* pointer to first element */
        uint32_t program_counter;
        Seq_T unmapped_IDs;
        Seq_T segments; /* UArray of segment */
} *universal_machine;

typedef struct segment {
        bool valid; /* Tracks whether the segment is valid/mapped */
        Seq_T instructions;
} *segment;

universal_machine new_UM(Seq_T segment_zero);
void free_UM(universal_machine *UM);

UM_instruction get_instruction(universal_machine UM, uint32_t ID,
                                 uint32_t offset);
void set_instruction(universal_machine UM, uint32_t ID, uint32_t offset,
                         UM_instruction instruction);

uint32_t get_register(universal_machine UM, uint32_t register_ID);
void set_register(universal_machine UM, uint32_t register_ID,
                 UM_instruction instruction);

uint32_t map_segment(universal_machine UM, uint32_t segment_length);
void unmap_segment(universal_machine UM, uint32_t segment_ID);

#endif