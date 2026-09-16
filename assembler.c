/* 
Name 1: Braden McElroy
Name 2: Anna Ledbetter
UTEID 1: BRM3367
UTEID 2: AML6995
*/

#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character o  perations */
#include <limits.h> /* Library for definitions of common variable type characteristics */

#define MAX_LINE_LENGTH 255
#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255
#define NUM_OPCODES 28
#define NUM_INVALID 4
	
  enum
	{
	   DONE, OK, EMPTY_LINE
	};

  // opcode types
  enum {MATH, MEM, FIXED, SHIFT, TRAP, PC_OFFSET, JUMP};
  
  // arg types
  enum {REGISTER, LABEL, NUM};

int prog_start;
int pc;
int symbol_count;

typedef struct {
  char name[6]; 
  int binary;
  int type;
} opcode;


opcode opcodes[28] = {
  {"add",    0b0001000000000000, MATH},
  {"and",    0b0101000000000000, MATH},
  {"halt",   0b1111000000100101, FIXED},   // TRAP x25, fully fixed
  {"jmp",    0b1100000000000000, JUMP},
  {"jsr",    0b0100100000000000, PC_OFFSET},
  {"jsrr",   0b0100000000000000, JUMP},
  {"ldb",    0b0010000000000000, MEM},
  {"ldw",    0b0110000000000000, MEM},
  {"lea",    0b1110000000000000, PC_OFFSET},
  {"nop",    0b0000000000000000, FIXED},   // nzp=000, offset=0
  {"not",    0b1001000000111111, MATH},
  {"ret",    0b1100000111000000, FIXED},   // JMP with BaseR=R7 baked in
  {"lshf",   0b1101000000000000, SHIFT},
  {"rshfl",  0b1101000000010000, SHIFT},
  {"rshfa",  0b1101000000110000, SHIFT},
  {"rti",    0b1000000000000000, FIXED},
  {"stb",    0b0011000000000000, MEM},
  {"stw",    0b0111000000000000, MEM},
  {"trap",   0b1111000000000000, TRAP},
  {"xor",    0b1001000000000000, MATH},
  {"brn",    0b0000100000000000, PC_OFFSET},
  {"brz",    0b0000010000000000, PC_OFFSET},
  {"brp",    0b0000001000000000, PC_OFFSET},
  {"brnz",   0b0000110000000000, PC_OFFSET},
  {"brnp",   0b0000101000000000, PC_OFFSET},
  {"brzp",   0b0000011000000000, PC_OFFSET},
  {"br",     0b0000111000000000, PC_OFFSET},
  {"brnzp",  0b0000111000000000, PC_OFFSET}
};

char* invalid[4] = 
{ "in", "out", "getc", "puts",
};

// Symbol Table

typedef struct Label {
    char name[MAX_LABEL_LEN + 1];
    int address;
} Label;

Label symbol_table[MAX_SYMBOLS] = {0};

FILE* infile = NULL;
FILE* outfile = NULL;

int isOpcode(char *str) {
    for (int i = 0; i < NUM_OPCODES; i++) {
      if (strcmp((opcodes[i].name), str) == 0) {
        return 1;
      }
    }
    return -1;
}

int isValid(char *str) { 
  if ( !(isalpha(str[0])) || (str[0] == 'x')) {
    return 0;
  }
  for (int i = 0; i < NUM_INVALID; i++) {
    if (strcmp((invalid[i]), str) == 0) {
      return 0;
    }
  }
  int n = 0;
  while ( (n < 21) && (str[n] != '\0')) { 
    if ( isalnum(str[n]) == 0) { 
      return 0;
    }
    n++;
  }
  if (n == 21) {
    return 0;
  }

  return 1;
}

int readAndParse
( 
  FILE * pInfile, char * pLine, char ** pLabel, char
	** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4
)
{
  char * lRet, * lPtr;
  int i;
  if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
    return( DONE );
  for( i = 0; i < strlen( pLine ); i++ )
    pLine[i] = tolower( pLine[i] );
  /* convert entire line to lowercase */

  *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

    /* ignore the comments */
  lPtr = pLine;
  while( *lPtr != ';' && *lPtr != '\0' &&*lPtr != '\n' ) 
    lPtr++;

  *lPtr = '\0';
    if( !(lPtr = strtok( pLine, "\t\n ," ) ) ) 
  return( EMPTY_LINE );

    if(isOpcode( lPtr ) == -1 && lPtr[0] != '.' ) /* found a label */
    {
  *pLabel = lPtr;
  if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
    }
    
          *pOpcode = lPtr;

    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
    
          *pArg1 = lPtr;
    
          if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

    *pArg2 = lPtr;
    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

    *pArg3 = lPtr;

    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

    *pArg4 = lPtr;

    return( OK );
}

/* Note: MAX_LINE_LENGTH, OK, EMPTY_LINE, and DONE are defined values */


int toNum( char * pStr )
{
   char * t_ptr;
   char * orig_pStr;
   int t_length,k;
   int lNum, lNeg = 0;
   long int lNumLong;

   orig_pStr = pStr;
   if( *pStr == '#' )				/* decimal */
   { 
     pStr++;
     if( *pStr == '-' )				/* dec is negative */
     {
       lNeg = 1;
       pStr++;
     }
     t_ptr = pStr;
     t_length = strlen(t_ptr);
     for(k=0;k < t_length;k++)
     {
       if (!isdigit(*t_ptr))
       {
	 printf("Error: invalid decimal operand, %s\n",orig_pStr);
	 exit(4);
       }
       t_ptr++;
     }
     lNum = atoi(pStr);
     if (lNeg)
       lNum = -lNum;
 
     return lNum;
   }
   else if( *pStr == 'x' )	/* hex     */
   {
     pStr++;
     if( *pStr == '-' )				/* hex is negative */
     {
       lNeg = 1;
       pStr++;
     }
     t_ptr = pStr;
     t_length = strlen(t_ptr);
     for(k=0;k < t_length;k++)
     {
       if (!isxdigit(*t_ptr))
       {
	 printf("Error: invalid hex operand, %s\n",orig_pStr);
	 exit(4);
       }
       t_ptr++;
     }
     lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
     lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
     if( lNeg )
       lNum = -lNum;
     return lNum;
   }
   else
   {
	printf( "Error: invalid operand, %s\n", orig_pStr);
	exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
   }
}

int arg_type( char *pArg ) { // pass by val not ref, so it's read only (makes a copy var on)
  if (pArg[0] == 'r' && isdigit(pArg[1]) && pArg[1] <= '7' && pArg[2] == '\0') {
    return REGISTER;
  }
  if (isdigit(pArg[0]) || (pArg[0] == 'x') || (pArg[0] == '#') || (pArg[0] == '-')) {
    return NUM;
  }
  return LABEL;
}

int calculate_offset (char *arg, int bits) {
  int type = arg_type(arg);
  int offset = 0;
  if (type == LABEL) {
    int pc_plus = pc + 2;
    int m;
    for (m = 0; m < symbol_count; m++) {
      printf("\nname:%s, address:0x%.4X\n", symbol_table[m].name, symbol_table[m].address);
      if (strcmp((symbol_table[m].name), arg) == 0) {
        break;
      }
    }
    offset = (symbol_table[m].address - pc_plus) / 2; 
    printf("\noffset:%d, label_address:0x%.4X, pc_plus:0x%.4X\n", offset, symbol_table[m].address, pc_plus);
  }

  if (type == NUM){
    offset = toNum(arg);
  }

  if ( offset < 0 ) { // if negative, sign extends first
    offset += (1<<bits);
  }
  return offset;
}


int BuildSymbolTable(FILE *pInFile, Label symbol_table[]) {
    char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, 
    *lArg1,*lArg2, *lArg3, *lArg4;

    int lRet;
    int i = 0;
    int line_count = 0;

    do{
      printf("line_count:%d\n", line_count);
      lRet = readAndParse(pInFile, lLine, &lLabel, 
        &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);
        if (lRet != DONE && lRet != EMPTY_LINE){
          if (!strcmp(lOpcode, ".orig")) {
            prog_start = toNum(lArg1);
            pc = prog_start;
          }
          if (strlen(lLabel)){
            if (isValid(lLabel)){
              symbol_table[i].address = prog_start + (line_count*2);
              strcpy(symbol_table[i].name, lLabel);
              i++;
            }
          }

          if (isOpcode(lOpcode) == 1 || !strcmp(lOpcode, ".fill")){
            line_count ++;
          }

        }
    } while (lRet != DONE);
    return i;
}

int second_pass(FILE *pInFile, FILE *pOutfile) {
  char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, 
  *lArg1,*lArg2, *lArg3, *lArg4;

  int lInstr;

  int lRet;
  int i = 0;
  int line_count = 0;
  int line_bin;

  do{
    lRet = readAndParse(pInFile, lLine, &lLabel, 
      &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);
      if (lRet != DONE && lRet != EMPTY_LINE){
        if (!strcmp(lOpcode, ".orig")) {
          lInstr = toNum(lArg1);
          printf("Op: %s\n", lOpcode);  
          printf("lInstr: 0x%.4X\n", lInstr);
          fprintf( pOutfile, "0x%.4X\n", lInstr );
          return 1;
        }
        if (!strcmp(lOpcode, ".fill")) {
          lInstr = toNum(lArg1);
          lInstr &= 0xFFFF; // mask negative numbers to 16 bits
          printf("Op: %s\n", lOpcode);  
          printf("lInstr: 0x%.4X\n", lInstr);
          fprintf( pOutfile, "0x%.4X\n", lInstr );
          pc +=2;
          return 1;
        }
        if (!strcmp(lOpcode, ".end")) {
          return 0;
        }
        
        // add opcode to binary line
        int i;
        for (i=0; i<28; i++) {
          if (!strcmp(lOpcode, opcodes[i].name)) {
            lInstr = opcodes[i].binary;
            break;
          }
        }
        printf("i: %d\n", i);
        printf("type: %d\n", opcodes[i].type);


        switch (opcodes[i].type) {
          case MATH: {
            int reg = atoi(&lArg1[1]); // only care about register number
            printf("Arg1: %c\n", lArg1[1]);
            lInstr += (reg * (1<<9) );

            reg = atoi(&lArg2[1]); 
            lInstr += reg * (1<<6);

            int type = arg_type(lArg3);
            if (type == REGISTER) {
              reg = atoi(&lArg3[1]); 
              lInstr += reg;
            }
            if (type == NUM){
              int imm = toNum(lArg3);
              if ( imm < 0 ) { // if negative, sign extends first
                imm += 32;
              }
              lInstr += imm;
              lInstr += (1<<5); // ctrl bit
            }
            // for valid assembly, MATH won't have Label args
            break;
          }
          case MEM: {
            int reg = atoi(&lArg1[1]); // only care about register number
            lInstr += (reg * (1<<9) );

            reg = atoi(&lArg2[1]); 
            lInstr += reg * (1<<6);

            // should always be NUM 
            int imm = toNum(lArg3);
            if ( imm < 0 ) { // if negative, sign extends first
              imm += 64;
            }
            lInstr += imm;
            break;
          } 
          case FIXED: {
            break;
          }
          case SHIFT: {
            int reg = atoi(&lArg1[1]); // only care about register number
            lInstr += (reg * (1<<9) );

            reg = atoi(&lArg2[1]); 
            lInstr += reg * (1<<6);

            // should always be pos NUM 
            int imm = toNum(lArg3);
            lInstr += imm;

            break;
          }
          case TRAP: {
            // should always be pos NUM 
            int imm = toNum(lArg1);
            lInstr += imm;
            break;
          }
          case PC_OFFSET: { // check nzp is one arg, offset / label is arg2
            int offset;
      
            if (!strcmp(lOpcode, "lea")) { // LEA
              int reg = atoi(&lArg1[1]); // only care about register number
              printf("\nArg1: %c\n", lArg1[1]);
              printf("Arg2: %s\n", lArg2);
              lInstr += (reg * (1<<9) );

              offset = calculate_offset(lArg2, 9);
              lInstr += offset;
            }
            else if (!(lInstr & 0xF000)) {
              printf("Op:%s, Arg1: %s\n", lOpcode, lArg1);
              offset = calculate_offset(lArg1, 9);
              lInstr += offset;
            }
            else if (!strcmp(lOpcode, "jsr")) {
              offset = calculate_offset(lArg1, 11);
              lInstr += offset;
            }
              
            break;
          }
          case JUMP: {
            int reg = atoi(&lArg1[1]);
            printf("\nArg1: %c\n", lArg1[1]);
            lInstr += reg * (1<<6);
            break;
          }

          default: {
            printf("Error: Unsupported opcode type\n");
            break;
          }
        }
        
        printf("Op: %s\n", lOpcode);
        printf("lInstr: 0x%.4X\n", lInstr);
        fprintf( pOutfile, "0x%.4X\n", lInstr );
        pc += 2;
        return 1;
      }
      line_count++;
  } while (lRet != DONE);
  return i;
}

int main (int argc, char* argv[]) {
    
    // Parse command line arguments 
    char *prgName   = NULL;
    char *iFileName = NULL;
    char *oFileName = NULL;  

    prgName   = argv[0];
    iFileName = argv[1];
    oFileName = argv[2];

    /* open the source file */
    infile = fopen(iFileName, "r");
    outfile = fopen(oFileName, "w");
    
    if (!infile) {
      printf("Error: Cannot open file %s\n", argv[1]);
      exit(4);
    }
    if (!outfile) {
      printf("Error: Cannot open file %s\n", argv[2]);
      exit(4);
    }

    // first pass: symbol table
    symbol_count = BuildSymbolTable(infile, symbol_table);

    for(int i=0; i<symbol_count; i++){
      printf("Name: %s\nAddress: 0x%.4X\n\n", symbol_table[i].name, symbol_table[i].address);
    }
    printf("Program Start: 0x%.4X\n", prog_start);

    // second pass
    rewind(infile);
    int check_done = 1;
    
    while (check_done == 1) {
      check_done = second_pass(infile, outfile);
    }

    fclose(infile);
    fclose(outfile);
}
