/*
    Remove all unnecessary lines (including this one) 
    in this comment.
    REFER TO THE SUBMISSION INSTRUCTION FOR DETAILS

    Name 1: Braden McElroy
    Name 2: Anna Ledbetter
    UTEID 1: BRM3367
    UTEID 2: AML6995
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
*/

#define WORDS_IN_MEM    0x08000 
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

  int PC,		/* program counter */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P;		/* p condition bit */
  int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {                                                    
  printf("----------------LC-3b ISIM Help-----------------------\n");
  printf("go               -  run program to completion         \n");
  printf("run n            -  execute program for n instructions\n");
  printf("mdump low high   -  dump memory from low to high      \n");
  printf("rdump            -  dump the register & bus values    \n");
  printf("?                -  display this help menu            \n");
  printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  process_instruction();
  CURRENT_LATCHES = NEXT_LATCHES;
  INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {                                      
  int i;

  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating for %d cycles...\n\n", num_cycles);
  for (i = 0; i < num_cycles; i++) {
    if (CURRENT_LATCHES.PC == 0x0000) {
	    RUN_BIT = FALSE;
	    printf("Simulator halted\n\n");
	    break;
    }
    cycle();
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {                                                     
  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating...\n\n");
  while (CURRENT_LATCHES.PC != 0x0000)
    cycle();
  RUN_BIT = FALSE;
  printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {          
  int address; /* this is a byte address */

  printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  printf("-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  printf("\n");

  /* dump the memory contents into the dumpsim file */
  fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  fprintf(dumpsim_file, "-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {                               
  int k; 

  printf("\nCurrent register/bus values :\n");
  printf("-------------------------------------\n");
  printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
  printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  printf("Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  printf("\n");

  /* dump the state information into the dumpsim file */
  fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
  fprintf(dumpsim_file, "-------------------------------------\n");
  fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
  fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  fprintf(dumpsim_file, "Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */  
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {                         
  char buffer[20];
  int start, stop, cycles;

  printf("LC-3b-SIM> ");

  scanf("%s", buffer);
  printf("\n");

  switch(buffer[0]) {
  case 'G':
  case 'g':
    go();
    break;

  case 'M':
  case 'm':
    scanf("%i %i", &start, &stop);
    mdump(dumpsim_file, start, stop);
    break;

  case '?':
    help();
    break;
  case 'Q':
  case 'q':
    printf("Bye.\n");
    exit(0);

  case 'R':
  case 'r':
    if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(dumpsim_file);
    else {
	    scanf("%d", &cycles);
	    run(cycles);
    }
    break;

  default:
    printf("Invalid Command\n");
    break;
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {                                           
  int i;

  for (i=0; i < WORDS_IN_MEM; i++) {
    MEMORY[i][0] = 0;
    MEMORY[i][1] = 0;
  }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {                   
  FILE * prog;
  int ii, word, program_base;

  /* Open program file. */
  prog = fopen(program_filename, "r");
  if (prog == NULL) {
    printf("Error: Can't open program file %s\n", program_filename);
    exit(-1);
  }

  /* Read in the program. */
  if (fscanf(prog, "%x\n", &word) != EOF)
    program_base = word >> 1;
  else {
    printf("Error: Program file is empty\n");
    exit(-1);
  }

  ii = 0;
  while (fscanf(prog, "%x\n", &word) != EOF) {
    /* Make sure it fits. */
    if (program_base + ii >= WORDS_IN_MEM) {
	    printf("Error: Program file %s is too long to fit in memory. %x\n",
             program_filename, ii);
	    exit(-1);
    }

    /* Write the word to memory array. */
    MEMORY[program_base + ii][0] = word & 0x00FF;
    MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
    ii++;
  }

  if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);
  printf("PC: 0x%.4X\n", CURRENT_LATCHES.PC);

  printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */ 
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *files[], int num_prog_files) { 
  int i;

  init_memory();
  for ( i = 0; i < num_prog_files; i++ ) {
    load_program(files[i]);
  }
  CURRENT_LATCHES.Z = 1;  
  NEXT_LATCHES = CURRENT_LATCHES;
    
  RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {                              
  FILE * dumpsim_file;

  /* Error Checking */
  if (argc < 2) {
    printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
           argv[0]);
    exit(1);
  }

  printf("LC-3b Simulator\n\n");

  initialize(&argv[1], argc - 1);

  if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
    printf("Error: Can't open dumpsim file\n");
    exit(-1);
  }

  while (1)
    get_command(dumpsim_file);
    
}

/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   MEMORY

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */

/***************************************************************/

int sext(int val, int bits) { 
  val &= (1 << bits); // get first bit
  if (val & (1 << (bits - 1))){  // @braden for bits = 5 --> 0x100000 & 0x10000 
    val -= (1 << bits);
  }
  return val;
}

int mask_and_sext(int instr, int bits) { // bits = bit length of offset
    int mask = (1 << bits ) - 1; // ex) for bits = 5, 0x1000000 --> 0x011111
    int val = instr & mask;
    if ( val & (1 << (bits-1)) ) { // if negative, sign extend
        val = val | (~mask) ; // mask everything above bits to 1
    }
    return val;
}


void process_instruction(){
  /*  function: process_instruction
   *  
   *    Process one instruction at a time  
   *       -Fetch one instruction
   *       -Decode 
   *       -Execute
   *       -Update NEXT_LATCHES
   */     

 // FETCH
    // printf("PC: 0x%.4X\n", CURRENT_LATCHES.PC); // it is correct here
    int LSB = MEMORY[(CURRENT_LATCHES.PC >> 1)][0];
    int MSB = MEMORY[(CURRENT_LATCHES.PC >> 1)][1];
    int instr = LSB | (MSB << 8) ;
    printf("MEMORY[LSB]: 0x%.2X, MEMORY[MSB]: 0x%.2X, instr: 0x%.4X\n", LSB, MSB, instr);

// DECODE
    int opcode = ( instr >> 12 ) & 0xF;
    int Arg1 = ( instr >> 9 ) & 0x7;
    int Arg2 = ( instr >> 6 ) & 0x7;
    int Arg3 = instr & 0x7;
    // for MATH
    int val1 = mask_and_sext(CURRENT_LATCHES.REGS[(Arg2)], 16);
    int val2;
    if ( (instr >> 5) & 0x1) { // ctrl bit
        val2 = mask_and_sext(instr, 5);
    } else {
        val2 = mask_and_sext(CURRENT_LATCHES.REGS[(instr & 0x7)], 16);
    }
    // for MEM
    int MAR;
    int MDR;
    bool set_cc = 0;
    // for
    int offset;


// DECODE + EXECUTE
    NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2; 

    printf("instr: 0x%.4X\n", instr);
    switch (opcode) {

        case 0x0: {        // Branch
          // set BEN based on IR[11:9] & N,Z,P. Also set it for "BR" (unconditional)
          bool BEN = 
            (
              ((instr & 0x0800) & CURRENT_LATCHES.N) 
              | ((instr & 0x0400) & CURRENT_LATCHES.Z) 
              | ((instr & 0x0200) & CURRENT_LATCHES.P)
              | !((instr & 0x0800) | (instr & 0x0400) | (instr & 0x0200))
            );

            if (BEN) {
              
            }
            break;
        }

        case 0x1: { // ADD 
            NEXT_LATCHES.REGS[Arg1] = Low16bits(val1 + val2);
            printf("Arg1 (DR): 0x%.4X\n", Arg1);
            printf("Arg2 (SR1): 0x%.4X\n", Arg2);
            printf("Arg3 (SR2 or imm5): 0x%.4X\n", Arg3);
            printf("val1 (from Arg2): 0x%.4X\n", val1);
            printf("val2 (from Arg3 or imm5): 0x%.4X\n", val2);
            printf("Result (stored in DR): 0x%.4X\n", NEXT_LATCHES.REGS[Arg1]);
            set_cc = TRUE;
            break;
        }

        case 0x5: { // AND 
            NEXT_LATCHES.REGS[Arg1] = Low16bits(val1 & val2);
            printf("Arg1 (DR): 0x%.4X\n", Arg1);
            printf("Arg2 (SR1): 0x%.4X\n", Arg2);
            printf("Arg3 (SR2 or imm5): 0x%.4X\n", Arg3);
            printf("val1 (from Arg2): 0x%.4X\n", val1);
            printf("val2 (from Arg3 or imm5): 0x%.4X\n", val2);
            printf("Result (stored in DR): 0x%.4X\n", NEXT_LATCHES.REGS[Arg1]);
            set_cc = TRUE;
            break;
        }

        case 0x9: { // XOR
            NEXT_LATCHES.REGS[Arg1] = Low16bits(val1 ^ val2);
            printf("Arg1 (DR): 0x%.4X\n", Arg1);
            printf("Arg2 (SR1): 0x%.4X\n", Arg2);
            printf("Arg3 (SR2 or imm5): 0x%.4X\n", Arg3);
            printf("val1 (from Arg2): 0x%.4X\n", val1);
            printf("val2 (from Arg3 or imm5): 0x%.4X\n", val2);
            printf("Result (stored in DR): 0x%.4X\n", NEXT_LATCHES.REGS[Arg1]);
            set_cc = TRUE;
            break;
        }

        case 0x2: {        /* 0b0010 - LDB */
            /* DR, BaseR, boffset6 */
            set_cc = TRUE;
            break;
        }

        case 0x3: {        /* 0b0011 - STB */
            /* SR, BaseR, boffset6 */
            break;
        }

        case 0x4: { // JSR,JSRR
            NEXT_LATCHES.REGS[7] = NEXT_LATCHES.PC;
            if ( instr & (1 << 11)) { // ctrl bit
                offset = mask_and_sext(instr, 10);
                NEXT_LATCHES.PC = Low16bits( (NEXT_LATCHES.PC + ( offset << 1)) );
            } else {
                NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[(instr & 0x1C0) >> 6];
            }
            printf("NEXT_LATCHES.REGS[7]: 0x%.4X\n", NEXT_LATCHES.REGS[7]);
            printf("Control bit: %d\n", (instr & (1 << 11)));
            printf("offset: 0x%.4X\n", offset);
            printf("Updated NEXT_LATCHES.PC: 0x%.4X\n", NEXT_LATCHES.PC);
            break;
        }   

        case 0x6: {        /* 0b0110 - LDW */
            /* DR, BaseR, offset6 */
            set_cc = TRUE;
            break;
        }

        case 0x7: {        /* 0b0111 - STW */
            /* SR, BaseR, offset6 */
            break;
        }

        case 0xC: {        // JMP, RET(=JMP R7)
            NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[(instr & 0x1C0) >> 6];
            printf("Reg: 0x%.4X\n", instr & 0x1C0);
            printf("Updated NEXT_LATCHES.PC: 0x%.4X\n", NEXT_LATCHES.PC);
            break;
        }

        case 0xD: {        /* 0b1101 - LSHF / RSHFL / RSHFA (bits[5:4] select) */
            set_cc = TRUE;
            break;
        }

        case 0xE: {        /* 0b1110 - LEA */
            /* DR, PCoffset9 from label */
            break;
        }

        case 0xF: { // TRAP 0x25 = HALT
            CURRENT_LATCHES.REGS[7] = NEXT_LATCHES.PC;
            MAR = ( (instr & 0xFF) << 1);
            MDR = ( MEMORY[MAR][0] | (MEMORY[MAR][1] << 8));
            NEXT_LATCHES.PC = ( MEMORY[MDR][0] | (MEMORY[MDR][1] << 8)); // should be 0x0000
            break;
        }

        default: {
            printf("Error: unrecognized opcode\n");
            exit(4);
        }
    } 

    // WRITE BACK: update NEXT_LATCHES CC
    if (set_cc) {
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 0; 
        if (mask_and_sext(NEXT_LATCHES.REGS[Arg1], 16 ) > 0) { // every to set_cc, DR = Arg1
            NEXT_LATCHES.P = 1;
        } else if (mask_and_sext(NEXT_LATCHES.REGS[Arg1], 16 ) < 0)  {
            NEXT_LATCHES.N = 1;  
        } else {
            NEXT_LATCHES.Z = 1;
        }
    } 
}

/*
@braden I learned we can test with this from the TA's
 ./simulate program.asm mem.asm // fills memory with mem.asm values
 
 mem.asm
 0x5000 // at this address
 0x2394
 0x0002
 0x00A1
*/