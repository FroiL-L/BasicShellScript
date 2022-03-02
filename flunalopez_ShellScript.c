/**
 * flunalopez_ShellScript:
 * 	Program simulates a shell with basic functionaility in the console.
 *
 * @author: Froilan Luna-Lopez
 * 	University of Nevada, Reno
 * 	27 February 2022
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
char* redirectCommand(char* special, char* line, bool* isDirect, char* tokens[], char* outputTokens[]) {

}

/*
 * printError():
 * 	Informs user that an error was encountered.
 * args:
 * 	None
 * return
 * 	None
 */
inline void printError() {
	printf("Shell Program Error Encountered\n");
}

/*
 * parseInput():
 * 	Parses a given pointer of characters to tokenize delimited by a space character.
 * args:
 * 	input: Pointer to characters to parse.
 * 	splitWords: Pointer to array of characters to save tokenized words into.
 * return:
 * 	Integer with the number of words tokenized into array.
 */
int parseInput(char *input, char *splitWords[]) {
	int wordInd = 0;
	splitWords[0] = strtok(input, " ");
	while (splitWords[wordInd] != NULL) {
		splitWords[++wordInd] = strtok(NULL, " ");
	}

	return wordInd;
}

/*
 * promptUser():
 * 	Display shell prefix and prompt user for input.
 * args:
 * 	isBatch: Boolean variable indicating whether program is in batch mode or not.
 * return:
 * 	Void
 */
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
