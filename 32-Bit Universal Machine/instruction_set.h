/* Name: instruction_set.h
 * Purpose: implementation of instruction_set.h
 * By: Bradley Chao and Matthew Soto
 * Date: 11/16/2022
 */

#ifndef INSTRUCTION_SET_INCLUDED
#define INSTRUCTION_SET_INCLUDED

#include "universal_machine.h"
#include <stdlib.h>
#include <stdio.h>

typedef unsigned UM_Reg;

void conditional_move(universal_machine UM, UM_Reg A, UM_Reg B, UM_Reg C);
void segmented_load(universal_machine UM, UM_Reg A, UM_Reg B, UM_Reg C);
void segmented_store(universal_machine UM, UM_Reg A, UM_Reg B, UM_Reg C);
void addition(universal_machine UM, UM_Reg A, UM_Reg B, UM_Reg C);
void multiplication(universal_machine UM, UM_Reg A, UM_Reg B, UM_Reg C);
void division(universal_machine UM, UM_Reg A, UM_Reg B, UM_Reg C);
void bitwise_nand(universal_machine UM, UM_Reg A, UM_Reg B, UM_Reg C);

void map(universal_machine UM, UM_Reg B, UM_Reg C);
void unmap(universal_machine UM, UM_Reg C);
void output(universal_machine UM, UM_Reg C);
void input(universal_machine UM, UM_Reg C);
void load_program(universal_machine UM, UM_Reg B);
void load_value(universal_machine UM, UM_Reg A, uint32_t value);

#endif