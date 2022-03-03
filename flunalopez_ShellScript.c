/**
 * flunalopez_tristanbailey_ShellScript:
 * 	Program simulates a shell with basic functionaility in the console.
 *
 * @author: Froilan Luna-Lopez
 * @author: Tristan Bailey
 * 	University of Nevada, Reno
 * 	27 February 2022
 * 
 *
 */

// Libraries
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Macros
#define ARRAY_MAXSIZE 100

/*
 */
char* redirectCommand(char* special, char* line, bool* isDirect, char* tokens[],
	char* outputTokens[]);

/*
 * launchProcess():
 *  
 * args:
 *  tokens: 2-d array of tokens
 *	numTokens: the number of tokens in the prev list/array
 *	isRedirect: bool determining if command is a redirect
 * return:
 *	none
 */
void launchProcesses(char *tokens[], int numTokens, bool isRedirect);

/*
 * printError():
 * 	Informs user that an error was encountered.
 * args:
 * 	None
 * return
 * 	None
 */
inline void printError();

/*
 * parseInput():
 * 	Parses a given pointer of characters to tokenize delimited by a space character.
 * args:
 * 	input: Pointer to characters to parse.
 * 	splitWords: Pointer to array of characters to save tokenized words into.
 * return:
 * 	Integer with the number of words tokenized into array.
 */
int parseInput(char *input, char *splitWords[]);

/*
 * promptUser():
 * 	Display shell prefix and prompt user for input.
 * args:
 * 	isBatch: Boolean variable indicating whether program is in batch mode or not.
 * return:
 * 	Void
 */
void promptUser(bool isBatch);

int main() {
	// Variables
	char cwd_path[ARRAY_MAXSIZE + 1];

	getcwd(cwd_path, sizeof(cwd_path));	// Save current working directory.

	while(1) {
		// Variables
		char command[ARRAY_MAXSIZE + 1];

		// Initialize shell
		promptUser(false);			// Prompt user with shell prefix
		fgets(command, sizeof(command), stdin);	// Get command
		unsigned int command_len = strlen(command);

		// Parse input
		if (command_len > 0) {
			// Variables
			char* tokens[ARRAY_MAXSIZE + 1];
			unsigned int tokens_count;

		       	// Remove trailing new-line
			if (command[command_len - 1] == '\n') {
				command[command_len - 1] = '\0';
			}

			tokens_count = parseInput(command, tokens);	// Parse input
			
			// TEMP: Print out tokens to shell
			for (int i = 0; i < tokens_count; i++) {
			       printf("%s ", tokens[i]);
			}
	 		printf("\n");

		}
	}
	return 1;
}

void printError() {
	printf("Shell Program Error Encountered\n");
}

int parseInput(char *input, char *splitWords[]) {
	int wordInd = 0;
	splitWords[0] = strtok(input, " ");
	while (splitWords[wordInd] != NULL) {
		splitWords[++wordInd] = strtok(NULL, " ");
	}

	return wordInd;
}

void promptUser(bool isBatch) {
	// Don't prompt user if in batch mode
	if (isBatch) {
		return;
	}

	// Variables
	char* username;				// System username
	char* hostname;				// System hostname
	char cwd_path[ARRAY_MAXSIZE + 1];	// Current working directory path

	
	username = getenv("LOGNAME");		// Get system username
	hostname = getenv("HOSTNAME");		// Get system hostname
	getcwd(cwd_path, sizeof(cwd_path));	// Get current working directory path

	printf("%s@%s:%s $ ", username, hostname, cwd_path); // Prompt user
}

char* redirectCommand(char* special, char* line, bool* isRedirect, char* tokens[], char* outputTokens[]) {
	// Stop if no redirecting wanted
	if (!isRedirect) {
		return NULL;
	}

	// Variables
	char input[ARRAY_MAXSIZE + 1];
	char outputFileName[ARRAY_MAXSIZE + 1];
	input[0] = '\0';
	outputFileName[0] = '\0';
	unsigned int indexCounter = 0;

	// Get input command
	for( ; tokens[indexCounter] != NULL && strchr(tokens[indexCounter], *special) == NULL; indexCounter++) {
		strcat(input, tokens[indexCounter]);
		strcat(input, " ");
	}

	// Test redirect count
	if (tokens[indexCounter - 1] == NULL || strlen(tokens[indexCounter]) != 1) {
		printError();
		return "";
	}

	// Get destination
	indexCounter++;
	for ( ; tokens[indexCounter] != NULL; indexCounter++) {
		strcat(outputFileName, tokens[indexCounter]);
	}

	// Call command and get output
	if (*special == '>') {
		FILE* outFile;
		
		// Test for successful file opening
		if (!(outFile = fopen(outputFileName, "w"))) {
			printError();
			return NULL;
		}

		// Write to file
		for(int i = 0; outputTokens[i] != NULL; i++) {
			fprintf(outFile, "%s", outputTokens[i]);
		}

		fclose(outFile);
	}

	return NULL; //TO-DO: Return output file name
}

void launchProcesses(char *tokens[], int numTokens, bool isRedirect)
{
	for(int i = 0; i < numTokens; ++i) {
		//i will return for this part tomorrow
	}
}
