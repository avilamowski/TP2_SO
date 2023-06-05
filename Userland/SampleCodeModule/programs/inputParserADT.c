// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
static void cleanSpaces(char **input);
static void freeProgram(ShellProgram *program);

InputParserADT parseInput(char *input) {
	InputParserADT inputParserADT = (InputParserADT) malloc(sizeof(InputParserCDT));
	if (inputParserADT == NULL)
		return NULL;

	inputParserADT->shellPrograms = (ShellProgram **) malloc(MAX_CHAINED_PROGRAMS * sizeof(ShellProgram *));
	if (inputParserADT->shellPrograms == NULL) {
		free(inputParserADT);
		return NULL;
	}

	for (int i = 0; i < MAX_CHAINED_PROGRAMS; i++)
		inputParserADT->shellPrograms[i] = NULL;
	inputParserADT->qtyPrograms = 1;
	inputParserADT->background = 0;

	cleanSpaces(&input);
	if (*input == '\n' || *input == '\0') {
		inputParserADT->qtyPrograms = 0;
		return inputParserADT;
	}
	ShellProgram *firstProgram = parseProgram(inputParserADT, &input);
	ShellProgram *secondProgram = NULL;
	if (*input == PIPE) {
		input++;
		cleanSpaces(&input);
		secondProgram = parseProgram(inputParserADT, &input);
		inputParserADT->qtyPrograms++;
	}

	if (*input == AMPERSAND)
		inputParserADT->background = 1;

	inputParserADT->shellPrograms[0] = firstProgram;
	inputParserADT->shellPrograms[1] = secondProgram;
	return inputParserADT;
}

static ShellProgram *parseProgram(InputParserADT parser, char **input) {
	uint8_t qtyParams = 0;
	ShellProgram *program = (ShellProgram *) malloc(sizeof(ShellProgram));
	if (program == NULL)
		return NULL;

	program->params = (char **) malloc((MAX_PARAM_LENGTH + 2) * sizeof(char *));
	if (program->params == NULL) {
		free(program);
		return NULL;
	}

	program->name = (char *) malloc(MAX_COMMAND_LENGTH);
	if (program->name == NULL) {
		free(program->params);
		free(program);
		return NULL;
	}

	*input += strcpycharlimited(program->name, *input, ' ', MAX_COMMAND_LENGTH);
	cleanSpaces(input);

	program->params[qtyParams] = malloc(MAX_COMMAND_LENGTH);
	if (program->params[qtyParams] == NULL) {
		freeProgram(program);
		return NULL;
	}

	strcpy(program->params[qtyParams++], program->name);

	int lastParamCopiedLength = 1;
	while (lastParamCopiedLength > 0 && **input != AMPERSAND && **input != PIPE && **input != '\n') {
		program->params[qtyParams] = malloc(MAX_PARAM_LENGTH);
		if (program->params[qtyParams] == NULL) {
			freeProgram(program);
			return NULL;
		}
		lastParamCopiedLength = strcpycharlimited(program->params[qtyParams], *input, ' ', MAX_PARAM_LENGTH);
		if (lastParamCopiedLength > 0) {
			*input += lastParamCopiedLength;
			cleanSpaces(input);
			qtyParams++;
		}
	}

	program->params[qtyParams] = NULL;
	return program;
}

static void cleanSpaces(char **input) {
	while (**input == ' ')
		(*input)++;
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
	free(program);
}

void freeParser(InputParserADT parser) {
	for (int i = 0; i < MAX_CHAINED_PROGRAMS; i++) {
		if (parser->shellPrograms[i] != NULL)
			freeProgram(parser->shellPrograms[i]);
	}
	free(parser->shellPrograms);
	free(parser);
}