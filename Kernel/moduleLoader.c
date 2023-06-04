// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <lib.h>
#include <moduleLoader.h>
#include <stdint.h>
#include <video.h>

static void loadModule(uint8_t **module, void *targetModuleAddress);
static uint32_t readUint32(uint8_t **address);

uint64_t loadModules(void *payloadStart, void **targetModuleAddress) {
	int i;
	uint8_t *currentModule = (uint8_t *) payloadStart;
	uint32_t moduleCount = readUint32(&currentModule);

	for (i = 0; i < moduleCount; i++)
		loadModule(&currentModule, targetModuleAddress[i]);
	return (uint64_t) ((void *) currentModule - payloadStart);
}

static void loadModule(uint8_t **module, void *targetModuleAddress) {
	uint32_t moduleSize = readUint32(module);
	memcpy(targetModuleAddress, *module, moduleSize);
	*module += moduleSize;
}

static uint32_t readUint32(uint8_t **address) {
	uint32_t result = *(uint32_t *) (*address);
	*address += sizeof(uint32_t);
	return result;
}
