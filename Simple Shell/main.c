#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <signal.h>
#include <time.h>

/*
    Trim any white space in the beginning of the string
*/
char *trimLeadingWhiteSpace(char *text)
{
    while(isspace((unsigned char)*text))
        text++;

    if(*text == 0)
        return text;

    return text;
}



/*
    Get User Input from user and print current directory
*/
void getUserInput(char *command)
{
    strcpy(command, "");
    do
    {
        printCurrentDirectory();
        gets(command);
        command = trimLeadingWhiteSpace(command);
    }
    while(!strcmp(command, ""));

}

/*

    Print current directory

*/
void printCurrentDirectory()
{
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    printf("User@Machine:~%s$", cwd);
}

/*
    Process the command by user and raise a flag if it is not blocking command (contains '&')
    then executes the command
*/

void processCommand(char *commandKey, char *arguments[])
{
    int isBlocking = 1;
    int i = 0;
    for(i = 0; i < 10 && arguments[i] != NULL; i++)
    {
        if(strcmp(arguments[i], "&") == 0)
        {
            isBlocking = 0;
            arguments[i] = NULL;
            break;
        }
    }

    executeCommand(commandKey, arguments, isBlocking);
}

/*
    Execute the command by forking a child process to execute. The parent process will block if it's a block instruction.
*/

void executeCommand(char *commandKey, char *arguments[], int isBlocking)
{
    int status;
    if(strcmp(commandKey, "exit") == 0)
        exit(0);


    if(strcmp(commandKey, "cd") == 0)
    {
        chdir(arguments[1]);
    }
    else
    {
        pid_t pid = fork();
        if (pid == 0)//child
        {
            execvp(commandKey, arguments);
            printf("Unknown Command: %s\n",commandKey);
            exit(0);
        }
        else if (pid > 0)//Parent
        {
            if(isBlocking)
                wait(NULL);

            return;
        }
        else
        {
            printf("Failed to fork");
            return;
        }
    }
}

/*
    Split the command on the white spaces and put it in an array called arguments
*/
void tokenizeCommand(char *command, char **commandKey, char *arguments[])
{
    char *commandToken;

    commandToken = strtok(command," ");
    *commandKey = commandToken;

    int i = 0;
    while(commandToken != NULL)
    {
        arguments[i++] = commandToken;
        commandToken = strtok(NULL, " ");
    }
    arguments[i] = NULL;
}

/*
    Handler for the signal when a child exits
*/

void handleChildTermination(int signal)
{
    logTerminatedChild();
}


/*
    Write to a file when a child exits and log the time it was terminated
*/
void logTerminatedChild()
{

    FILE *f = fopen("terminatedChildren.txt", "a");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    fprintf(f, "Child Has Been Terminated At %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    fclose(f);
}

/*
    Start of the program
*/
int main()
{
    signal(SIGCHLD, handleChildTermination);
    char command[30];
    char *commandKey;
    char *arguments[10];

    while(1)
    {
        getUserInput(command);
        tokenizeCommand(command, &commandKey, arguments);
        processCommand(commandKey, arguments);
    }
    return 0;
}
