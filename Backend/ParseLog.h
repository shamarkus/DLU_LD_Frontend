#ifndef PARSELOG_H
#define PARSELOG_H

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

//Struct/Global Variable Declarations
struct Parameters { 
	int AT_DEF;
	int UTC;
	time_t startTime;
	time_t endTime;
	int numOfArgs;
	char** argv;
	char Args[MAX_COLUMN_SIZE][MAX_STRING_SIZE];
	char initFilename[MAX_STRING_SIZE];
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

void parseLogFile(struct Parameters* inputParams,char outputFilename[]);

void writeHeaderLine(FILE* fileOutput,struct Parameters* inputParams);			

void writeChangingRecords(FILE* fileInput,FILE* fileOutput,struct Parameters* inputParams);

#endif
