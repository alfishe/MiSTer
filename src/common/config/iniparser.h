#ifndef COMMON_CONFIG_INIPARSER_H_
#define COMMON_CONFIG_INIPARSER_H_

#include <inttypes.h>
#include "../types.h"

typedef struct
{
	uint8_t id;
	char* name;
} iniSectionType;

typedef enum
{
	UINT8 = 0,
	INT8,
	UINT16,
	INT16,
	UINT32,
	INT32,
	FLOAT,
	STRING,
	CUSTOM_HANDLER
} iniTypesEnum;

typedef void custom_handler_t(char*);

typedef struct
{
	char* name;
	void* var;
	iniSectionType type;
	int min;
	int max;
	int section_id;
} iniVariableType;

typedef struct
{
	const char* filename;
	const iniSectionType sections[];
	const iniVariableType vars[];
	int nsections;
	int nvars;
} iniConfigType;

class iniparser
{
private:
	FileDescriptor file;
	char buffer[512];

	iniparser() {};

public:
	iniparser(char *filepath);
	bool parse();

protected:
	// Helper methods
	bool getSection();
	bool getLine();
	char getch();

};

#endif /* COMMON_CONFIG_INIPARSER_H_ */
