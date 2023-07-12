/* Name: run_UM.c
 * This module is responsible for abstracting runtime details away from the 
 * the universal machine data structure. This module is responsible for loading
 * data into the UM structure from file and executing command loop.
 * Bradley Chao and Matthew Soto
 * November 18, 2022
 */

#include "run_UM.h"
#include <stdbool.h>

/* Name: read_program_file
 * Purpose: Read bytes of file and translate them into a sequence of 32-bit
 * instructions and store them in the universal machine data structure
 * Parameters: File pointer
 * Returns: Pointer to universal machine structure with program instructions
 * Effects: Checked runtime error if file is null
 */
universal_machine read_program_file(FILE *fp)
{
        assert(fp != NULL);

        Seq_T program_instructions = Seq_new(100);
        assert(program_instructions != NULL);

        uint32_t word = 0;

        int byte = fgetc(fp);

        while (byte != EOF) {
                word = 0;

                for (int i = 3; i >= 0; i--) {
                        word = Bitpack_newu(word, 8, i * 8, byte);
                        byte = fgetc(fp);
                }

                Seq_addhi(program_instructions, (void *) (uintptr_t) word);
        }

        universal_machine UM = new_UM(program_instructions);

        return UM;
}

void debug_registers(universal_machine UM)
{
        for (size_t i = 0; i < 8; i++) {
                printf("R[%lu]: %d\n", i, UM->registers[i]);
        }
}

/* Name: run_program
 * Purpose: Command loop for each machine cycle 
 * Parameters: Pointer to instance of universal machine
 * Returns: Void
 * Effects: Checked runtime error if program counter is out of bounds, invalid
 * OP_CODE, and if segment zero was unavailable 
 */
void run_program(universal_machine UM)
{
        assert(UM != NULL);

        while (true) {
                /* Get segment zero data */
                segment segment_zero = Seq_get(UM->segments, 0);
                assert(segment_zero != NULL);
                
                Seq_T words = segment_zero->instructions;
                assert(words != NULL);

                /* (1) Check program counter is within bounds of segment zero */
                uint32_t max_index = Seq_length(words) - 1;
                assert(UM->program_counter <= max_index);
                
                UM_instruction word = (uintptr_t) Seq_get(words, 
                                                         UM->program_counter);
                   
                int OP_CODE = Bitpack_getu(word, 4, 28);

                /* (2) Check whether code corresponds to an instruction */
                assert(OP_CODE >= 0 && OP_CODE <= 13);
                
                UM_Reg A, B, C;

                /* Halt Command, exit function to free data */
                if (OP_CODE == 7) {
                        return;
                }
                /* Special Load Value Command */
                else if (OP_CODE == 13) {
                        A = Bitpack_getu(word, 3, 25);
                        int load_val = Bitpack_getu(word, 25, 0);
                        
                        load_value(UM, A, load_val);
                }
                /* Other 12 instructions */
                else {
                        A = Bitpack_getu(word, 3, 6);
                        B = Bitpack_getu(word, 3, 3);
                        C = Bitpack_getu(word, 3, 0);
                    
                        run_helper(UM, OP_CODE, A, B, C);
                }        

                if (OP_CODE == 12) {
                        UM->program_counter = get_register(UM, C);
                }
                else {
                        UM->program_counter++;
                }
        }
}

/* Name: run_helper
*  Purpose: Handles non load value instruction cases  op codes 0-12
*  Parameters: UM, OP_CODE, A, B, C
*  Returns: none
*  Effects: helps choose which case/function to call
*/
void run_helper(universal_machine UM, int OP_CODE, UM_Reg A,
                UM_Reg B, UM_Reg C)
{
        switch (OP_CODE) {
                case 0:
                        conditional_move(UM, A, B, C);
                        break;
                case 1:
                        segmented_load(UM, A, B, C);
                        break;
                case 2:
                        segmented_store(UM, A, B, C);
                        break;
                case 3:
                        addition(UM, A, B, C);
                        break;
                case 4:
                        multiplication(UM, A, B, C);
                        break;
                case 5:
                        division(UM,  A, B, C);
                        break;
                case 6:
                        bitwise_nand(UM, A, B, C);
                        break;
                case 8:
                        map(UM, B, C);
                        break;
                case 9:
                        unmap(UM, C);
                        break;
                case 10:
                        output(UM, C);
                        break;
                case 11:
                        input(UM, C);
                        break;
                case 12:
                        load_program(UM, B);
                        break;
        }
}