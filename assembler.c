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

// Symbol Table

typedef struct Label {
    char array[20]; 
    int address;
} Label;

Label symbol_table[255] = {0};


//Parsing Command Line Arguments
int main1(int argc, char* argv[]) {

     char *prgName   = NULL;
     char *iFileName = NULL;
     char *oFileName = NULL;

     prgName   = argv[0];
     iFileName = argv[1];
     oFileName = argv[2];

     printf("program name = '%s'\n", prgName);
     printf("input file name = '%s'\n", iFileName);
     printf("output file name = '%s'\n", oFileName);
}


//Opening And Closing Files
FILE* infile = NULL;
FILE* outfile = NULL;

int
main2(int argc, char* argv[]) {
	
     /* open the source file */
     infile = fopen(argv[1], "r");
     outfile = fopen(argv[2], "w");
		 
     if (!infile) {
       printf("Error: Cannot open file %s\n", argv[1]);
       exit(4);
		 }
     if (!outfile) {
       printf("Error: Cannot open file %s\n", argv[2]);
       exit(4);
     }

     /* Do stuff with files */

     fclose(infile);
     fclose(outfile);
}

int main()
{    

}

