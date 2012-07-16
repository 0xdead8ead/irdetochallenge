// commentStripper.c
// Author: Chase Schultz
// Date: 07/15/2012


// Includes
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

// Defined Constants
#define MIN_REQUIRED 3



/* display usage - Customarily this is where I'd insert some awesome ascii art... Keeping it professional I will refrain */
int help(char *argv[]) {
	printf("Usage: %s [-i <inputFile>] [-o <outputFile>]\n", argv[0]);
	printf("\t-i: input file name\n");
	printf("\t-o: ouput file name\n");
	exit(1);
}

/* Check / Parse Arguments */
int parseArgs(int argc, char *argv[], char ** inputFilename, char ** outputFilename){

	if (argc < MIN_REQUIRED) {
		return help(argv);
	}

	/* iterate over all arguments define input & output file */
	int i;
	for (i = 1; i < (argc - 1); i++) {
		if (strcmp("-i", argv[i]) == 0) {
			/* do something with it */ 
			printf("Input File %s Received\n\n", argv[i+1]);
			*inputFilename = argv[i+1];
			continue;
		}
		if (strcmp("-o", argv[i]) == 0) {
			/* do something with it */ 
			printf("Output File %s Received\n\n", argv[i+1]);
			*outputFilename = argv[i+1];
			continue;
		}
		//return help(argv);
	}
	return 1;
}

char * getFileContents(char * inputFilename){
	FILE * filePointer;
	long fileSize;
	char * fileContents = NULL;
	
	/* Open File, If problem - error out */
	if((filePointer = fopen(inputFilename, "r")) == NULL){
		perror("Unable to open file");
		exit(1);
	}
	else{
		
		/* Find out how big the file is ... */
		fseek(filePointer, 0L, SEEK_END);
		fileSize = ftell(filePointer);
		rewind(filePointer);

		/* Allocate memory for entire file contents - If problem - error out */
		fileContents = calloc(1, fileSize+1);
		if((fileContents = calloc(1, fileSize+1)) == NULL){ 
			fclose(filePointer);
			perror("Unable to allocate memory");
			exit(1);
		}
		else{
			/* Copy the contents of the file into the fileContents */
			if(fread(fileContents, fileSize, 1, filePointer) != 1){
  				fclose(filePointer);
  				free(fileContents);
  				perror("Unable to read contents");
  				exit(1);
			}
			/* Make sure the string is null terminated */
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
	
	/* Open File, If problem - error out */
	if((filePointer = fopen(outputFilename, "w")) == NULL){
		perror("Unable to open file");
		exit(1);
	}
	else{
		/* Copy the stripped content to the output file */
		if(fwrite(fileContents, fileSize, 1, filePointer) != 1){
				fclose(filePointer);
				free(fileContents);
				perror("Unable to write contents");
				exit(1);
		}
		/* Make sure the string is null terminated */
		fclose(filePointer);
	}
}

void removeCComments(char * inputString){
	int beginningOfComment = 0;
	int endOfComment = 0;
	int index;
	for(index=0; index < ((int)strlen(inputString))-1;index++){
		/* work around for string literals */
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

		/* Bonus Question # Statements handled as C Preprocessor
			
			Using Reference found here:

			http://stackoverflow.com/questions/1510869/does-the-c-preprocessor-strip-comments-or-expand-macros-first
			
			In removing comments - the preproccesor inserts a single space.
		 */
		if(inputString[index] == '#'){
			int i;
			for(i=index; i < ((int)strlen(inputString))-1;i++){
				if(inputString[i] == '/' && inputString[i+1] == '*'){	
					inputString[i] = ' ';
					beginningOfComment = i+1;
					int secondIndex;
					for(secondIndex=beginningOfComment;secondIndex < ((int)strlen(inputString))-1;secondIndex++){
						
							/* Find the End of a Comment */
							if(inputString[secondIndex] == '*' && inputString[secondIndex+1] == '/'){
								endOfComment = secondIndex + 2;
								break;
							}
							else{
								/* END OF COMMENT NOT FOUND */
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

		

		/* Strip straight C comments */
		if(inputString[index] == '/' && inputString[index+1] == '*'){
			beginningOfComment = index;
			int secondIndex;
			for(secondIndex=beginningOfComment;secondIndex < ((int)strlen(inputString))-1;secondIndex++){
				
				/* Find the End of a Comment */
				if(inputString[secondIndex] == '*' && inputString[secondIndex+1] == '/'){
					endOfComment = secondIndex + 2;
					break;
				}
				else{
					/* END OF COMMENT NOT FOUND */
					endOfComment = 0;
				}
			}
			if(endOfComment != 0){
				memmove(&inputString[beginningOfComment], &inputString[endOfComment], strlen(inputString) - endOfComment);
				inputString[strlen(inputString)-(endOfComment - beginningOfComment)] = '\0';
			}

		}
		else{
			/* BEGINNING OF COMMENT NOT FOUND */
		}
	}
	return;
}

void removeCppComments(char * inputString){
	int beginningOfComment = 0;
	int endOfComment = 0;

	int index;
	for(index=0; index < ((int)strlen(inputString))-1;index++){
		/* work around for string literals */
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
		
		/* Strip straight C++ comments */
		if(inputString[index] == '/' && inputString[index+1] == '/'){
			beginningOfComment = index;
			int secondIndex;
			for(secondIndex=beginningOfComment;secondIndex < ((int)strlen(inputString))-1;secondIndex++){
				
				/* Find the End of a Comment */
				if(inputString[secondIndex] == '\n'){
					endOfComment = secondIndex;
					break;
				}
				/* In the case the line is terminated by \ char the next line is also a comment */
				if(inputString[secondIndex] == '\\' && inputString[secondIndex+1] == '\n'){
					secondIndex = secondIndex + 2;
					continue;
				}
				else{
					/* END OF COMMENT NOT FOUND */
					endOfComment = 0;
				}
			}
			if(endOfComment != 0){
				memmove(&inputString[beginningOfComment], &inputString[endOfComment], strlen(inputString) - endOfComment);
				inputString[strlen(inputString)-(endOfComment - beginningOfComment)] = '\0';
			}

		}
		else{
			/* BEGINNING OF COMMENT NOT FOUND */
		}
		
	}
	return;
}

char * stripComments(char * inputString){
	/* Remove C Comments */
	removeCComments(inputString);
	removeCppComments(inputString);

	/* DEBUG STATEMENTE - DELETE THIS */
	printf("\n\nContents of String after removing C Comments\n=========================================\n%s",inputString);

}



int main(int argc, char *argv[]){

	/* input / output file defaults */
	char * inputFilename = "irdetoexample.c";
	char * outputFilename = "irdetoexample-clean.c";

	/* Parse arguments, passing references to input / output file */
	parseArgs(argc, argv, &inputFilename, &outputFilename);

	printf("Stripping Comments from File: %s\n\n", inputFilename);

	/* Get contents of file and put it in a character array */
	char * stringToBeParsed = getFileContents(inputFilename);

	/* Output unstripped filecontents to screen - DEBUG - DELETE THIS*/ 
	printf("File Contents before Stripping\n==============================\n%s", stringToBeParsed);

	stripComments(stringToBeParsed);

	saveStrippedContents(outputFilename, stringToBeParsed);

	return 0;

}