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

int prog_start;

typedef struct {
  char name[6]; 
  int digit;
} opcode;

opcode opcodes[28] = {
    {"add",    1},
    {"and",    5},
    {"halt",   15},  // same opcode as trap x25
    {"jmp",    12},
    {"jsr",    4},   // bit[11]=1 vs jsrr
    {"jsrr",   4},   // bit[11]=0
    {"ldb",    2},
    {"ldw",    6},
    {"lea",    14},
    {"nop",    0},   // same opcode as br
    {"not",    9},   // encoded as XOR DR,SR,#-1
    {"ret",    12},  // JMP R7 — same opcode as jmp
    {"lshf",   13},
    {"rshfl",  13},  // same opcode as lshf; bit[4]/bit[5] differ
    {"rshfa",  13},  // same opcode as lshf/rshfl
    {"rti",    8},
    {"stb",    3},
    {"stw",    7},
    {"trap",   15},
    {"xor",    9},
    {"brn",    0},   // same opcode as br; n/z/p bits differ
    {"brz",    0},
    {"brp",    0},
    {"brnz",   0},
    {"brnp",   0},
    {"brzp",   0},
    {"br",     0},
    {"brnzp",  0}
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
      if (strcmp((opcodes[i]), str) == 0) {
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

int
toNum( char * pStr )
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
              symbol_table[i].name[MAX_LABEL_LEN] = '\0';
              i++;
            }
          }
        }
        line_count++;
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

        char lArg[MAX_LINE_LENGTH + 1];
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
