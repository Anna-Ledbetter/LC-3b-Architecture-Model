/* 
Name 1: Braden McElroy
Name 2: Anna Ledbetter
UTEID 1: BRM3367
UTEID 2: AML6995
*/

/*
TODO :
1. make a label struct - DONE
2. make instruction array, invalid array - DONE
  a. and other words??? FOR?
3. make a main loop "for new line: check for symbol", add to array... - DONE
    a. EITHER ADD PSEUDO CODE IN ISVALID or IN 2ND PASS - DONE (2nd pass)
    b. if not an instruction - DONE
    c. len = 1-20 - DONE
    d. for char (can we treat the whole thing as a string?) : 
        is letter / num, add to array, else not a valid label - i don't even understand this question and I know I wrote it
    e. WHAT DO WE DO IF JUST NOT VALID CODE, RETURN ERROR?
*/

/* 
TODO : for runthrough 2 (instruction translation)
 * do testion with just one line of input
1. decide on opcode name and decimal relationship, 
  we can add it to opcode struct 
  OR for loop existing struct and use that as an index into the decimal array @braden up to you
2. shift bits
  a. opcode decimal *2^12 
  b. arg 
    i. (only arg2 / 3) immediate through toNum
    ii. R# --> # *2^(arg#)
    iii. else, = label, so look it up (also keep a cntr like in symbol_table)
  
*/

/* TODO : Sept 13
  1. DONE - take care of arg decode --> type, ex) ADD imm vs Reg 
  2. copy the reg code to all switch cases

  TO TEST :
  1. arg_type() works
  2. how does nzp get stored for BR? just Arg1?


*/

#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
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
  {"lea",    0b1110000000000000, MEM},
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
    if ( isalnum(str[n] == 0)) {
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

int sext(int num, int bits) { // @Braden, what is this for?

}

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

int toBin(int num) { // @Braden, what is this for?

}

int BuildSymbolTable(FILE *pInFile, Label symbol_table[]) {
    char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, 
    *lArg1,*lArg2, *lArg3, *lArg4;

    int lRet;
    int i = 0;
    int line_count = 0;

    do{
      lRet = readAndParse(pInFile, lLine, &lLabel, 
        &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);
        if (lRet != DONE && lRet != EMPTY_LINE){
          if (!strcmp(lOpcode, ".orig")) {
            prog_start = toNum(lArg1);
          }
          if (strlen(lLabel)){
            if (isValid(lLabel)){
              symbol_table[i].address = prog_start + (line_count*2);
              strcpy(symbol_table[i].name, lLabel);
              i++;
            }
          }
        }
        line_count++;
    } while (lRet != DONE);
    return i;
}

int arg_type( char *pArg ) { // pass by val not ref, so it's read only (makes a copy var on)
  if (pArg[0] == 'r') {
    return REGISTER;
  }
  if (isdigit(pArg[0]) || (pArg[0] == 'x') || (pArg[0] == '#') || (pArg[0] == '-')) {
    return NUM;
  }
  return LABEL;
}

int second_pass(FILE *pInFile, FILE *pOutfile) {
  char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, 
  *lArg1,*lArg2, *lArg3, *lArg4;

  int lInstr;

  int lRet;
  int i = 0;
  int line_count = 0;
  int line_bin;
  int curr_bit = 0;

  do{
    lRet = readAndParse(pInFile, lLine, &lLabel, 
      &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);
      if (lRet != DONE && lRet != EMPTY_LINE){
        if (!strcmp(lOpcode, ".orig") || !strcmp(lOpcode, ".fill")) {
          lInstr = toNum(lArg1); 
          printf("0x%.4X\n", lInstr);
          fprintf( pOutfile, "0x%.4X\n", lInstr );
        }
        if (!strcmp(lOpcode, ".end")) {
          return 0;
        }
        
        // add opcode to binary line
        int i;
        for (i=0; i<28; i++) {
          if (!strcmp(lOpcode, opcodes[i].name)) {
            line_bin = opcodes[i].binary;
            curr_bit += 4;
          }
        }

        switch (opcodes[i].type) {
          case MATH: {
            int reg = atoi(&lArg1[1]); // only care about register number
            line_bin += (reg * (1<<9) );

            int reg = atoi(&lArg2[1]); 
            line_bin += reg * (1<<9);

            int type = arg_type(lArg3);
            if (type == REGISTER) {
              int reg = atoi(&lArg3[1]); 
              line_bin += reg;
            }
            if (type == NUM){
              line_bin += toNum(lArg3);
            }
            break;
          }
          case MEM: {
            // TODO
            break;
          } 
          case FIXED: {
            break;
          }
          case SHIFT: {
            // TODO
            break;
          }
          case TRAP: {
            // TODO
            break;
          }
          case PC_OFFSET: {
            // TODO
            break;
          }
          case JUMP: {
            // TODO:
            break;
          }
          default: {
            printf("Error: Unsupported opcode type\n");
            break;
          }
        }


              
        


        /* my understanding is that my function will just take in one arg and you'll call it 
            however many times you need from switch cases
            - also, now you need to process whatever arg you passed in based on 
              what type I return
              - so for example toNum() if I return NUM
              - REGISTER, number = Arg + 1, atoi(number)
              - LABEL, iterate through array and add the digit that corrosponds to the name
        */

        char lArg[MAX_LINE_LENGTH + 1]; // I wrote this, but what even is the point? ig this was to proccess args, but it won't work
        strcpy(lArg, lArg1);
        for(int n = 1; n <= 2; n++) {
          printf("%s\n", lArg);
          
          strcpy(lArg, lArg2);
        }
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
    int symbol_count = BuildSymbolTable(infile, symbol_table);

    for(int i=0; i<symbol_count; i++){
      printf("Name: %s\nAddress: %d\n\n", symbol_table[i].name, symbol_table[i].address);
    }
    printf("Program Start: %d\n", prog_start);

    // second pass
    rewind(infile);
    int check_done = 1;
    
    while (check_done == 1) {
      check_done = second_pass(infile, outfile);
    }

    fclose(infile);
    fclose(outfile);
}
