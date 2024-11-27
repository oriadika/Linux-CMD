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

#define Max_String 2048

int main(int argc, char **argv)
{

    char cwd[PATH_MAX];
    char input[Max_String]; // Buffer

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

        cmdLine *cmdLine = parseCmdLines(input);

        if (cmdLine == NULL)
        {
            if (feof(stdin))
                exit(0);

            fprintf(stderr, "Error command that gets NULL\n");
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
            break;
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

        else if (strcmp())
    }

    return 0;
}

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
