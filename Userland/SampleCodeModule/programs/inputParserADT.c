#include <inputParserADT.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <syscalls.h>

typedef struct InputParserCDT {
	ShellProgram **shellPrograms;
	uint8_t qtyPrograms;
	uint8_t background;
} InputParserCDT;

InputParserADT parseInput(char *input) {
	InputParserADT inputParserADT = malloc(sizeof(InputParserCDT));
	inputParserADT->shellPrograms = malloc(MAX_CHAINED_PROGRAMS * sizeof(ShellProgram *));
	for (int i = 0; i < MAX_CHAINED_PROGRAMS; i++)
		inputParserADT->shellPrograms[i] = NULL;
	inputParserADT->qtyPrograms = 1;
	inputParserADT->background = 0;

	ShellProgram *firstProgram = malloc(sizeof(ShellProgram));
	firstProgram->params = malloc((MAX_PARAM_LENGTH + 2) * sizeof(char *));
	uint8_t qtyParams = 0;

	firstProgram->name = malloc(MAX_COMMAND_LENGTH);
	input += strcpycharlimited(firstProgram->name, input, ' ', MAX_COMMAND_LENGTH);
	while (*input == ' ')
		input++;

	firstProgram->params[qtyParams] = malloc(MAX_COMMAND_LENGTH);
	strcpy(firstProgram->params[qtyParams++], firstProgram->name);

	int lastParamCopiedLength;
	do {
		firstProgram->params[qtyParams] = malloc(MAX_PARAM_LENGTH);
		lastParamCopiedLength = strcpycharlimited(firstProgram->params[qtyParams], input, ' ', MAX_PARAM_LENGTH);
		input += lastParamCopiedLength;
		while (*input == ' ')
			input++;
		if (lastParamCopiedLength > 0)
			qtyParams++;
	} while (lastParamCopiedLength > 0);

	firstProgram->params[qtyParams] = NULL;

	inputParserADT->shellPrograms[0] = firstProgram;
	return inputParserADT;
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