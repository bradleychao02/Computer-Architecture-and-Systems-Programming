/* unit_test.c */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "run_UM.h"
#include "bitpack.h"
#include "universal_machine.h"
#include "instruction_set.h"

void test_get_instruction(universal_machine UM);
void test_set_instruction(universal_machine UM);
void test_get_register(universal_machine UM);
void test_set_register(universal_machine UM);
void test_mapping(universal_machine UM);

/* Instruction Tests */
void print_registers(universal_machine UM);




/* Number Operator Action
0 Conditional Move if $r[C] ̸= 0 then $r[A] := $r[B]
1 Segmented Load $r[A] := $m[$r[B]][$r[C]]
2 Segmented Store $m[$r[A]][$r[B]] := $r[C]
3 Addition $r[A] := ($r[B] + $r[C]) mod 232
4 Multiplication $r[A] := ($r[B] × $r[C]) mod 232
5 Division $r[A] := ⌊$r[B] ÷ $r[C]⌋
6 Bitwise NAND $r[A] := ¬($r[B] ∧ $r[C])
7 Halt Computation stops.
8 Map Segment A new segment is created with a number of
words equal to the value in $r[C]. Each word in
the new segment is initialized to 0. A bit pattern
that is not all zeroes and that does not identify
any currently mapped segment is placed in $r[B].
The new segment is mapped as $m[$r[B]].
9 Unmap Segment The segment $m[$r[C]] is unmapped. Future Map
Segment instructions may reuse the
identifier $r[C].
10 Output The value in $r[C] is written to the I/O device
immediately. Only values from 0 to 255 are
allowed.
11 Input The universal machine waits for input on the
I/O device. When input arrives, $r[C] is loaded
with the input, which must be a value from
0 to 255. If the end of input has been signaled,
then $r[C] is loaded with a full 32-bit word in
which every bit is 1.
12 Load Program Segment $m[$r[B]] is duplicated, and the
duplicate replaces $m[0], which is abandoned.
The program counter is set to point to
$m[0][$r[C]]. If $r[B] = 0, the load-program
operation is expected to be extremely quick.
13 Load Value See semantics for “other instruction” in
Section 3.5.2. */

//void i_conditional_Move(universal_machine UM);

void i_segmented_load(universal_machine UM);

int main(int argc, char *argv[])
{
        assert(argc == 2);

        FILE *fp = fopen(argv[1], "rb");
        assert(fp != NULL);

        universal_machine UM = read_program_file(fp);

        int input = fgetc(stdin) - 48;
        fprintf(stderr, "Input: %d\n", input);

        switch(input) {
                case 0 :
                        printf("Running Conditional Move...\n");
                        break;
                case 1:
                        printf("Running Segmented Load...\n");

                        /* $r[A] := $m[$r[B]][$r[C]] */
                        /* Only use r[0], r[1], and r[2] */

                        /* Sets $r[B] to be 0 */
                        set_register(UM, 1, 0);

                        /* Sets $r[C] to be 4 */
                        set_register(UM, 2, 4);

                        segmented_load(UM, 0, 1, 2);

                        print_registers(UM);
                        break;
                case 2:
                        /* $m[$r[A]][$r[B]] := $r[C] */
                        printf("Running segmented store...\n");

                        /* Register A / r[0] is set to 0 segment index */
                        set_register(UM, 0, 0);

                        /* Register B / r[1] is set to 7 offset index */
                        set_register(UM, 1, 7);

                        /* Set Register C r[2] to be value 5 */
                        set_register(UM, 2, 6969);

                        print_registers(UM);

                        segmented_store(UM, 0, 1, 2);

                        printf("%d, at $m[0][7]\n", get_instruction(UM, 0, 7));

                        break;
                case 3:
                        printf("Running Addition...\n");
                        break;
                case 4:
                        printf("Running multiplication...\n");
                        break;
                case 5:
                        printf("Running Division...\n");
                        break;
                case 6:
                        printf("Running Bitwise NAND...\n");
                        break;
                case 7:
                        printf("Running HALT\n");
                        break;
                case 8:
                        printf("RUNNING MAP...\n");

                        printf("%d\n", get_instruction(UM, 0, 28));

           /* Sets register 2/C to be 10 */
                        set_register(UM, 2, 10);

                        /* Expect reg 1/B to be changed to 1 */
                        map(UM, 1, 2);
                        printf("Word at $m[1][9]: %d\n", 
                        get_instruction(UM, 1, 9));


                        set_register(UM, 2, 1);

                //printf("Word at $m[1][9]: %d\n", get_instruction(UM, 1, 9));
        
                        printf("\n");

                        break;
                case 9:



                        break;
                case 49:
                        printf("RUNNING OUTPUT...\n");



                        break;
                case 50:
                        printf("RUNNING INPUT...\n");
                        break;
                case 51: 
                        printf("RUNNING LOAD PROGRAM...\n");
                        set_register(UM, 1, 1);
                        set_register(UM, 2, 5);
                        
                        /* Register 6/B will store 1 */
                        set_register(UM, 7, 100);
                        map(UM, 6, 7);
                        print_registers(UM);
                        load_program(UM, 1);

                        segment seg_zero = (segment) Seq_get(UM->segments, 0);

                printf("Seq Length %d\n", Seq_length(seg_zero->instructions));
                        
                        break;
                case 52:
                        printf("RUNNING LOAD VALUE...\n");
                        
                        break;
                default :
                        fprintf(stderr, "UNIT TEST UNKNOWN COMMAND\n");
                        exit(EXIT_FAILURE);
                        break;
        }

        printf("HERE\n");
        free_UM(&UM);

        printf("HERE2\n");


        fclose(fp);

        return 0;
}


/* Test UM member functions */

void test_get_instruction(universal_machine UM)
{
        uint32_t result = get_instruction(UM, 0, 77);
        assert(result == 741354544);
}

void test_set_instruction(universal_machine UM)
{
        set_instruction(UM, 0, 77, 0);
        assert(get_instruction(UM, 0, 77) == 0);
}

void test_get_register(universal_machine UM)
{       
        for (size_t i = 0; i < 8; i++) {
                assert(get_register(UM, i) == 0);
        }
}

void test_set_register(universal_machine UM)
{
        for (size_t i = 0; i < 8; i++) {
                set_register(UM, i, i);
        }

        for (size_t i = 0; i < 8; i++) {
                assert(get_register(UM, i) == i);
        }
}

void test_mapping(universal_machine UM)
{
        /* Allocate 100 new segments on top of $m[0] */
        for (int i = 1; i < 101; i++) {
                /* This should effectively create $m[i] */
                uint32_t mapped_ID = map_segment(UM, 200);
                assert((int) mapped_ID == i);
                assert(Seq_length(UM->segments) == (i + 1));
        }

        printf("Test\n");

        /* Unmap $m[1] */
        unmap_segment(UM, 1);
      
        assert(map_segment(UM, 4) == 1);

        assert(Seq_length(UM->segments) == 101);
}

/* End testing UM member functions */

/* Start instructions unit testing */
void print_registers(universal_machine UM)
{
        for (size_t i = 0; i < 8; i++) {
                fprintf(stderr, "REGISTER VALUE : %d\n", get_register(UM, i));
        }
}



// void i_multiplication(universal_machine UM)
// {
//         /* Prints first 3 register values */
//         print_registers(UM);

//         multiplication(UM, 0, 1, 2);

//         printf("TEST\n");

//         print_registers(UM);
// }



//void i_conditional_Move(universal_machine UM)
//{
//first test (correct)
        /* Prints first 3 register values */
        // print_registers(UM);
        // conditional_move(UM, 2, 3, 4);
        // printf("TEST\n");
        // print_registers(UM);
//second test (correct) 1,1,2
        /* Prints first 3 register values */
        // print_registers(UM);
        // conditional_move(UM, 0, 1, 2);
        // printf("TEST\n");
        // print_registers(UM);
//third test (correct) 6,6,7
        /* Prints first 3 register values */
        // print_registers(UM);
        // conditional_move(UM, 5, 6, 7);
        // printf("TEST\n");
        // print_registers(UM);

//fourth test (correct) segfault because there is no 9th register
        /* Prints first 3 register values */
        // print_registers(UM);
        // conditional_move(UM, 6, 7, 8);
        // printf("TEST\n");
        // print_registers(UM);
//}



//seems to be failing
void i_segmented_load(universal_machine UM)
{
        /* Prints first 3 register values */
        print_registers(UM);
        segmented_load(UM, 3, 4, 5);
        printf("TEST\n");
        print_registers(UM);
}

// void i_segmented_store(universal_machine UM)
// {
//  /* Prints first 3 register values */
//         print_registers(UM);
//         segmented_load(UM, 0, 1, 2);
//         printf("TEST\n");
//         print_registers(UM);

// }



//addtion
// void i_add(universal_machine UM)
// {

// //first test
//         /* Prints first 3 register values */
//         print_registers(UM);
//         addition(UM, 0, 4, 3);
//         printf("TEST\n");
//         print_registers(UM);

//second test (expected to fail since register 0 holds a num between 0 and 8)
//         print_registers(UM);
//         addition(UM, 0, 8, 1);
//         printf("TEST\n");
//         print_registers(UM);
// }



// void i_division(universal_machine UM)
// {
//fist test (passed)
        /* Prints first 3 register values */
        // print_registers(UM);
        // division(UM, 0, 2, 2);
        // printf("TEST\n");
        // print_registers(UM);

//second test (passed)
        /* Prints first 3 register values */
        // print_registers(UM);
        // division(UM, 0, 7, 2);
        // printf("TEST\n");
        // print_registers(UM);

//third test (should abort since we divide by 0) (passed)
        /* Prints first 3 register values */
        // print_registers(UM);
        // division(UM, 0, 4, 0);
        // printf("TEST\n");
        // print_registers(UM);
//}



//void i_bitwise_nand(universal_machine UM)
//{
//fist test (1 & 1 is 1 and the NOT of 1 is -2) (correct)
        /* Prints first 3 register values */
        // print_registers(UM);
        // bitwise_nand(UM, 0, 1, 1);
        // printf("TEST\n");
        // print_registers(UM);
//second test (2 & 6 is 2 and ~2 is -3) (correct)
        /* Prints first 3 register values */
        // print_registers(UM);
        // bitwise_nand(UM, 0, 2, 6);
        // printf("TEST\n");
        // print_registers(UM);
// third test (5 & 1 is 1 and ~1 is -2) (correct)
//         Prints first 3 register values */
//         print_registers(UM);
//         bitwise_nand(UM, 0, 5, 1);
//         printf("TEST\n");
//         print_registers(UM);
// }



// void i_halt(universal_machine UM)
// {
// //fist test (stops and does not print again) (correct)
//         /* Prints first 3 register values */
//         print_registers(UM);
//         halt();
//         printf("TEST\n");
//         print_registers(UM);  
// }



// void i_map_segment(universal_machine UM)
// {
// //first test
//         /* Prints first 3 register values */
        // print_registers(UM);
        // map(UM, 0, 1, 2);
        // printf("TEST\n");
        // print_registers(UM);  
//}

// void i_unmap_segment(universal_machine UM)
// {
// //first test
//         /* Prints first 3 register values */
//         print_registers(UM);
//         map(UM, 0, 1, 2);
//         printf("TEST\n");
//         print_registers(UM);  
//}



//the value in register C is 2 so nothing is 
//coming out but if you put others it is fine
// void i_output(universal_machine UM)
// {
// //first test
//         /* Prints first 3 register values */
//         print_registers(UM);
//         output(UM, 2);
//         //printf("I hope this worked\n");
//         printf("TEST\n");
//         print_registers(UM);  
// }

//looks good
// void i_input(universal_machine UM)
// {
//first test (we put 6 and got 54 in the third register)(correct)
        /* Prints first 3 register values */
        // print_registers(UM);
        // input(UM, 2);
        // printf("I hope this worked\n");
        // printf("TEST\n");
        // print_registers(UM);  

//second test (we put Q and got 81 in the first register)(correct)
        /* Prints first 3 register values */
        // print_registers(UM);
        // input(UM, 7);
        // printf("I hope this worked\n");
        // printf("TEST\n");
        // print_registers(UM);  
//third test (we put S and got 83 in the eighth register)(correct)
        /* Prints first 3 register values */
        // print_registers(UM);
        // input(UM, 7);
        // printf("I hope this worked\n");
        // printf("TEST\n");
        // print_registers(UM); 
// }



// void i_load_program(universal_machine UM)
// {
//  /* Prints first 3 register values */
// //printf("hihidhidhis");
//         print_registers(UM);
//         load_program(UM, 1, 1);
//         printf("TEST\n");
//         print_registers(UM);

// }

//not yet tested
// void i_load_value(universal_machine UM)
// {
//  /* Prints first 3 register values */
// //printf("hihidhidhis");
//         print_registers(UM);
//         load_value(UM, 1, 1);
//         printf("TEST\n");
//         print_registers(UM);

// }


