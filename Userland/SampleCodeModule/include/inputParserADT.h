#ifndef _INPUT_PARSER_ADT_H
#define _INPUT_PARSER_ADT_H

#define MAX_COMMAND_LENGTH 32
#define MAX_PARAM_LENGTH 128
#define MAX_CHAINED_PROGRAMS 2
#define MAX_ARGS 3

typedef struct ShellProgram {
	char *name;
	char **params;
} ShellProgram;

typedef struct InputParserCDT *InputParserADT;

InputParserADT parseInput(char *input);
int8_t getProgramQuantity(InputParserADT parser);
ShellProgram *getProgram(InputParserADT parser, int index);
void freeParser(InputParserADT parser);

#endif