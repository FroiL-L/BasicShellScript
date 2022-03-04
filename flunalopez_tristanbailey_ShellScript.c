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

/*
 * promptUser():
 * 	Display shell prefix and prompt user for input.
 * args:
 * 	isBatch: Boolean variable indicating whether program is in batch mode or not.
 * return:
 * 	Void
 */
void promptUser(bool isBatch);

/*
* executeCommand():
*	executes the cd, help, exict, redirect and execvp type arguments that were input by the user
*	through appropriate function calls or launchProcesses
* args:
*	cmd: the line input by the user before parsing
*	isRedirect: a bool that tracks if a redirectCommand() was given and is changed by redirectCommand
*	tokens: a 2-d matrix that holds out tokenized command + arguments fromt he user
*	outputTokens: a 2-d matrix of output tokens updated by redirectCommand()
*	isExits:a bool representign if the user want to quit the program, is updated by exitProgram()
* return:
*	char* which represents the outputfile from a redirect
*/
char *executeCommand(char *cmd, bool *isRedirect, char* tokens[], char* outputTokens[],
	bool *isExits);

/*
* printHelp();
*	Displays the help information/menu telling the user acceptable commands
* args:
*	tokens: a matrix where each row is a token that was parsed from the user input
*	numTokens: the number of tokens parsed
* return:
*	Void
*/
void printHelp(char* tokens[], int numTokens);

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
			for (int i = 0; i < tokens_count; ++i){
				for (int i2 = 0; tokens[i][i2] != '\0'; ++i2)
				{
					printf("%c", tokens[i][i2]);
				}
				printf("\n");
			}
	 		printf("\n");
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
	int numTokens = 0;
	bool passedSpecial = false; // Determines if a special character was encountered.
	char* outputFileName = (char* ) malloc(sizeof(char) * (ARRAY_MAXSIZE + 1)); // String to save the output file name to.
	char inputFileName[ARRAY_MAXSIZE + 1]; // String to save the input file name to.

	outputFileName[0] = '\0';
	inputFileName[0] = '\0';

	// Get number of tokens
	while(tokens[numTokens]) numTokens++;

	// Extrapolate file names from tokens
	for(int i = 1; i <= numTokens; i++) {
		if (passedSpecial) { // Parsing right-hand side of special character
			if (outputFileName[0] != '\0') { // Found too many output file names.
				*isRedirect = false;
				printError();
				return outputFileName;
			}
			else { // Found first output file name.
				strcat(outputFileName, tokens[i]);
			}
		}
		else if (strchr(tokens[i], *special)){ // Found instance of special character.
			if (strlen(tokens[i]) != 1) { // Too many instances found.
				*isRedirect = false;
				printError();
				return outputFileName;
			}
			else { // Valid instance found.
				passedSpecial = true;
				*isRedirect = true;
			}
		}
		else if (inputFileName[0] != '\0') { // Found too many input file names.
			*isRedirect = false;
			printError();
			return outputFileName;
		}
		else { // Found first input file name.
			strcat(inputFileName, tokens[i]);
		}
	}

	return outputFileName;
}

bool exitProgram(char* tokens[], int numTokens) {
	// Variables
	char* backOfToken = tokens[0] + strlen(tokens[0]);
	
	// Remove trailing newline
	while((*--backOfToken) == '\n');
	*(backOfToken + 1) = '\0';
	
	// Test for valid exit call
	if (strcmp(tokens[0], "exit") == 0) {
		if (numTokens != 1) { // Test for extra arguments
			printError();
			return false;
		}
		else { // Valid exit call
			return true;
		}
	}
	return false;
}

void launchProcesses(char *tokens[], int numTokens, bool isRedirect)
{
	//if redirect then do not process with execvp
	if (isRedirect)
	{
		return;
	}
	
	//do not execvp with these three commands
	if(tokens[0] == "cd" || tokens[0] == "redirect" || tokens[0] == "exit" || tokens[0] == "help")
	{
		return;
	}
	//loop through each argument after the command
	for(int i = 1; i < numTokens; ++i) {
		pid_t child;

		char* temp[3];
		temp[0] = tokens[0];
		temp[1] = tokens[i];
		temp[2] = 0;
		int result;
		//fork failed
		if ((child = fork()) < 0){
			printError();
			//halt process
			exit(1);
		}
		//child process
		else if (child == 0){
			result = execvp(temp[0], temp);
			if(result < 0)
			{
				printError();
			}
		}
		//parent process
		else{
			while(!(result < 0) && wait(NULL) != child){;}
		}
	}
}

void changeDirectories(char *tokens[], int numTokens)
{
	//strcmp ignores the tail null char in strings when comparing so "aa\0" == "aa"
	if(strcmp(tokens[0],"cd") == 0){
		//special case where we cd to home directory
		if(numTokens == 1){
			chdir(getenv("HOME"));
		}
		//if num args is equalt to 2, which is required by cd then cd
		else if(numTokens == 2){
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
	//var creation
	char* out_fname = "";
	char* clone = strdup(cmd);
	char* is_redirect = (strchr(cmd, '>'));
	int num_tokens = 0;
	
	//add '\n' for batch executions
	strcat(cmd, "\n");
	
	//check for redirect
	if(is_redirect != NULL){
		//was a redirect so save output file name
		out_fname = redirectCommand(">", clone, isRedirect, tokens, outputTokens);
	}
	else{
		//o.w. parse string and get # of tokens (command + num of args)
		num_tokens = parseInput(clone, tokens);
		//exit if no tokens
		if(num_tokens == 0){
			return out_fname;
		}
		*isExits = exitProgram(tokens, num_tokens);
		changeDirectories(tokens, num_tokens);
		printHelp(tokens, num_tokens);
		launchProcesses(tokens, num_tokens, isRedirect);
	}
	return out_fname;
}

void printHelp(char* tokens[], int numTokens)
{
	if(strcmp(tokens[0],"help") == 0){
		if (numTokens != 1)
		{
			printError();
		}
		else{
			printf("\nFroilin/Tristan's example linux shell.\n");
			printf("These shell commands are defined internally.\n");
			printf("help -prints this screen so you can see available shell commands.\n");
			printf("cd -changes directories to specified path; if not given, defaults to home.\n");
			printf("exit -closes the example shell.\n");
			printf("[input] > [output] -pipes input file into output file\n\n");
			printf("And more! If it's not explicitly defined here( or in the documentation for the assignment)");
			printf("then the command should try to be executed by launchProcesses.\n");
			printf("That's how we get ls -la to work here!\n\n");
			//we straigth up lied here, as cd defaults home is not req to be implemented
			//and therefore was not.
		}
	}
}