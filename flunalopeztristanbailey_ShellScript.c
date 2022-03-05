/**
 * flunalopez_tristanbailey_ShellScript:
 * 	Program simulates a shell with basic functionaility in the console.
 * 		Such as cd, exit, help, execvp(ls, touch, mkdir, etc...)
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
#include <sys/types.h>

// Macros
#define ARRAY_MAXSIZE 10000

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

int main(int argc, char* argv[]) {

	//make a pid_t for the main process
	pid_t mainProc;

	// Variables
	char command[ARRAY_MAXSIZE+1]; 	// String with command to run from batch file.
	char* outputFileName = ""; 	// Name of file to send command output to.
	char* tokens[ARRAY_MAXSIZE + 1];	// 2D array of strings from tokenizing input command.
	char* outputTokens[ARRAY_MAXSIZE + 1]; 	// Tokens to output.
	bool isExits;	// Determines whether to exit from program (negligible for batch mode).
	bool isRedirect;	// Determines if command output should be piped to seperate file.
	FILE* inputFile = stdin; // File with commands to feed in.

	// Test if running in match mode
	if (argc > 2) { 
		printError();
		return 0;
	}
	else{
		bool is_batch;
		// Test if file can be accessed and change input stream
		if (argc == 2){
			if((inputFile = fopen(argv[1], "r")) == NULL) {
				printError();
				return 0;
			}
			//batch mode active
			is_batch = true;
		}
		else{
			//batch mode inactive
			is_batch = false;
		}
		// Run commands in batch mode
		//gets each line from the file till the EOF

		//if it is not batch then wile short circuits to always being true
		//it will then prompt and read user input
		//if it is batch then input will be taken from the file till 'exit' or EOF
		while(!is_batch || fgets(command, sizeof(command), inputFile) != NULL) {
			promptUser(is_batch);			// Prompt user with shell prefix
			//get user input if not batch mode
			if(!is_batch){
				fgets(command, sizeof(command), inputFile);
			}

			//remove the newline char
			if(command[strlen(command)-1] == '\n')
				command[strlen(command)-1] = '\0';
			//remove the caruage return (it appears at end of fgets str)
			if(command[strlen(command)-1] == '\r')
				command[strlen(command)-1] = '\0';
			//print the command from file if in batch mode
			if(is_batch){
				printf("\n%s\n", command);
			}

			//execute command
			outputFileName = executeCommand(command, &isRedirect, tokens, outputTokens, &isExits);

			if(isExits)
			{
				fclose(inputFile);
				free(outputFileName);
				kill(0, SIGTERM);
			}

			// Test if output file proved
			if (strcmp(outputFileName, "") != 0) {
				FILE* outputFile; // File pointer to output file

				// Test if we can access output file
				if ((outputFile = fopen(outputFileName, "w")) == NULL) {
					printError();
					return 1;
				}

				// Write output tokens to output file
				for(int i = 0; outputTokens[i]; i++) {
					fprintf(outputFile, "%s", outputTokens[i]);
				}
				fclose(outputFile);
				free(outputFileName);
			}
		}
		fclose(inputFile);
		return 0;
	}
	/*
	// Run in interactive mode
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

			char* commandDup = strdup(command);
			tokens_count = parseInput(command, tokens);	// Parse input
			
			// TEMP: Print out tokens to shell
			for (int i = 0; i < tokens_count; i++) {
			       printf("%s ", tokens[i]);
			}
			printf("\n");

			// Test for exit call
			if (exitProgram(tokens, tokens_count)) return 0;

			// TEMP: Test redirectCommand()
			bool isRedirect;
			char* outputTokens[ARRAY_MAXSIZE + 1];
			char* outputFilename = redirectCommand(">", commandDup, &isRedirect, tokens, outputTokens);

			if (isRedirect) {
				FILE* outputFile;
				if((outputFile = fopen(outputFilename, "w")) == NULL) {
					printError();
				}
				else {
					for(int i = 0; outputTokens[i] != NULL; i++) {
						fprintf(outputFile, "%s", outputTokens[i]);
					}
				}
			}
			free(commandDup);
			free(outputFilename);
		}
	}*/
	return 0;
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

	printf("\n%s@%s:%s $ ", username, hostname, cwd_path); // Prompt user
}

char* redirectCommand(char* special, char* line, bool* isRedirect, char* tokens[], char* outputTokens[]) {
	// Variables
	bool foundSpecial = false;
	char* outputFileName = (char*) malloc(sizeof(char) * (ARRAY_MAXSIZE + 1));
	char inputFileName[ARRAY_MAXSIZE + 1];
	int outputFileNameIndex = 0;
	int inputFileNameIndex = 0;

	outputFileName[0] = '\0';
	inputFileName[0] = '\0';

	// Split command
	for(int i = 0; line[i]; i++) {
		if (line[i] == '\0' || line[i] == '\n') continue;
		if (line[i] == '>') {
			if (foundSpecial == false) {
				foundSpecial = true;
				*isRedirect = true;
			}
			else {
				*isRedirect = false;
				//printError(); this is not needed
				strcpy(outputFileName, "");
				return outputFileName;
			}
		}
		else if (foundSpecial == false) {
			inputFileName[inputFileNameIndex] = line[i];
			inputFileNameIndex++;
		}
		else {
			outputFileName[outputFileNameIndex] = line[i];
			outputFileNameIndex++;
		}
	}

	inputFileName[inputFileNameIndex] = '\0';
	outputFileName[outputFileNameIndex] = '\0';

	// Remove leading whitespace
	char* trimmedInputs = inputFileName;
	char* trimmedOutputs = outputFileName;
	while(*trimmedInputs == ' ') trimmedInputs++;
	while(*trimmedOutputs == ' ') trimmedOutputs++;

	// Remove trailing whitespace
	char* trimmedInBack = trimmedInputs + strlen(trimmedInputs) - 1;
	char* trimmedOutBack = trimmedOutputs + strlen(trimmedOutputs) - 1;
	while(*trimmedInBack == ' ') {
		*trimmedInBack = '\0';
		trimmedInBack--;
	}
	while(*trimmedOutBack == ' ') {
		*trimmedOutBack = '\0';
		trimmedOutBack--;
	}

	// Test input/output counts
	char* inputFileTokens[ARRAY_MAXSIZE + 1];
	char* outputFileTokens[ARRAY_MAXSIZE + 1];
	int inputNum = parseInput(trimmedInputs, inputFileTokens);
	int outputNum = parseInput(trimmedOutputs, outputFileTokens);
	
	if (outputNum != 1) { // Too many output files found
		*isRedirect = false;
		printError();
		strcpy(outputFileName, "");
		return outputFileName;
	}
	if (inputNum != 1) { // Too many input files found
		if (strcmp(inputFileTokens[0], "cat\n") && inputNum == 2) { // Test if command in inputs
			strcpy(inputFileName, inputFileTokens[1]);
		}
		else { // Too many input files found
			*isRedirect = false;
			printError();
			strcpy(outputFileName, "");
			return outputFileName;
		}
	}


	// Get output tokens
	FILE* inputFile;
	char* readLine;
	ssize_t contentRead;
	size_t len = 0;
	unsigned int outputRow = 0;

	// Test if access to file is allowed
	inputFile = fopen(inputFileName, "r");
	if (inputFile == NULL) {
		*isRedirect = false;
		printError();
		strcpy(outputFileName, "");
		return outputFileName;
	}

	// Get contents within file
	while((contentRead = getline(&readLine, &len, inputFile)) != -1) {
		char* lineCopy = strdup(readLine);
		outputTokens[outputRow] = lineCopy;
		outputRow++;
	}
	fclose(inputFile);
	strcpy(outputFileName, trimmedOutputs);
	return outputFileName;
}

bool exitProgram(char* tokens[], int numTokens) {
	// Variables
	char* backOfToken = tokens[0] + strlen(tokens[0]);
	
	// Remove trailing newline (this is unecessary and doesn't work as intended)
	//	but it does not cause any issues either
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
	//do not execvp with these four commands
	if(isRedirect || (strcmp(tokens[0], "cd")*strcmp(tokens[0], "exit")*strcmp(tokens[0], "help")
	 == 0))
	{
		return;
	}
	//single command, no args
	if(numTokens == 1){
		//store child process id
		pid_t child;

		//create tokens list to feed to execvp, must end with 0
		char* temp[2];
		temp[0] = tokens[0];
		temp[1] = 0;
		int result;

		//child process faild to launch
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
				exit(1);
			}
		}
		//parent process
		else{
			//have parent wait unless execvp failes or child fails
			while(!(result < 0) && wait(NULL) != child){;}
		}
	}
	//single command, many args
	else{
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
					exit(1);
				}
			}
			//parent process
			else{
				//have parent wait unless execvp failes or child fails
				while(!(result < 0) && wait(NULL) != child){;}
			}
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
	char* out_fname = (char* ) malloc(sizeof(char) * (ARRAY_MAXSIZE + 1)); // String to save the output file name to.
	strcpy(out_fname, "");
	char* clone = strdup(cmd);

	char* is_redirect = (strchr(cmd, '>'));
	//printf("%c\n", *is_redirect);
	int num_tokens = 0;
	
	//add '\n' for batch executions
	strcat(cmd, "\n");
	
	//check for redirect
	if(is_redirect != NULL){
		//was a redirect so save output file name
		char* temp;
		temp = redirectCommand(">", clone, isRedirect, tokens, outputTokens);
		strcpy(out_fname, temp);
		free(temp);
	}
	else{
		isRedirect = false;
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
			printf("And more! If it's not explicitly defined here (or in the documentation for the assignment)");
			printf("then the command should try to be executed by launchProcesses.\n");
			printf("That's how we get ls -la to work here!\n\n");
			//we straigth up lied here, as cd defaults home is not req to be implemented
			//and therefore was not.
		}
	}
}