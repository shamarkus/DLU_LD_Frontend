#include "ParseLog.h"
#include <unistd.h>
//IF NOT OKAY FOR WINDOWS (VC) ACCESS() FUNCTION
//THEN INCLUDE THIS
//#ifdef WIN32
//#include <io.h>
//#define F_OK 0
//#define access _access
//#endif
//https://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c


// Executable Command Format : 
// ./DLULogProcessor FILEDIR FILENAME1 FILENAME2 ... LOGTYPE TIME1 TIME2 ARGS
// 
// EXAMPLE : 
//Filenames for conversions between column number and corresponding string for ATP & ATO style files
static const char headerConversions_ATO_Filename[] = "NumToHeaderString_ATO.txt";
static const char headerConversions_ATP_Filename[] = "NumToHeaderString_ATP.txt";
static const char filepath[] = "\\\\station.int.ttc.ca\\ttc\\homedrive\\mkarlov\\desktop\\DLU";
//Function Prototypes/Declarations
int determineATPorATO(struct Parameters* inputParams,char* arg);

time_t determineEpochTime(char* yearDate,char* hourDate);

int determineESTorEDT(time_t tm);

void DetermineSpecificArgs(char* paramStr,struct Parameters* inputParams,char* headerConversionsFilename);

struct Parameters* initializeParams(struct Parameters* inputParams,int argc,char** argv);

void printParams(struct Parameters* inputParams);

bool checkOutputFilenames(struct Parameters* inputParams);

char *strptime_I(const char *buf, const char *fmt, struct tm *tm);

char* strtok_r (char *s, const char *delim, char **save_ptr);

char *strptime_I(const char *buf, const char *fmt, struct tm *tm){
	//Strip the string to get the numeric value
	int Y,m,d,H,M,S;
	sscanf(buf,fmt,&Y,&m,&d,&H,&M,&S);
	
	//check for YYYY-MM-DD or YYYY/MM/DD
	if(!m || !d){
	    return NULL;
	}

	//Configure based on struct tm format
	tm->tm_isdst = -1;
	tm->tm_year= (Y-1900);
	tm->tm_mon = (m-1);
	tm->tm_mday = d;
	tm->tm_hour = H;
	tm->tm_min = M;
	tm->tm_sec = S;
	if(mktime(tm) < 1){
		return NULL;
	}
	return buf;
}

char* strtok_r (char *s, const char *delim, char **save_ptr){
	char *end;
	if (s == NULL)
	s = *save_ptr;
	if (*s == '\0')
	{
		*save_ptr = s;
		return NULL;
	}
	/* Scan leading delimiters.  */
	s += strspn (s, delim);
	if (*s == '\0')
	{
		*save_ptr = s;
		return NULL;
	}
	/* Find the end of the token.  */
	end = s + strcspn (s, delim);
	if (*end == '\0')
	{
		*save_ptr = end;
		return s;
	}
	/* Terminate the token and make *SAVE_PTR point past it.  */
	*end = '\0';
	*save_ptr = end + 1;
	return s;
}

//Returns the numerical value that represents whether the file is ATP or ATO
//Necessary for correct timestamping
//int determineATPorATO(struct Parameters* inputParams,char* arg){
//	char* delimStr = strtok(arg,".");
//	char* tempStr;
//	while(delimStr != NULL){
//		tempStr = delimStr;
//		strcat(inputParams->initFilename,tempStr);
//		delimStr = strtok(NULL,".");
//		if(delimStr != NULL && !strcmp(delimStr,TXT_SUFFIX)){
//			if(*(tempStr+4)=='c') return ATO_NUM;
//			else if(*(tempStr+4)=='8') return ATP_NUM;
//		}
//		strcat(inputParams->initFilename,".");
//	}
//	printf("Error: Passed filename was named incorrectly, and 8001c0 or 800180 was unfound\n");
//	return AT_UNDEFINED_NUM;
//}

//Returns Epoch Time from date strings of format %Y/%m/%d %H:%M:%S
time_t determineEpochTime(char* yearDate,char* hourDate){
	char stringDate[MAX_TIME_STRING_SIZE];	
	sprintf(stringDate,"%s %.*s",yearDate,8,hourDate);
	struct tm* tm = (struct tm*)malloc(sizeof(struct tm));

	if(strptime_I(stringDate,"%d/%d/%d %d:%d:%d",tm) || strptime_I(stringDate,"%d-%d-%d %d:%d:%d",tm)){
		time_t time = mktime(tm);
		free(tm);
		return time;
	}
	else {
		printf("Error: Arguments do not contain the correct time. The current time will be returned\n");
		return time(NULL);
	}
}

//Necessary for determining whether the current timezone is EST or EDT
int determineESTorEDT(time_t tm){
	char timezoneString[MAX_STRING_SIZE];
	if(strftime(timezoneString,MAX_STRING_SIZE,"%Z",localtime(&tm)) != 0){
		if(!strcmp(timezoneString,UTC5_TIME)) return UTC5_TIME_NUM;
		return UTC4_TIME_NUM;
	}
	else {
		printf("Error: Converting to EST or EDT was Unsuccessful\n");
		exit(EXIT_FAILURE);
	}
}

//Takes integer based parameters which refer to a specific header string
//Conversion occurs here, through a .txt file which determines the string from the number of the argument
void DetermineSpecificArgs(char* paramStr,struct Parameters* inputParams,char* headerConversionsFilename){	
	FILE *file = fopen(headerConversionsFilename,"r");
	char *savePtr, *tempParam = strtok_r(paramStr,"\t",&savePtr);

	int argCount = 0, lineCount = 1;
	if(file != NULL){
		char line[MAX_STRING_SIZE];
		while(fgets(line, sizeof(line), file) != NULL){
			if(tempParam == NULL){
				fclose(file);
				return;
			}
			else if((atoi(tempParam)) == lineCount){
				strcpy(inputParams->Args[argCount],strtok(line,"\t"));
				inputParams->argN[argCount] = lineCount;
				strcpy(inputParams->Args[argCount+1],"\0");
				inputParams->argN[argCount+1] = -1;

				tempParam = strtok_r(NULL,"\t",&savePtr);
				argCount++;
			}
			lineCount++;
		}
		fclose(file);
	}
	else {
		printf("Error: NumToHeaderString.txt doesn't exist in the /exe folder\n");
		exit(EXIT_FAILURE);
	}
}
struct fileInfo* determineFileArray(struct Parameters* inputParams,char* fileStr){
	//Dynamically growing fileArray
	struct fileInfo* fileArray = (struct fileInfo*)malloc(sizeof(struct fileInfo));
	char* tempFile = strtok(fileStr,"\t");
	char tempPath[MAX_STRING_SIZE];
	int i, arraySize = 1;

	//for(int i = 0; strcmp(argv[i+2],ATO_STR_NUM) && strcmp(argv[i+2],ATP_STR_NUM); i++){
	while(tempFile != NULL){

		if(i==arraySize){
			arraySize *= 2;
			fileArray = realloc(fileArray,arraySize * sizeof(struct fileInfo));
		}

		//File Info
		strcpy(fileArray[i].fileName,tempFile);
		sprintf(tempPath,"%s%s",inputParams->dirPath,tempFile);
		fileArray[i].fileInput = fopen(tempPath,"r");
		inputParams->fileC = i+1;
		//Core Info
		fileArray[i].core = atoi(strtok(NULL,"\t"));

		i++;
		tempFile = strtok(NULL,"\t");
	}
	return fileArray;
}
//Initializes the input parameters struct object
//Determines start, end times, as well as all other input parameters
//Params initialized in format of :
// ./DLULogProcessor FILENAME : 
struct Parameters* initializeParams(struct Parameters* inputParams,int argc,char** argv){
	//Dynamically allocate space for struct
	inputParams = (struct Parameters*)malloc(sizeof(struct Parameters));
	
	//Determine directory path
	strcpy(inputParams->dirPath,argv[1]);

	//Determine fileArray
	inputParams->fileArray = determineFileArray(inputParams,argv[2]);
	//Determine whether ATP or ATO file
	inputParams->AT_DEF = atoi(argv[3]);

	//Determine Start & End Times 
	//inputParams->startTime = determineEpochTime(argv[2],argv[3]); 
	inputParams->startTime = determineEpochTime(strtok(argv[4],"\t"),"");
	inputParams->endTime = determineEpochTime(strtok(NULL,"\t"),""); 
	//Determine Timezone
	inputParams->UTC = determineESTorEDT(inputParams->endTime);

	//Update Times Based on Timezone And Normalize
	struct tm* startTMstruct = localtime(&(inputParams->startTime));
	startTMstruct->tm_hour += inputParams->UTC;
	inputParams->startTime = mktime(startTMstruct);

	struct tm* endTMstruct = localtime(&(inputParams->endTime));
	endTMstruct->tm_hour += inputParams->UTC;
	inputParams->endTime = mktime(endTMstruct);

	//Determine Parameter Information
	inputParams->argC = argc - 4 - inputParams->fileC;
	char* headerConversionsFilepath[MAX_STRING_SIZE];
	sprintf(headerConversionsFilepath,"%s%s",inputParams->dirPath, (inputParams->AT_DEF) ? headerConversions_ATP_Filename : headerConversions_ATO_Filename);
	DetermineSpecificArgs(argv[5],inputParams,headerConversionsFilepath);

	return inputParams;
}

void printParams(struct Parameters* inputParams){
	printf("%d \n",inputParams->AT_DEF);
	printf("%d \n",inputParams->UTC);
	printf("%d \n",inputParams->startTime);
	printf("%d \n",inputParams->endTime);
	for(int i = 0; i < sizeof(inputParams->Args)/sizeof(inputParams->Args[0]); i++) {
		if(!strcmp(inputParams->Args[i],"\0")) return;
		printf("%s \n",inputParams->Args[i]);
	}
}
bool checkOutputFilenames(struct Parameters* inputParams){
	for(int i = 0; i < inputParams->fileC;i++){
		char outputFileName[MAX_STRING_SIZE];
		sprintf(outputFileName,"%s%s.%s",inputParams->dirPath,inputParams->fileArray[i].fileName,CSV_SUFFIX);
		if (access(outputFileName, F_OK) != 0) {
		    // file doesnt exist
		    inputParams->fileArray[i].fileOutput = fopen(outputFileName,"w+");
		    continue;
		}
		for(int fileCounter=1;fileCounter<100;fileCounter++){
			sprintf(outputFileName,"%s%s-%02d.%s",inputParams->dirPath,inputParams->fileArray[i].fileName,fileCounter,CSV_SUFFIX);
			if (access(outputFileName, F_OK) != 0) {
			    // file doesnt exist
				inputParams->fileArray[i].fileOutput = fopen(outputFileName,"w+");
				break;
			}
			else if(fileCounter == 99){
				printf("Error: The directory is full -- Program will be terminated\n");
				return false;
			}
		}
	}
	return true;
}

int main(int argc,char** argv){

	struct Parameters* inputParams;

	inputParams = initializeParams(inputParams,argc,argv);

	printParams(inputParams);
	
	if(!checkOutputFilenames(inputParams)) return 0;

	parseLogFile(inputParams);

	if(inputParams->fileC > 1) concatLogFiles(inputParams);

	free(inputParams);

	printf("File Has Been Created\n");

	return 0;
}
