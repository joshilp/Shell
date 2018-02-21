// Shell starter file
// You may make any changes to any part of this file.

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#define COMMAND_LENGTH 1024
#define NUM_TOKENS (COMMAND_LENGTH / 2 + 1)
#define HISTORY_DEPTH 10


// initialize history[][] and history_counter to keep track of command history
char history[HISTORY_DEPTH][COMMAND_LENGTH];
int history_counter = 0;

//set disable-randomization off

/**
 * Command Input and Processing
 */

//returns stored tokens
//used for debugging
void debug_tokens(char *tokens[])
{
	// DEBUG: Dump out arguments:
	for (int i = 0; tokens[i] != NULL; i++)
	{
		write(STDOUT_FILENO, "   Token: ", strlen("   Token: "));
		write(STDOUT_FILENO, tokens[i], strlen(tokens[i]));
		write(STDOUT_FILENO, "\n", strlen("\n"));
	}
}


//returns values stored in history[][]
//used for debugging
void phistory()
{
	printf("printing phistory\n");
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 6; j++ )
		{
			printf("%c when ",history[i][j]);
			printf("i: %d j: %d\n",i,j );
		}
	}
	printf("\n");
}


//record history of each command into history[][]
void record_history(char *tokens[])
{
	// printf("recording history\n");

	//initialize variables to store index in history
	//and variable findNULL to keep track index within tokens[]
    int h_index = history_counter % 10;
	int findNULL = 0;

	//reset memory for history
	memset(history[h_index], 0, strlen(history[h_index]));
	//debug_tokens(tokens);
	//phistory();

	for (int i = 0; i < COMMAND_LENGTH; i++)
	{
		//find \0 to be replaced with space
		//if space found, iterate to next index in tokens[]
		if (tokens[0][i] == '\0')
		{
			history[h_index][i] = ' ';
			findNULL++;

			//break if nothing stored
			if(tokens[findNULL] == NULL)
			{
				break;
			}
		}

		//copy tokens[] to history[][]
		else
		{
			history[h_index][i] = tokens[0][i];
		}

	}
}



void print_history()
{
	// initialize variables
	int h_size = HISTORY_DEPTH;
	int first = (history_counter % 10);
	int index = history_counter-10;

	// intialize array to store history command number
	// and its command
	char h_index[h_size];
	char h_cmd[sizeof(history)];

	//set variables for history 10 and under
	if (history_counter < 11)
	{
		h_size = history_counter;
		first = 0;
		index = 0;
	}

	int i = 0;

	//loop to print history
	while (i < h_size)
	{
		// print index values
		sprintf(h_index, "%d\t", index+1);
    	write(STDOUT_FILENO, h_index, strlen(h_index));

    	// store full command into h_cmd to be printed
		for(int j = 0; j < COMMAND_LENGTH; j++)
	    {
	    	h_cmd[j] = history[first][j];
	    }

	    // print command next to index
    	write (STDOUT_FILENO, h_cmd,strlen(h_cmd));
    	write(STDOUT_FILENO, "\n", strlen("\n"));	

    	// iterate variables
    	first = (first + 1) % 10;
    	index++;
    	i++;
	}

	// memset(h_index, 0, sizeof(h_index));
	// memset(h_cmd, 0, sizeof(h_cmd));

}

int run_history(char *tokens[])
{
	// printf("running history\n");
	// initialize variables
	// run = 0 and up for index values
	// run = -1 to flag as bad index value
	int first = history_counter - 9;
	int last = history_counter;
	int run = 0;

	//intialize variables if less than 10 and under stored commands
	if(history_counter <= 10)
	{
		first = 1;
		last = history_counter;
	}

	// printf("first: %d\n", first );
	// printf("last: %d\n", last );

	// check for command !
	if (tokens[0][0] == '!')
	{
		//printf("found !\n");
		//check for input after !
		if(tokens[0][1] == '\0')
		{
			run = -1;
		}

		//check for 0 value, flag run as -1
		if(tokens[0][1] == '0')
		{
			run = -1;
		}

		if(tokens[0][1] == '!')
		{
			//printf("found !!\n");
			if(tokens[0][2] != '\0')
			{
				run = -1;
			}

			else
			{
				run = last;
			}
		}

		//cast string as integer to get index value to be run
		else
		{
			char *char_val;
			char_val = &tokens[0][1];
			run = atoi(char_val);
		}


	}

	// return -1 to flag invalid index value
	if (run < 0 || run < first || run > last)
	{
		write(STDOUT_FILENO, "Invalid command.", strlen("Invalid command."));
		write(STDIN_FILENO, "\n", strlen("\n"));
		run = -1;
		//printf("returning -1\n");
		return (run);
	}

	// printf("run: %d\n",run);
	// printf("returning: %d\n",run % 10);
	// return index value to be run
	return ((run-1) % 10);

}

// handles CTRL C in shell to run print_history
void handle_SIGINT()
{
	write(STDOUT_FILENO, "\n", strlen("\n"));
	print_history();
}

/*
 * Tokenize the string in 'buff' into 'tokens'.
 * buff: Character array containing string to tokenize.
 *       Will be modified: all whitespace replaced with '\0'
 * tokens: array of pointers of size at least COMMAND_LENGTH/2 + 1.
 *       Will be modified so tokens[i] points to the i'th token
 *       in the string buff. All returned tokens will be non-empty.
 *       NOTE: pointers in tokens[] will all point into buff!
 *       Ends with a null pointer.
 * returns: number of tokens.
 */
int tokenize_command(char *buff, char *tokens[])
{
	int token_count = 0;
	_Bool in_token = false;
	int num_chars = strnlen(buff, COMMAND_LENGTH);
	for (int i = 0; i < num_chars; i++) {
		switch (buff[i]) {
		// Handle token delimiters (ends):
		case ' ':
		case '\t':
		case '\n':
			buff[i] = '\0';
			in_token = false;
			break;

		// Handle other characters (may be start)
		default:
			if (!in_token) {
				tokens[token_count] = &buff[i];
				token_count++;
				in_token = true;
			}
		}
	}
	tokens[token_count] = NULL;
	return token_count;
}



/**
 * Read a command from the keyboard into the buffer 'buff' and tokenize it
 * such that 'tokens[i]' points into 'buff' to the i'th token in the command.
 * buff: Buffer allocated by the calling code. Must be at least
 *       COMMAND_LENGTH bytes long.
 * tokens[]: Array of character pointers which point into 'buff'. Must be at
 *       least NUM_TOKENS long. Will strip out up to one final '&' token.
 *       tokens will be NULL terminated (a NULL pointer indicates end of tokens).
 * in_background: pointer to a boolean variable. Set to true if user entered
 *       an & as their last token; otherwise set to false.
 */
void read_command(char *buff, char *tokens[], _Bool *in_background)
{
	*in_background = false;

	// Read input
	int length = read(STDIN_FILENO, buff, COMMAND_LENGTH-1);

	// if (length < 0) {
	// 	perror("Unable to read command from keyboard. Terminating.\n");
	// 	exit(-1);
	// }
	if ( (length < 0) && (errno !=EINTR) )
	{
		perror("Unable to read command. Terminating.\n");
		exit(-1); /* terminate with error */
	}

	// Null terminate and strip \n.
	buff[length] = '\0';
	if (buff[strlen(buff) - 1] == '\n') {
		buff[strlen(buff) - 1] = '\0';
	}




	// Tokenize (saving original command string)
	int token_count = tokenize_command(buff, tokens);
	if (token_count == 0) {
		return;
	}

	// Extract if running in background:
	if (token_count > 0 && strcmp(tokens[token_count - 1], "&") == 0) {
		*in_background = true;
		tokens[token_count - 1] = 0;
	}
}

/**
 * Main and Execute Commands
 */
int main(int argc, char* argv[])
{

	//intialize buffer and tokens
	char input_buffer[COMMAND_LENGTH];
	char *tokens[NUM_TOKENS];

	// FOR COMMANDS
	char currdir[COMMAND_LENGTH];
	int history_val = 0;


	// set up the signal handler
	struct sigaction handler;
	handler.sa_handler = handle_SIGINT;
	handler.sa_flags = 0;
	sigemptyset(&handler.sa_mask);
	sigaction(SIGINT, &handler, NULL);



	while (true) 
	{

		// Get command
		// Use write because we need to use read() to work with
		// signals, and read() is incompatible with printf().

		// prints to show current directory at each command line
		getcwd(currdir, sizeof(currdir));
		write(STDOUT_FILENO, currdir, strlen(currdir));
		write(STDOUT_FILENO, "> ", strlen("> "));

		// get input
		_Bool in_background = false;
		read_command(input_buffer, tokens, &in_background);


	


		// // DEBUG: Dump out arguments:
		// for (int i = 0; tokens[i] != NULL; i++) {
		// 	write(STDOUT_FILENO, "   Token: ", strlen("   Token: "));
		// 	write(STDOUT_FILENO, tokens[i], strlen(tokens[i]));
		// 	write(STDOUT_FILENO, "\n", strlen("\n"));
		// }
		// if (in_background) {
		// 	write(STDOUT_FILENO, "Run in background.", strlen("Run in background."));
		//  write(STDOUT_FILENO, "\n", strlen("\n"));	
		// }

		


		//4. Internal Commands
		// only run these commands if valid input
		if (tokens[0] != NULL)
		{
			//records history for any command that
			//does not begin with '!'
			if (tokens[0][0] != '!')
			{
				record_history(tokens);
				history_counter++;
			}

			//checks for first input of !
			if (tokens[0][0] == '!')
			{
				//gets index value from history
				//if value is -1, will not run as it is invalid
				//if value is > 0, will tokenize that index value
				history_val = run_history(tokens);
				if (history_val >= 0)
				{
					tokenize_command(history[history_val],tokens);
					record_history(tokens);
					history_counter++;
				}
			}

			//checks for input exit
			//and exits
			if (strcmp(tokens[0], "exit") == 0)
			{
				exit(0);
			}

			//checks for input pwd
			//prints current directory
			else if (strcmp(tokens[0], "pwd") == 0)
			{
				
				getcwd(currdir, sizeof(currdir));
				write(STDOUT_FILENO, currdir, strlen(currdir));
				write(STDOUT_FILENO, "\n", strlen("\n"));
			}

			//checks for input cd
			else if (strcmp(tokens[0], "cd") == 0)
			{
				//checks for valid input
				if (tokens[1] != NULL)
				{
					//gets success value of chdir
					int changedir = chdir(tokens[1]);
					
					//prints error for invalid directory
					if (changedir == -1)
					{
						write(STDOUT_FILENO, tokens[1], strlen(tokens[1]));
						write(STDOUT_FILENO, " is an invalid directory.", strlen(" is an invalid directory."));
						write(STDOUT_FILENO, "\n", strlen("\n"));
					}
				}
			}

			//checks for input history and prints list
			else if (strcmp(tokens[0], "history") == 0)
			{
				// printf("printing history from main\n");
				print_history();

			}
		

			// 3. Creating Child Process
			// Steps For Basic Shell:
			// 1. Fork a child process
			else
			{
				pid_t pid;
				pid = fork();
				// printf ("Child %d intialized\n", pid);

				//check for invalid child process
				if (pid < 0) 
				{
					write(STDOUT_FILENO, "fork() failed.", strlen("fork() failed."));
					exit(-1);
				}

				// 2. Child process invokes execvp() using results in token array.
				// runs if successful child
				if (pid == 0) 
				{ 
					int execvp_val = execvp(tokens[0],tokens);
					if ( execvp_val == -1)
					{
		                write(STDOUT_FILENO, "execvp failed\n", strlen("execvp failed\n"));
						exit(-1);
					}
					// else
					// {
		   			// write(STDOUT_FILENO, "execvp successful\n", strlen("execvp successful\n"));
					// }
				}

				// 3. If in_background is false, parent waits for
				//    child to finish. Otherwise, parent loops back to
				//    read_command() again immediately.
				if (in_background == false)
				{
					waitpid(pid,NULL,0);
					//printf ("Child %d Completed\n", pid);
				}

				while (waitpid(-1, NULL, WNOHANG) > 0)
				{
					; // do nothing.
				}
			}
		}
	
	// printf("\nhistory_counter: %d\n",history_counter);
	// write(STDOUT_FILENO, &history[history_counter], strlen(history[history_counter]));
	// write(STDOUT_FILENO, "\n", strlen("\n"));
		
	}

	return 0;
}

