#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <signal.h>
#include <string.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "LineParser.h" //As Said in the Instruction

// Constants
#define Max_String 2048
int debug_mode = 0;

// Function Declaration
void execute(cmdLine *cmdLine);
void debugSearch(int argc, char **argv);
int stoierr(char *str);

// Function Definition
// Main Function
int main(int argc, char **argv)
{

    char cwd[PATH_MAX];
    char input[Max_String]; // Buffer

    for (int i = 1; i < argc; i++)
        setDebugMode(argv[i]);
    

    while (1)
    {
        if (getcwd(cwd, sizeof(cwd)) == NULL)
        {
            perror("getcwd failed\n");
            break;
        }

        printf("cwd: %s", cwd);

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            perror("Error reading input\n");
            break;
        }

        // Parse the input
        cmdLine *cmdLine = parseCmdLines(input);

        if (cmdLine == NULL)
        {
            if (feof(stdin))
                exit(0);

            perror("Error command that gets NULL");
            continue;
        }

        if (!cmdLine->argCount)
        {
            fprintf(stderr, "0 arguments given in the Command\n");
            freeCmdLines(cmdLine);
            continue;
        }

        if (strcmp(cmdLine->arguments[0], "quit") == 0)
        {
            freeCmdLines(cmdLine);
            exit(0);
        }

        if (strcmp(cmdLine->arguments[0], "cd") == 0)
        {
            if (chdir(cmdLine->arguments[1]) == -1)
            {
                perror("Having trouble in changing the dir\n");
            }

            freeCmdLines(cmdLine);
            continue;
        }


        // Alarm Command
        else if (strcmp(cmdLine->arguments[0], "alarm") == 0)
        {
            // Check if the command has 2 arguments, if not print an error message and continue
            if(cmdLine->argCount == 2)
            {
                int processID = stoierr(cmdLine->arguments[1]);

                if(processID != -1){
                    kill(processID, SIGCONT); // Send the signal to the process
                }
            }

            else
            {
                fprintf(stderr, "Error in the number of arguments\n");
            }
        }

        // Blast Command
        else if(strcmp(cmdLine->arguments[0], "blast") == 0){

            // Check if the command has 2 arguments, if not print an error message and continue
            if(cmdLine->argCount == 2)
            {
                int processID = stoierr(cmdLine->arguments[1]);

                if(processID != -1){
                    kill(processID, SIGINT); // Send the signal to the process
                }
            }

            else
            {
                fprintf(stderr, "Error in the number of arguments\n");
            }

        }

        // Execute the command
        else
        {   
            
            execute(cmdLine);
            // Free the memory
            freeCmdLines(cmdLine);
            cmdLine = NULL;
        }


    }

    return 0;
}

// Execute the command
void execute(cmdLine *cmdLine)
{
    int temp, stat_loc;
    int pid = fork();

    if (pid == -1)
    {
        perror("Error in Forking\n");
        freeCmdLines(cmdLine);
        _exit(EXIT_FAILURE); // Get out of the program
    }

    // Child Process
    if (pid == 0)
    {
        //Debug Checker
        
        // Check if there is an input redirection
        if (cmdLine->inputRedirect)
        {
            close(STDIN_FILENO);
            temp = open(cmdLine->inputRedirect, O_RDONLY | O_CREAT, 0777);
            // Check if the file is opened, if not print an error message and exit temp=-1
            if (temp == -1)
            {
                perror("Error in opening the file\n");
                freeCmdLines(cmdLine);
                _exit(EXIT_FAILURE);
            }
        }

        // Check if there is an output redirection
        if (cmdLine->outputRedirect)
        {
            close(STDOUT_FILENO);
            temp = open(cmdLine->outputRedirect, O_WRONLY | O_CREAT, 0777);
            // Check if the file is opened, if not print an error message and exit b=-1
            if (temp == -1)
            {
                perror("Error in opening the file\n");
                freeCmdLines(cmdLine);
                _exit(EXIT_FAILURE);
            }
        }

        // Check if the command is executable
        if (execv(cmdLine->arguments[0], cmdLine->arguments) == -1)
        {
            perror("Error executing command");
            freeCmdLines(cmdLine);
            exit(EXIT_FAILURE);
        }
    }

    // Parent Process.. Wait for the child to finish pid>0
    else
    {
        if (cmdLine->blocking)
        {
            if (waitpid(pid, &stat_loc, 0) == -1)
            {
                perror("Error during waitpid");
                freeCmdLines(cmdLine);
                exit(EXIT_FAILURE);
            }
        }
    }
}

// Debug Mode Function - Looking for -d or -D
void setDebugMode(const char *argv)
{
    if (strcmp(argv, "-D") == 0 || strcmp(argv, "-d") == 0)
        debug_mode = 1;
}

// Convert String to Integer
int stoierr(char *str)
{
    int val;
    // Convert the string to integer and entering him into val
    sscanf(str, "%d", &val);

    // Check if the conversion was successful, if not print an error message and exit
    if (val == 0)
    {
        fprintf(stderr, "Error in converting string to integer\n");
        exit(EXIT_FAILURE); // return -1;
    }

    return val;
}