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
#include <sys/wait.h>

// Macros
#define ARRAY_MAXSIZE 100

/*
 * redirectCommand():
 * 	Directs output received from command to a destination.
 * args:
 * 	@special: Special character within command (e.g., >)
 *	@line: Character pointer to the user inputted command.
 *	@isDirect: Determines whether the output will go to a destination that isn't the console.
 *	@tokens: Tokenized input.
 *	@outputTokens: 2D array of all words to be outputted.
 */
char* redirectCommand(char* special, char* line, bool* isDirect, char* tokens[],
	char* outputTokens[]);

/*
 * exitProgram():
 * 	Tests if tokens from user input is a valid exit call.
 * args:
 * 	@tokens: 2D array containing tokens as strings.
 * 	@numTokens: Integer with the number of tokens.
 * return:
 * 	True if tokens have a valid exit call.
 * 	False otherwise.
 */
bool exitProgram(char* tokens[], int numTokens);

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
 * changeDirectories():
 *  Changes the current working directory if the first argument in tokens is "cd"
 *  and iff there is one other argument that is the directory to be changed to
 *  o.w. it errors
 * args:
 *  tokens: 2-d array of tokens
 *	numTokens: the number of tokens in the prev list/array
 * return:
 *	none
 */
void changeDirectories(char *tokens[], int numTokens);

char *executeCommand(char *cmd, bool *isRedirect, char* tokens[], char* outputTokens[],
	bool *isExits);
/*
 * printError():
 * 	Informs user that an error was encountered.
 * args:
 * 	None
 * return
 * 	None
 */
static inline void printError();

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

			tokens_count = parseInput(command, tokens);	// Parse input
			
			// TEMP: Print out tokens to shell
			for (int i = 0; i < tokens_count; i++) {
			       printf("%s ", tokens[i]);
			}
			printf("\n");

			// TEMP: Test exitProgram
			if (exitProgram(tokens, tokens_count)) return 1;
		}
	}
	return 1;
}

static inline void printError() {
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
	// Variables
	bool passedSpecial = false; // Determines if a special character was encountered.
	char* outputFileName = (char* ) malloc(sizeof(char) * (ARRAY_MAXSIZE + 1)); // String to save the output file name to.
	unsigned int outputFileNameIndex = 0; // Track the end of outputFileName.

	outputFileName[0] = '\0';

	// Extrapolate 
	for (int i = 0; line[i] != '\n'; i++) {
		if (line[i] == '>') { // Special character found.
			if (passedSpecial) { // Return error if more than one special character found.
				printError();
				return outputFileName;
			}
			else { // Special character is first instance.
				passedSpecial = true;
				*isRedirect = true;
			}
		}
		else if (passedSpecial && line[i] != '\0') { // Save characters after encountering special character.
			outputFileName[outputFileNameIndex] = line[i];
			outputFileName[outputFileNameIndex + 1] = '\0';
			outputFileNameIndex++;
		}
	}

	return outputFileName;
}

bool exitProgram(char* tokens[], int numTokens) {
	// Variables
	char strippedToken[ARRAY_MAXSIZE + 1];

	// Remove trailing whitespace on first token
	for (int i = 0; tokens[0][i] != 0; i++) {
		strippedToken[i] = tokens[0][i];
	}

	// Test for valid exit call
	if (strcmp(strippedToken, "exit")) {
		if (numTokens != 1) { // Test for extra arguments
			printError();
			return false;
		}
		else { // Valid exit call
			return true;
		}
	}
}

void launchProcesses(char *tokens[], int numTokens, bool isRedirect)
{
	//loop through each argument after the command
	for(int i = 1; i < numTokens; ++i) {
		pid_t child;
		int result = 0;
		//fork failed
		if (((child = fork()) < 0) && !(strcmp(tokens[0], "exit")) && !(strcmp(tokens[0], "help"))
			&& !(strcmp(tokens[0], "cd"))){
			result = -1;
			//halt process
			i = numTokens;
		}
		//child process
		else if (child == 0){
			result = execvp(tokens[0], tokens);
		}
		//parent process
		else{
			if(result == -1){
				printError();
			}
			else{
				wait(NULL);
			}
		}
	}
}

void changeDirectories(char *tokens[], int numTokens)
{
	//strcmp ignores the tail null char in strings when comparing so "aa\0" == "aa"
	if(strcmp(tokens[0],"cd") == 0){
		//if num args is equalt to 2, which is required by cd then cd
		if(numTokens == 2){
			chdir(tokens[1]);
		}
		//o.w. tell user error
		else{
			printError();
		}
	}
}

char *executeCommand(char *cmd, bool *isRedirect, char* tokens[], char* outputTokens[],
	bool *isExits)
{

}
