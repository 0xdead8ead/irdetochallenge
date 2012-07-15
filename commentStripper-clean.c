





#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>


#define MIN_REQUIRED 2



 
int help(char *argv[]) {
	printf("Usage: %s [-i <inputFile>] [-o <outputFile>]\n", argv[0]);
	printf("\t-i: input file name\n");
	printf("\t-i: ouput file name\n");
	return 1;
}

 
int parseArgs(int argc, char *argv[], char ** inputFilename, char ** outputFilename){

	if (argc < MIN_REQUIRED) {
		return help(argv);
	}

	 
	int i;
	for (i = 1; i < (argc - 1); i++) {
		if (strcmp("-i", argv[i]) == 0) {
			  
			printf("Input File %s Received\n\n", argv[i+1]);
			*inputFilename = argv[i+1];
			continue;
		}
		if (strcmp("-o", argv[i]) == 0) {
			  
			printf("Output File %s Received\n\n", argv[i+1]);
			*outputFilename = argv[i+1];
			continue;
		}
		
	}
	return 1;
}

char * getFileContents(char * inputFilename){
	FILE * filePointer;
	long fileSize;
	char * fileContents = NULL;
	
	
	if((filePointer = fopen(inputFilename, "r")) == NULL){
		perror("Unable to open file");
		exit(1);
	}
	else{
		
		
		fseek(filePointer, 0L, SEEK_END);
		fileSize = ftell(filePointer);
		rewind(filePointer);

		
		fileContents = calloc(1, fileSize+1);
		if((fileContents = calloc(1, fileSize+1)) == NULL){ 
			fclose(filePointer);
			perror("Unable to allocate memory");
			exit(1);
		}
		else{
			
			if(fread(fileContents, fileSize, 1, filePointer) != 1){
  				fclose(filePointer);
  				free(fileContents);
  				perror("Unable to read contents");
  				exit(1);
			}
			
			fileContents[fileSize] = '\0';
			fclose(filePointer);
			return fileContents;
		}
		fclose(filePointer);
	}
	return fileContents;
}

void saveStrippedContents(char * outputFilename, char * fileContents){
	FILE * filePointer;
	long fileSize = strlen(fileContents);
	
	
	if((filePointer = fopen(outputFilename, "w")) == NULL){
		perror("Unable to open file");
		exit(1);
	}
	else{
		
		if(fwrite(fileContents, fileSize, 1, filePointer) != 1){
				fclose(filePointer);
				free(fileContents);
				perror("Unable to write contents");
				exit(1);
		}
		
		fclose(filePointer);
	}
}

void removeCComments(char * inputString){
	int beginningOfComment = 0;
	int endOfComment = 0;
	int index;
	for(index=0; index < ((int)strlen(inputString))-1;index++){
		
		if(inputString[index] == '"'){
			int i=index+1;
			while(inputString[i] != '"'){
				i++;
			}
			index = i+1;
		}
		else if(inputString[index] == 39){
			int i=index+1;
			while(inputString[i] != 39){
				i++;
			}
			index = i+1;
		}

		
		if(inputString[index] == '#'){
			int i;
			for(i=index; i < ((int)strlen(inputString))-1;i++){
				if(inputString[i] == '/' && inputString[i+1] == '*'){	
					inputString[i] = ' ';
					beginningOfComment = i+1;
					int secondIndex;
					for(secondIndex=beginningOfComment;secondIndex < ((int)strlen(inputString))-1;secondIndex++){
						
							
							if(inputString[secondIndex] == '*' && inputString[secondIndex+1] == '/'){
								endOfComment = secondIndex + 2;
								break;
							}
							else{
								
								endOfComment = 0;
							}
					}
					if(endOfComment != 0){
						memmove(&inputString[beginningOfComment], &inputString[endOfComment], strlen(inputString) - endOfComment);
						inputString[strlen(inputString)-(endOfComment - beginningOfComment)] = '\0';
						break;
					}
				}
			}
		}	

		

		
		if(inputString[index] == '/' && inputString[index+1] == '*'){
			beginningOfComment = index;
			int secondIndex;
			for(secondIndex=beginningOfComment;secondIndex < ((int)strlen(inputString))-1;secondIndex++){
				
				
				if(inputString[secondIndex] == '*' && inputString[secondIndex+1] == '/'){
					endOfComment = secondIndex + 2;
					break;
				}
				else{
					
					endOfComment = 0;
				}
			}
			if(endOfComment != 0){
				memmove(&inputString[beginningOfComment], &inputString[endOfComment], strlen(inputString) - endOfComment);
				inputString[strlen(inputString)-(endOfComment - beginningOfComment)] = '\0';
			}

		}
		else{
			
		}
	}
	return;
}

void removeCppComments(char * inputString){
	int beginningOfComment = 0;
	int endOfComment = 0;

	int index;
	for(index=0; index < ((int)strlen(inputString))-1;index++){
		
		if(inputString[index] == '"'){
			int i=index+1;
			while(inputString[i] != '"'){
				i++;
			}
			index = i+1;
		}
		else if(inputString[index] == 39){
			int i=index+1;
			while(inputString[i] != 39){
				i++;
			}
			index = i+1;
		}
		
		
		if(inputString[index] == '/' && inputString[index+1] == '/'){
			beginningOfComment = index;
			int secondIndex;
			for(secondIndex=beginningOfComment;secondIndex < ((int)strlen(inputString))-1;secondIndex++){
				
				
				if(inputString[secondIndex] == '\n'){
					endOfComment = secondIndex;
					break;
				}
				
				if(inputString[secondIndex] == '\\' && inputString[secondIndex+1] == '\n'){
					secondIndex = secondIndex + 2;
					continue;
				}
				else{
					
					endOfComment = 0;
				}
			}
			if(endOfComment != 0){
				memmove(&inputString[beginningOfComment], &inputString[endOfComment], strlen(inputString) - endOfComment);
				inputString[strlen(inputString)-(endOfComment - beginningOfComment)] = '\0';
			}

		}
		else{
			
		}
		
	}
	return;
}

char * stripComments(char * inputString){
	
	removeCComments(inputString);
	removeCppComments(inputString);

	
	printf("\n\nContents of String after removing C Comments\n=========================================\n%s",inputString);

}



int main(int argc, char *argv[]){

	
	char * inputFilename = "cfile.c";
	char * outputFilename = "output.c";

	
	parseArgs(argc, argv, &inputFilename, &outputFilename);

	printf("Stripping Comments from File: %s\n\n", inputFilename);

	
	char * stringToBeParsed = getFileContents(inputFilename);

	 
	printf("File Contents before Stripping\n==============================\n%s", stringToBeParsed);

	stripComments(stringToBeParsed);

	saveStrippedContents(outputFilename, stringToBeParsed);

	return 0;

}