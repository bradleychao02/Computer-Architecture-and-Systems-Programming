/* Name: main.c
 * Purpose: main executes a virtual machine program which reads in a user file
 * and executes 32-bit word instructions
 * By: Bradley Chao and Matthew Soto
 * Date: 11/16/2022
 */

/* :) */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "run_UM.h"
#include "universal_machine.h"

/* Name: main
*  Purpose: read file, call function to run program, and free memory
*  Parameters: argc, argv
*  Returns: int
*  Effects:  Checked runtime if two files are not provided,
*            file provided is empty, or UM is null
*/
int main(int argc, char *argv[])
{
        assert(argc == 2);

        FILE *fp = fopen(argv[1], "rb");
        assert(fp != NULL);

        universal_machine UM = read_program_file(fp);
        //assert(UM != NULL);

        run_program(UM);
       
        free_UM(&UM);

        fclose(fp);

        return 0;
}