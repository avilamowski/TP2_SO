#include <inputParserADT.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syscalls.h>

typedef struct InputParserCDT {
	ShellProgram **shellPrograms;
	uint8_t qtyPrograms;
	uint8_t background;
} InputParserCDT;

#define PIPE '|'
#define AMPERSAND '&'

static ShellProgram *parseProgram(InputParserADT parser, char **input);

InputParserADT parseInput(char *input) {
	InputParserADT inputParserADT = malloc(sizeof(InputParserCDT));
	inputParserADT->shellPrograms = malloc(MAX_CHAINED_PROGRAMS * sizeof(ShellProgram *));
	for (int i = 0; i < MAX_CHAINED_PROGRAMS; i++)
		inputParserADT->shellPrograms[i] = NULL;
	inputParserADT->qtyPrograms = 1;
	inputParserADT->background = 0;

	ShellProgram *firstProgram = parseProgram(inputParserADT, &input);
	ShellProgram *secondProgram = NULL;
	if (*input == PIPE) {
		input++;
		while (*input == ' ') // TODO: Ver si pasar a una funcion
			input++;
		secondProgram = parseProgram(inputParserADT, &input);
	}

	if (*input == AMPERSAND)
		inputParserADT->background = 1;
	inputParserADT->shellPrograms[0] = firstProgram;
	inputParserADT->shellPrograms[1] = secondProgram;
	return inputParserADT;
}

static ShellProgram *parseProgram(InputParserADT parser, char **input) {
	uint8_t qtyParams = 0;
	ShellProgram *program = malloc(sizeof(ShellProgram));
	program->params = malloc((MAX_PARAM_LENGTH + 2) * sizeof(char *));

	program->name = malloc(MAX_COMMAND_LENGTH);
	*input += strcpycharlimited(program->name, *input, ' ', MAX_COMMAND_LENGTH);
	while (**input == ' ')
		(*input)++;

	program->params[qtyParams] = malloc(MAX_COMMAND_LENGTH);
	strcpy(program->params[qtyParams++], program->name);

	int lastParamCopiedLength;
	do {
		program->params[qtyParams] = malloc(MAX_PARAM_LENGTH);
		lastParamCopiedLength = strcpycharlimited(program->params[qtyParams], *input, ' ', MAX_PARAM_LENGTH);

		if (lastParamCopiedLength <= 0 || program->params[qtyParams][0] == AMPERSAND || program->params[qtyParams][0] == PIPE) {
			free(program->params[qtyParams]);
			break;
		}
		else {
			*input += lastParamCopiedLength;
			while (**input == ' ')
				(*input)++;
			qtyParams++;
		}
	} while (lastParamCopiedLength > 0);

	program->params[qtyParams] = NULL;
	return program;
}

int8_t getProgramQuantity(InputParserADT parser) {
	if (parser == NULL)
		return -1;
	return parser->qtyPrograms;
}

ShellProgram *getProgram(InputParserADT parser, int index) {
	if (parser == NULL || index < 0 || index > MAX_CHAINED_PROGRAMS)
		return NULL;
	return parser->shellPrograms[index];
}

int isInBackground(InputParserADT parser) {
	return parser->background;
}

static void freeProgram(ShellProgram *program) {
	int i = 0;
	free(program->name);
	while (program->params[i] != NULL)
		free(program->params[i++]);
	free(program->params);
}

void freeParser(InputParserADT parser) {
	for (int i = 0; i < MAX_CHAINED_PROGRAMS; i++) {
		if (parser->shellPrograms[i] != NULL)
			freeProgram(parser->shellPrograms[i]);
	}
	free(parser->shellPrograms);
	free(parser);
}