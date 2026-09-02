/* 
Name 1: Braden McElroy
Name 2: Anna Ledbetter
UTEID 1: BRM3367
UTEID 2: AML6995
*/

/*
TODO :
1. make a label struct - DONE
2. make instruction array (and other words??? FOR?)
3. make a main loop "for new line: check for symbol", add to array...
    a. if not '.' or ';' or 'x' or 'X'
    b. if not an instruction
    c. len = 1-20
    d. for char (can we treat the whole thing as a string?) : 
        is letter / num, add to array, else not a valid label 
*/

/* 
TODO : for runthrough 2 (instruction translation)
1. test with just one line of input
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
	
  enum
	{
	   DONE, OK, EMPTY_LINE
	};

int prog_start;

typedef char opcode[5];

opcode opcodes[28] = 
{
"add", "and", "halt", "jmp", "jsr", "jsrr", "ldb",
"ldw", "lea", "nop", "not", "ret", "lshf", "rshfl",
"rshfa", "rti", "stb", "stw", "trap", "xor", "brn",
"brz", "brp", "brnz", "brnp", "brzp", "br", "brnzp"
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


void BuildSymbolTable(FILE *pInFile, Label symbol_table[]) {
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
            symbol_table[i].address = prog_start + (line_count*2);
            strcpy(symbol_table[i].name, lLabel);
            symbol_table[i].name[MAX_LABEL_LEN] = '\0';
            i++;
          }
        }
        line_count++;
    } while (lRet != DONE);
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

    BuildSymbolTable(infile, symbol_table);

    int i = 0;
    while (!strcmp(symbol_table[i].name, 0)){
      printf("Name: %s\nAddress: %d\n\n", symbol_table[i].name, symbol_table[i].address);
    }
    printf("Program Start: %d\n", prog_start);

    fclose(infile);
    fclose(outfile);
}
