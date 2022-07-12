#include "ParseLog.h"

void tokenizeLine(struct Parameters* inputParams,struct recordInfo* curRecord){

	int tokenCount = 3;	
	char* tokenStr = strtok(NULL,"\t");
	int argCount = 0;
	char** argv = inputParams->argv;
	//tokenizes the line and completes logArgs
	while(tokenStr != NULL){
		if(argv[argCount+6]==NULL) break;
		else if((atoi(argv[argCount+6])) == tokenCount){
			strcpy(curRecord->logArgs[argCount],tokenStr);
			strcpy(curRecord->logArgs[argCount+1],"\0");
			argCount++;
		}
		tokenCount++;
		tokenStr = strtok(NULL,"\t");
	}
}

void writeHeaderLine(FILE* fileOutput,struct Parameters* inputParams){
	fprintf(fileOutput,"Timestamp,");
	for(int i = 0; i < sizeof(inputParams->Args)/sizeof(inputParams->Args[0]);i++){
		if(!strcmp(inputParams->Args[i],"\0")){
			fprintf(fileOutput,"\n");
			return;
		}
		fprintf(fileOutput,"%s,",inputParams->Args[i]);
	}
}
void writeChangingRecords(FILE* fileInput,FILE* fileOutput,struct Parameters* inputParams){
	char line[MAX_LINE_SIZE];
	int lineCount = 0;	
	int numInitFiles = 0;
	int incCounter = 0;
	time_t firstTime = 0;
	struct recordInfo* prevRecord;
	struct recordInfo* curRecord;
	struct recordInfo* recordArray[11];
	//Print First Record
	while(fgets(line, sizeof(line),fileInput) != NULL){
		//Header line of log
		if(lineCount == 0){
			writeHeaderLine(fileOutput,inputParams);			
			lineCount++;
			continue;	
		}

		//Allocate space for current record struct
		curRecord = (struct recordInfo*)malloc(sizeof(struct recordInfo));
		
		//Epoch time based on date and time fields
		char* yearDate = strtok(line,"\t");
		char* hourDate = strtok(NULL,"\t");
		time_t curTime = determineEpochTime(yearDate,hourDate);
		curRecord->epochTime = curTime;

		//Initial case
		if(difftime(curTime,inputParams->startTime)<0){
			free(curRecord);
			continue;
		}
		else { 
			if(!firstTime){
				firstTime = curTime;
			}
		}
		//Case where the specified endTime is reached before the EOF
		if(difftime(curTime,inputParams->endTime)==1){
			free(curRecord);
			return;
		}

		//Fills in logArgs
		tokenizeLine(inputParams,curRecord);

		//The first second of the log
		if(difftime(firstTime,curTime) == 0){
			recordArray[numInitFiles] = curRecord; 
			recordArray[numInitFiles+1] = NULL; 
			numInitFiles++;
		}
		else{
			//write the first time block & free
			if(recordArray[0] != NULL){
				prevRecord = writeFirstRecord(recordArray,numInitFiles,fileOutput,inputParams->AT_DEF,inputParams->UTC);
			}
			//General Line Case - previous record gets checked with current record
			if(difftime(curRecord->epochTime,prevRecord->epochTime) != 0 ){
				incCounter = 0;
			}
			else{
				incCounter++;
				if(inputParams->AT_DEF == ATP_NUM){
					incCounter++;
				}
			}
			curRecord->secondFraction = incCounter;

			//check difference between cur and prev - if difference; write
			if(changeInRecords(prevRecord,curRecord)){
				writeRecordInfo(curRecord,fileOutput,inputParams->UTC);
			}
			free(prevRecord);
			prevRecord = curRecord;
		}
		lineCount++;
	}
	//last free not accounted for within the while loop
	free(prevRecord);
}

struct recordInfo* writeFirstRecord(struct recordInfo* recordArray[],int numInitFiles,FILE* fileOutput,int AT_DEF,int UTC){
	struct recordInfo** prev;
	for(struct recordInfo** temp = recordArray;*temp != NULL;temp++){
		numInitFiles = updateSecondFraction(*temp,numInitFiles,AT_DEF);
		if(*temp == recordArray[0] || changeInRecords(*prev,*temp)) {
			writeRecordInfo(*temp,fileOutput,UTC);
		}
		prev = temp;
	}
	for(struct recordInfo** temp = recordArray;temp != prev;temp++){
		free(*temp);	
		*temp = NULL;
	}
	return *prev;
}

bool changeInRecords(struct recordInfo* prevRecord,struct recordInfo* curRecord){
	//Any logs exhibit change
	for(int i = 0; strcmp(curRecord->logArgs[i],"\0") ; i++){
		if(strcmp(prevRecord->logArgs[i],curRecord->logArgs[i])) return true;
	}

	return false;
}
char* convertEpochToString(struct recordInfo* curRecord,char UTCTimestamp[],int UTC){
	struct tm* curTMstruct = localtime(&(curRecord->epochTime));
	curTMstruct->tm_hour -= UTC;
	curTMstruct->tm_isdst = -1;
	mktime(curTMstruct);
	//format is YYYY-MM-DD HH:MI:SS
	if(strftime(UTCTimestamp,MAX_STRING_SIZE,"%Y-%m-%d %H:%M:%S",curTMstruct) != 0){
		return UTCTimestamp;
	}
	else {
		printf("Error: Converting Record %p Of Time %d to String was Unsuccessful\n",curRecord,curRecord->epochTime);
		printf("%s \n",UTCTimestamp);
		exit(EXIT_FAILURE);
	}
}
void writeRecordInfo(struct recordInfo* curRecord,FILE* fileOutput,int UTC){
	//Convert epoch time to string
	char UTCTimestamp[MAX_STRING_SIZE];
	fprintf(fileOutput,"%s.%d,",convertEpochToString(curRecord,UTCTimestamp,UTC),curRecord->secondFraction);

	for(int i = 0; i < sizeof(curRecord->logArgs)/sizeof(curRecord->logArgs[0]);i++){
		if(!strcmp(curRecord->logArgs[i],"\0")){
			fprintf(fileOutput,"\n");
			return;
		}
		fprintf(fileOutput,"%s,",curRecord->logArgs[i]);
	}
}

int updateSecondFraction(struct recordInfo* curRecord,int numInitFiles,int AT_DEF){
	//updates the fraction based on the block containing the first second
	//ATO Logs Inc by 1 
	//ATP Logs Inc by 2
	int logInc = 1;
	if(AT_DEF == ATP_NUM){
		logInc = 2;
	}
	curRecord->secondFraction = 10 - (logInc * numInitFiles);
	return --numInitFiles;
}

void parseLogFile(struct Parameters* inputParams,char outputFilename[]){
	strcat(strcat(inputParams->initFilename,"."),TXT_SUFFIX);
	FILE* fileInput = fopen(inputParams->initFilename,"r");                             	
	FILE* fileOutput = fopen(outputFilename,"w");                             	
	
	if(fileInput == NULL || fileOutput == NULL){
		printf("Unable to open Input/Output files\n");
		exit(EXIT_FAILURE);
	}

	writeChangingRecords(fileInput, fileOutput, inputParams);

	fclose(fileInput);
	fclose(fileOutput);
}
