/*
 * umlab.c
 *
 * Functions to generate UM unit tests. Once complete, this module
 * should be augmented and then linked against umlabwrite.c to produce
 * a unit test writing program.
 *  
 * A unit test is a stream of UM instructions, represented as a Hanson
 * Seq_T of 32-bit words adhering to the UM's instruction format.  
 * 
 * Any additional functions and unit tests written for the lab go
 * here. 
 *  
 */

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <seq.h>
#include <bitpack.h>

typedef uint32_t Um_instruction;
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;


/* Functions that return the two instruction types */
Um_instruction three_register(Um_opcode op, int ra, int rb, int rc);
Um_instruction loadval(unsigned ra, unsigned val);


/* Wrapper functions for each of the instructions */
static inline Um_instruction halt(void) 
{
        return three_register(HALT, 0, 0, 0);
}

typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;

static inline Um_instruction add(Um_register a, Um_register b, Um_register c)
{
        return three_register(ADD, a, b, c);
}

Um_instruction output(Um_register c)
{
        return three_register(OUT, 0, 0, c);
}

/* Functions for working with streams */

static inline void append(Seq_T stream, Um_instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_addhi(stream, (void *) (uintptr_t)inst);
}

void print_new_line(Seq_T stream)
{
        append(stream, loadval(r0, 10));
        append(stream, output(r0));
}


const uint32_t Um_word_width = 32;

void Um_write_sequence(FILE *output, Seq_T stream)
{
        assert(output != NULL && stream != NULL);
        int stream_length = Seq_length(stream);
        for (int i = 0; i < stream_length; i++) {
                Um_instruction inst = (uintptr_t)Seq_remlo(stream);
                for (int lsb = Um_word_width - 8; lsb >= 0; lsb -= 8) {
                        fputc(Bitpack_getu(inst, 8, lsb), output);
                }
        }
      
}

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc)
{
        Um_instruction ret = 0;
        uint32_t A = ra;
        uint32_t B = rb;
        uint32_t C = rc;

        ret = Bitpack_newu(ret, 3, 0, C);
        ret = Bitpack_newu(ret, 3, 3, B);
        ret = Bitpack_newu(ret, 3, 6, A);
        ret = Bitpack_newu(ret, 4, 28, op);

        return ret;
}

Um_instruction loadval(unsigned ra, unsigned val)
{
        Um_instruction ret = 0;
        uint32_t A = ra;
        uint32_t value = val;

        ret = Bitpack_newu(ret, 25, 0, value);
        ret = Bitpack_newu(ret, 3, 25, A);
        ret = Bitpack_newu(ret, 4, 28, LV);

        return ret;
}

Um_instruction conditional_move(unsigned ra, unsigned rb, unsigned rc)
{
        return three_register(CMOV, ra, rb, rc);
}

Um_instruction segmented_load(unsigned ra, unsigned rb, unsigned rc)
{
        return three_register(SLOAD, ra, rb, rc);
}

Um_instruction segmented_store(unsigned ra, unsigned rb, unsigned rc)
{
        return three_register(SSTORE, ra, rb, rc);
}

Um_instruction addition(unsigned ra, unsigned rb, unsigned rc)
{
        return three_register(ADD, ra, rb, rc);
}

Um_instruction multiplication(unsigned ra, unsigned rb, unsigned rc)
{
        return three_register(MUL, ra, rb, rc);
}

Um_instruction division(unsigned ra, unsigned rb, unsigned rc)
{
        return three_register(DIV, ra, rb, rc);
}

Um_instruction bitwise_NAND(unsigned ra, unsigned rb, unsigned rc)
{
        return three_register(NAND, ra, rb, rc);
}

Um_instruction map_segment(unsigned rb, unsigned rc)
{
        return three_register(ACTIVATE, 0, rb, rc);
}

Um_instruction unmap_segment(unsigned rc)
{
        return three_register(INACTIVATE, 0, 0, rc);
}

Um_instruction input(unsigned rc)
{
        return three_register(IN, 0, 0, rc);
}

Um_instruction load_program(unsigned rb, unsigned rc)
{
        return three_register(LOADP, 0, rb, rc);
}

/* Unit tests for the UM */

void build_halt_test(Seq_T stream)
{
        append(stream, halt());
}

void build_verbose_halt_test(Seq_T stream)
{
        append(stream, halt());
        append(stream, loadval(r1, 'B'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'a'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'd'));
        append(stream, output(r1));
        append(stream, loadval(r1, '!'));
        append(stream, output(r1));
        append(stream, loadval(r1, '\n'));
        append(stream, output(r1));
}

void build_add_two_numbers(Seq_T stream)
{
        append(stream, add(r1, r2, r3));
        append(stream, halt());
}

void build_print_digit(Seq_T stream)
{
        append(stream, loadval(r1, 48));
        append(stream, loadval(r2, 6));

        /* 48 + 6 = 54 */
        append(stream, add(r3, r1, r2));

        /* output result as char */
        append(stream, output(r3));
        append(stream, halt());
}

/* Start Our Testing */

void build_IO(Seq_T stream)
{
        /* Inputs and outputs user input */
        append(stream, input(r0));
        append(stream, output(r0));
        append(stream, halt());
}

void build_cond_move(Seq_T stream)
{
        /* 0 Case */
        append(stream, loadval(r1, 68));
        append(stream, loadval(r2, 69)); 
        append(stream, loadval(r3, 0));
        append(stream, conditional_move(r1, r2, r3));   
        append(stream, output(r1)); 
        append(stream, output(r2));
        append(stream, output(r3));  
        print_new_line(stream);

        /* Non zero case */
        append(stream, loadval(r1, 68));
        append(stream, loadval(r2, 69)); 
        append(stream, loadval(r3, 48));
        append(stream, conditional_move(r1, r2, r3));   
        append(stream, output(r1)); 
        append(stream, output(r2));
        append(stream, output(r3));  

        print_new_line(stream);

        append(stream, halt());
}

void build_arithmetic(Seq_T stream)
{
        /* Addition */
        append(stream, loadval(r0, 50));
        append(stream, loadval(r1, 52));
        append(stream, addition(r2, r0, r1));
        append(stream, output(r2));

        /* Multiplication */
        append(stream, loadval(r0, 10));
        append(stream, loadval(r1, 10));
        append(stream, multiplication(r2, r0, r1));
        append(stream, output(r2));

        /* Test Modulo */
        /* 7924294 * 542 (mod 2^32) = 52 Ascii Dec = 4 Char */
        append(stream, loadval(r0, 7924294));
        append(stream, loadval(r1, 542));
        append(stream, multiplication(r3, r0, r1));
        append(stream, output(r3));

        /* Division */
        append(stream, loadval(r0, 500));
        append(stream, loadval(r1, 10));
        append(stream, division(r2, r0, r1));
        append(stream, output(r2));

        print_new_line(stream);

        append(stream, halt());
}

void build_NAND(Seq_T stream)
{
        /* ~(0 $ 0) gives bitwise INT_MAX */
        append(stream, loadval(r1, 0));
        append(stream, loadval(r2, 0));
        append(stream, bitwise_NAND(r3, r1, r2));

        append(stream, loadval(r4, 50));
        append(stream, add(r3, r3, r4));

        append(stream, output(r3));

        print_new_line(stream);
        append(stream, halt()); 
}

void build_map(Seq_T stream)
{
        append(stream, loadval(r1, 15));
        /* Map a new segment at index 1 with size 15, r0 stores 1 */
        append(stream, map_segment(r0, r1));

        /* $r[2] = 1, $r[4] = 2 */
        append(stream, loadval(r2, 1));
        append(stream, loadval(r4, 2));

        /* Store ASCII Dec 55/Char 7 at $m[$r[2]][$r[4]] */
        append(stream, loadval(r5, 55));
        append(stream, segmented_store(r2, r4, r5));

        append(stream, segmented_load(r6, r2, r4));

        append(stream, output(r6));

        print_new_line(stream);

        append(stream, loadval(r1, 15));
        append(stream, map_segment(r0, r1));
        append(stream, loadval(r2, 2));
        append(stream, loadval(r4, 2));
        append(stream, loadval(r5, 55));
        append(stream, segmented_store(r2, r4, r5));
        append(stream, segmented_load(r6, r2, r4));
        append(stream, output(r6));

        print_new_line(stream);
        
        append(stream, halt()); 
}

void build_unmap(Seq_T stream)
{
        append(stream, loadval(r1, 2));
        append(stream, loadval(r6, 4));
        append(stream, map_segment(r2, r1));
        append(stream, map_segment(r3, r6));

        append(stream, loadval(r5, 48));

        append(stream, addition(r6, r2, r5));
        append(stream, addition(r7, r3, r5));

        /* These are the values of r2 and r3 + 50 for legibility */
        append(stream, output(r6));
        append(stream, output(r7));

        /* Unmaps the segments that were mapped */
        append(stream, unmap_segment(r2));
        append(stream, unmap_segment(r3));

        print_new_line(stream);

        append(stream, halt());
}

void build_store_load(Seq_T stream)
{
        append(stream, loadval(r2, 10));
        append(stream, loadval(r3, 9));
        append(stream, loadval(r4, 35));

        /* Create 2 new segments of size $r[2] (10) and store ID in r1 (1) */
        append(stream, map_segment(r1, r2));
        append(stream, map_segment(r1, r2));

        /* Store 35 into $m[$r[1]][$r[3]] */
        append(stream, segmented_store(r1, r3, r4));

        /* Loads 35 into r5 from $m[$r[1]][$r[3]] */
        append(stream, segmented_load(r5, r1, r3));

        append(stream, output(r5));

        append(stream, loadval(r6, 2));
        append(stream, segmented_store(r6, r3, r4));
        append(stream, segmented_load(r5, r6, r3));

        append(stream, output(r5));

        print_new_line(stream);

        append(stream, halt());
}

void build_load_program(Seq_T stream)
{
        /* This one just tests redirection of program counter */
        append(stream, loadval(r1, 1));

        append(stream, loadval(r7, 0));
        append(stream, loadval(r3, 8));

        append(stream, loadval(r5, 50)); 
        append(stream, output(r5));

        print_new_line(stream);

        append(stream, load_program(r7, r3));

        append(stream, halt());
}

void build_loop(Seq_T stream)
{
        /* Declares variables */
        append(stream, loadval(r5, 3));

        append(stream, loadval(r0, 10));
        append(stream, loadval(r1, 1));

        append(stream, loadval(r4, 50));

        append(stream, loadval(r6, 11));

        /* Start loop */
        append(stream, addition(r4, r1, r4));
        append(stream, output(r4));

        append(stream, addition(r1, r1, r1));

        append(stream, division(r2, r1, r0));

        append(stream, conditional_move(r5, r6, r2));
        
        append(stream, load_program(r7, r5));

        print_new_line(stream);

        append(stream, halt());
}

void build_miscellaneous(Seq_T stream)
{
        append(stream, loadval(r1, 3));
        append(stream, load_program(r7, r1));
        /* Jumps over this one */
        
        append(stream, loadval(r1, 15));
        append(stream, map_segment(r0, r1));

        append(stream, loadval(r2, 1));
        append(stream, loadval(r4, 2));
        append(stream, loadval(r5, 55));
        append(stream, segmented_store(r2, r4, r5));
        append(stream, segmented_load(r6, r2, r4));
        append(stream, output(r6));

        print_new_line(stream);

        append(stream, loadval(r1, 15));
        append(stream, map_segment(r0, r1));
        append(stream, loadval(r2, 2));
        append(stream, loadval(r4, 2));
        append(stream, loadval(r5, 55));
        append(stream, segmented_store(r2, r4, r5));
        append(stream, segmented_load(r6, r2, r4));
        append(stream, output(r6));

        print_new_line(stream);

        append(stream, loadval(r1, 2));
        append(stream, loadval(r6, 4));
        append(stream, map_segment(r2, r1));
        append(stream, map_segment(r3, r6));

        append(stream, loadval(r5, 48));

        append(stream, addition(r6, r2, r5));
        append(stream, addition(r7, r3, r5));

        append(stream, output(r6));
        append(stream, output(r7));

        append(stream, unmap_segment(r2));
        append(stream, unmap_segment(r3));

        print_new_line(stream);

        append(stream, loadval(r1, 0));
        append(stream, loadval(r2, 0));
        append(stream, bitwise_NAND(r3, r1, r2));

        append(stream, loadval(r4, 50));
        append(stream, add(r3, r3, r4));

        append(stream, output(r3));

        print_new_line(stream);
        append(stream, halt());
}

