#ifndef PARSELOG_H_
#define PARSELOG_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <stdbool.h>
#include <ctype.h>

#define ATP_DEF "ATP"
#define ATO_DEF "ATO"
#define TXT_SUFFIX "txt"
#define CSV_SUFFIX "csv"
#define AT_UNDEFINED_NUM 2
#define ATP_NUM 1
#define ATO_NUM 0
#define ATP_STR_NUM "1"
#define ATO_STR_NUM "0"
#define MAX_COLUMN_SIZE 3096
#define MAX_STRING_SIZE 512
#define MAX_TIME_STRING_SIZE 25
#define MAX_LINE_SIZE 131072
#define UTC0_TIME "UTC+0" 
#define UTC4_TIME "EDT"
#define UTC5_TIME "EST"
#define UTC0_TIME_NUM 0 
#define UTC4_TIME_NUM 4
#define UTC5_TIME_NUM 5
#define MAX_INIT_TIMESTAMPS 11
#define CORE_ONE 1
#define CORE_TWO 2


//Struct/Global Variable Declarations
struct fileInfo {
	char fileName[MAX_STRING_SIZE];
	char curLine[MAX_STRING_SIZE];
	int core;

	time_t curTime;
	int secondFraction;

	FILE* fileInput;
	FILE* fileOutput;
};

struct Parameters { 
	int AT_DEF;
	int UTC;
	time_t startTime;
	time_t endTime;

	int argC;
	int argN[MAX_COLUMN_SIZE];
	char Args[MAX_COLUMN_SIZE][MAX_STRING_SIZE];
	char dirPath[MAX_STRING_SIZE];

	struct fileInfo* fileArray;
	int fileC;
};

struct recordInfo {
	time_t epochTime;
	int secondFraction;
	char logArgs[MAX_COLUMN_SIZE][MAX_STRING_SIZE];
};

bool changeInRecords(struct recordInfo* prevRecord,struct recordInfo* curRecord);

char* convertEpochToString(struct recordInfo* curRecord,char UTCTimestamp[],int UTC);

void writeRecordInfo(struct recordInfo* curRecord,FILE* fileOutput,int UTC);

struct recordInfo* writeFirstRecord(struct recordInfo* recordArray[],int numInitFiles,FILE* fileOutput,int AT_DEF, int UTC);

int updateSecondFraction(struct recordInfo* curRecord,int numInitFiles,int AT_DEF);

void tokenizeLine(struct Parameters* inputParams,struct recordInfo* curRecord);

void writeHeaderLine(FILE* fileOutput,struct Parameters* inputParams);			

void writeChangingRecords(FILE* fileInput,FILE* fileOutput,struct Parameters* inputParams);

void parseLogFile(struct Parameters* inputParams);

bool checkMultCores(struct Parameters* inputParams);

void writeConcatHeader(FILE* outputFile, bool multCores);

void concatLogFiles(struct Parameters* inputParams);

#endif
